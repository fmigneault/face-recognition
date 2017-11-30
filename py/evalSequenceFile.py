# -*- coding: utf-8 -*-
"""
Calls sequence tracks evaluation functions for specified input CSV files.
See 'evalSequenceFilePerf' function.

Required Files
==============

  (1) RESULTS:    containing the algorithm recognition trajectory-based output (transaction-level also supported)
  (2) SEQUENCES:  containing the reference sequences informations employed for algorithm evaluation

File Field Formats
==================

  'RESULTS'
  ---------

  CSV file must have be separated as:

    <basic_results><best_results>{<results>(i)}    for i=TRACK_COUNT

    where <basic_results>:
        SEQUENCE_TRACK_ID,SEQUENCE_NUMBER,FRAME_NUMBER,TRACK_COUNT,TARGET_COUNT

    where <best_results>:
        if <BACKWARD_COMPATIBILITY>=True:
            ,TRACK_NUMBER,BEST_LABEL,BEST_SCORE,ROI_TL_X,ROI_TL_Y,ROI_BR_X,ROI_BR_Y
        otherwise:
            ,TRACK_NUMBER,BEST_LABEL,BEST_SCORE_RAW,BEST_SCORE_ACC,ROI_TL_X,ROI_TL_Y,ROI_BR_X,ROI_BR_Y

    where each <results>(i):
        {<result_target>(j)}    for j=TARGET_COUNT

        where each <result_target>(j):
            if <BACKWARD_COMPATIBILITY>=True:
                ,TARGET_LABEL,TARGET_SCORE
            otherwise:
                ,TARGET_LABEL,TARGET_SCORE_RAW,TARGET_SCORE_ACC

  'SEQUENCES'
  -----------

  CSV file must be separated as:

    SEQUENCE_NAME,FRAME_NUMBER_NEW,FRAME_NUMBER_OLD,TRACK_NUMBER,GT_LABEL,LEFT_EYE_X,LEFT_EYE_Y,RIGHT_EYE_X,RIGHT_EYE_Y

Details
=======

First line is considered as the header by default and is ignored no matter its value.
Set corresponding <noHeaderSequences>/<noHeaderResults>=True to avoid automatic removal of the first line.

After handling removal of headers, 'SEQUENCES' and 'RESULTS' files must both have exactly the same amount of lines.
It is expected by the script that the lines correspond to the same entries (input 'SEQUENCES' => output 'RESULTS')

Counter-validation heuristic is applied with ground truth eye positions against found track ROI points TL,BR to
identify the expected tracks to evaluate in case that more than one ROI was registered on any given frame.
The maximum number of matching eye positions with ROIs along a corresponding track (eyes within ROI) indicates the
retained track if more than one was available within a corresponding frame sequence.
The script supposes that only one track per sequence is expected, others are ignored and removed for merged output.

Outputs
=======

Primary evaluation results are appended to a new file (-eval) with the corresponding and merged line input information.
Redundant information is removed for this merged file and additional tracks filtered by ROI ground truth are removed.
This file output can can skipped using <outputMergedEvalFile>=False.

Secondary evaluation results are after generated in a separate files (-perf).
If using backward compatibility (<evalBackwardCompatibility>=True), only trajectory-level results are produced.
Otherwise, corresponding (-perf-transaction) or (-perf-trajectory) are produced according to specified modes:

    <evalTransactionLevel> / <evalTrajectoryLevel>

Both modes can be employed simultaneously if enabling the appropriate expected fields (not backward compatible).
All generated files will be located under the same location as the input 'RESULTS' file.

Backward Compatibility
======================

This option allows processing older results files that did not combine both transaction/trajectory-level scores.
When backward compatibility is employed, trajectory-level evaluation is mandatory (only this one is possible).
"""

from printUtils import *
import os
import csv
import sys
import copy


TYPE_RES = "RESULTS"
TYPE_SEQ = "SEQUENCES"
TYPE_NORM = "NORMALIZED"

NORM_TRACK_LENGTH = 20  # circular-buffer track accumulation size to employ for normalization (default: [W=20])

FILTER_FLAG = "FILTER"

HEADER_INDEX_RESULTS = {
 0: {
    "SEQUENCE_TRACK_ID":0,
    "SEQUENCE_NUMBER":  1,
    "FRAME_NUMBER":     2,
    "TRACK_COUNT":      3,
    "TARGET_COUNT":     4,
    "TRACK_NUMBER":     5,
    "BEST_LABEL":       6,
    "BEST_SCORE_RAW":   7, # indexes change from here, fields before can omit backward compatibility in 'getFieldIndex'
    "BEST_SCORE_ACC":   8,
    "ROI_TL_X":         9,
    "ROI_TL_Y":         10,
    "ROI_BR_X":         11,
    "ROI_BR_Y":         12,
    "TARGET_LABEL":     13,
    "TARGET_SCORE_RAW": 14,
    "TARGET_SCORE_ACC": 15,
    },
 # backward compatibility
 1: {
    "SEQUENCE_NAME":    0,
    "SEQUENCE_NUMBER":  1,
    "FRAME_NUMBER":     2,
    "TRACK_COUNT":      3,
    "TARGET_COUNT":     4,
    "TRACK_NUMBER":     5,
    "BEST_LABEL":       6,
    "BEST_SCORE":       7,
    "ROI_TL_X":         8,
    "ROI_TL_Y":         9,
    "ROI_BR_X":         10,
    "ROI_BR_Y":         11,
    "TARGET_LABEL":     12,
    "TARGET_SCORE":     13,
    },
}

HEADER_INDEX_SEQUENCES = {
    "SEQUENCE_NAME":    0,
    "FRAME_NUMBER_NEW": 1,
    "FRAME_NUMBER_OLD": 2,
    "TRACK_NUMBER":     3,
    "GT_LABEL":         4,
    "LEFT_EYE_X":       5,
    "LEFT_EYE_Y":       6,
    "RIGHT_EYE_X":      7,
    "RIGHT_EYE_Y":      8
}

# evaluation thresholds (same amount required for transaction/trajectory)
THRESHOLD_SCORE_ACC_MAX = 20.0
THRESHOLD_SCORE_ACC_INCR = 0.1
THRESHOLD_SCORE_RAW_MAX = 1.0
THRESHOLD_SCORE_RAW_INCR = THRESHOLD_SCORE_RAW_MAX / (THRESHOLD_SCORE_ACC_MAX / THRESHOLD_SCORE_ACC_INCR)


def evalSequenceFilePerf(csvSequencesFilePath, csvResultsFilePath, filterSequencesFilePath=None,
                         noHeaderSequences=False, noHeaderResults=False, evalNormalizedScores=False,
                         evalTransactionLevel=True, evalTrajectoryLevel=True,
                         outputMergedEvalFile=True, evalBackwardCompatibility=False, verbosity=2):
    """
    Main function that executes the complete RESULTS video multi-track file evaluation filtered using GT SEQUENCES file.
    """
    # backward compatibility with trajectory-level evaluation only
    # otherwise, at least one of transaction/trajectory-level evaluation required
    assert((not evalBackwardCompatibility and (evalTransactionLevel or evalTrajectoryLevel)) or
           (evalBackwardCompatibility and evalTrajectoryLevel))
    if evalBackwardCompatibility:
        if evalTransactionLevel:
            verbose(verbosity, 1, "WARNING: 'evalTransactionLevel' turned off for 'evalBackwardCompatibility' mode")
            evalTransactionLevel = False
        if evalNormalizedScores:
            verbose(verbosity, 1, "WARNING: 'evalNormalizedScores' turned off for 'evalBackwardCompatibility' mode")
            evalNormalizedScores = False

    # read files lines and get basic information
    allLines = readSequenceFileInput(csvSequencesFilePath, csvResultsFilePath, noHeaderSequences, noHeaderResults)
    nTargets = int(getFieldInfo(allLines, lambda x: x[0][1], ["TARGET_COUNT", TYPE_RES, evalBackwardCompatibility]))
    assert(nTargets > 0)

    # check for extra filter sequences file
    if filterSequencesFilePath == "" or filterSequencesFilePath is None:
        filterSequences = []
    else:
        assert(os.path.isfile(filterSequencesFilePath))
        with open(filterSequencesFilePath, 'r') as filterFile:
            filterSequences = filterFile.readlines()
            filterSequences = [s.strip() for s in filterSequences]

    # parse lines (group by corresponding sequences)
    verbose(verbosity, 1, "Grouping lines per corresponding sequences...")
    sequenceGroupedLines = dict()
    for lineSeq, lineRes in allLines:
        seqKey = lineRes[getFieldIndex("SEQUENCE_TRACK_ID", TYPE_RES, evalBackwardCompatibility)]  # unique
        sequenceGroupedLines.setdefault(seqKey, list()).append({TYPE_SEQ: lineSeq, TYPE_RES: lineRes})

    # filter invalid tracks from sequences
    # (ensure to keep only one track per sequence, validation with GT eye position)
    verbose(verbosity, 1, "Filtering tracks with ground truths and from 'filter' file...")
    verbose(verbosity, 1, "Filtered sequences from 'filter' file: " + str(filterSequences))
    for seqKey in sequenceGroupedLines:
        seqLines = sequenceGroupedLines[seqKey]
        filterBestTrack(seqLines, evalBackwardCompatibility)
        filterFileTrack(seqLines, filterSequences, evalBackwardCompatibility)

    # apply normalization if requested
    evalTypes = [TYPE_RES]
    if evalNormalizedScores:
        verbose(verbosity, 1, "Applying score normalization to sequences...")
        addNormalizedScoreResults(sequenceGroupedLines)
        evalTypes.append(TYPE_NORM)

    # generate for multiple thresholds and prepare containers, default values, etc.
    thresholdsTransac = generateThresholds(0, THRESHOLD_SCORE_RAW_MAX, THRESHOLD_SCORE_RAW_INCR)
    thresholdsTraject = generateThresholds(0, THRESHOLD_SCORE_ACC_MAX, THRESHOLD_SCORE_ACC_INCR)
    threshTransacPerf = dict()
    threshTrajectPerf = dict()
    emptyTrackResults = generateEmptyResultLine(nTargets, evalBackwardCompatibility)
    trackCountIndex = getFieldIndex("TRACK_COUNT", TYPE_RES, evalBackwardCompatibility)
    maxLength = getFieldIndex("TARGET_SCORE_ACC", TYPE_RES, evalBackwardCompatibility, 0, nTargets - 1, nTargets) + 1
    nEvaluations = nTargets + 1   # +1 for 'best'
    for et in evalTypes:
        threshTransacPerf[et] = dict()
        threshTrajectPerf[et] = dict()
        if evalTransactionLevel:
            for e in range(nEvaluations):
                threshTransacPerf[et][e] = dict()
                for t in thresholdsTransac:
                    threshTransacPerf[et][e][t] = ConfusionMatrix()
        if evalTrajectoryLevel:
            for e in range(nEvaluations):
                threshTrajectPerf[et][e] = dict()
                for t in thresholdsTraject:
                    threshTrajectPerf[et][e][t] = ConfusionMatrix()
        if et == TYPE_RES:
            for tr in sequenceGroupedLines:
                for frameLine in sequenceGroupedLines[tr]:
                    # add default empty track values if no results are available for a given frame along tracks
                    if int(frameLine[TYPE_RES][trackCountIndex]) < 1:
                        frameLine[TYPE_RES].extend(emptyTrackResults)

    try:
        if evalTransactionLevel and not evalTrajectoryLevel:
            verbose(verbosity, 1, "Running evaluations for thresholds: (transaction) " +
                                  "[0," + str(THRESHOLD_SCORE_RAW_MAX) + "]...")
        elif not evalTransactionLevel and evalTrajectoryLevel:
            verbose(verbosity, 1, "Running evaluations for thresholds: (trajectory) " +
                                  "[0,"  + str(THRESHOLD_SCORE_ACC_MAX) + "]...")
        else:
            verbose(verbosity, 1, "Running evaluations for thresholds:" + \
                                  " (transaction) [0," + str(THRESHOLD_SCORE_RAW_MAX) + "] |" + \
                                  " (trajectory) [0,"  + str(THRESHOLD_SCORE_ACC_MAX) + "]...")
        title = ""
        transacDigits = len(str(int(1/THRESHOLD_SCORE_RAW_INCR)))
        transacFormat = "." + str(transacDigits) + "f"
        verbose(verbosity, 2, hide_cursor)  # avoid cursor blinking across progress bar
        for threshTransac, threshTraject in zip(thresholdsTransac, thresholdsTraject):
            if verbosity > 1:
                if evalTransactionLevel and not evalTrajectoryLevel:
                    title = "  Processing transaction-level threshold T=" + \
                            format(threshTransac, transacFormat).ljust(6)
                elif not evalTransactionLevel and evalTrajectoryLevel:
                    title = "  Processing trajectory-level threshold T=" + \
                            str(threshTraject).ljust(6)
                else:
                    title = "  Processing transaction|trajectory-level thresholds T=" + \
                            format(threshTransac, transacFormat) + "|" + str(threshTraject).ljust(6)
            progress = 0
            total = len(sequenceGroupedLines)
            verbose(verbosity, 2, printProgressBar, progress, total, prefix=title, length=30)

            # evaluate performance of each track-based lines per threshold, for every evaluation type
            for tr in sequenceGroupedLines:
                for et in evalTypes:
                    for frameLine in sequenceGroupedLines[tr]:
                        # remove any result ConfusionMatrix from previous iteration
                        if len(frameLine[et]) > maxLength:
                            del frameLine[et][maxLength:]
                        # transaction evaluation of each frame-line
                        # append ConfusionMatrix with current threshold, for every best/target-label/score combination
                        evalLineConfusionMatrix(frameLine, threshTransac, threshTraject, evalBackwardCompatibility, et)

                        # accumulate transaction-level evaluation results along each frame, per threshold
                        if evalTransactionLevel:
                            for e in range(nEvaluations):
                                # accumulate corresponding (TP,FP,TN,FN) across frames
                                threshTransacPerf[et][e][threshTransac] += frameLine[et][maxLength + e]

                    # accumulate trajectory-level evaluation results along each track, per threshold
                    if evalTrajectoryLevel:
                        evalResults = evalTrackConfusionMatrix(sequenceGroupedLines[tr], evalBackwardCompatibility, et)
                        for e in range(nEvaluations):
                            # accumulate corresponding (TP,FP,TN,FN) across tracks
                            threshTrajectPerf[et][e][threshTraject] += evalResults[e]

                progress += 1
                verbose(verbosity, 2, printProgressBar, progress, total, prefix=title, length=30)
        verbose(verbosity, 2, print_flush)
    except Exception:
        raise   # rethrow
    finally:    # reset cursor visibility
        verbose(verbosity, 2, show_cursor)

    # prepare performance summary constants
    verbose(verbosity, 1, "Running summary evaluations...")
    summaryValuesTransac = dict()
    summaryValuesTraject = dict()
    summaryHeader = []
    summaryLabels = ["_BEST"] + ["_TARGET_" + str(t) for t in range(nTargets)]
    for label in summaryLabels:
        summaryHeader += ["pAUC(5%)" + label, "pAUC(10%)" + label, "pAUC(20%)" + label, "AUC" + label, "AUPR" + label]
    summaryRanks = list(range(1, nTargets + 1))
    summaryHeader += ["RANK_" + str(k) for k in summaryRanks]
    perfHeader = ["T"] + ConfusionMatrix.fields() * nEvaluations
    filePathExt = os.path.splitext(csvResultsFilePath)

    # evaluate performance summary for each evaluation type, target and transaction/trajectory-level
    for et in evalTypes:
        summaryValuesTransac[et] = list()
        summaryValuesTraject[et] = list()
        for e in range(nEvaluations):
            if evalTransactionLevel:
                summaryValuesTransac[et] += [calcAUC(threshTransacPerf[et][e], pFPR=0.05),
                                             calcAUC(threshTransacPerf[et][e], pFPR=0.10),
                                             calcAUC(threshTransacPerf[et][e], pFPR=0.20),
                                             calcAUC(threshTransacPerf[et][e]),
                                             calcAUPR(threshTransacPerf[et][e])]
            if evalTrajectoryLevel:
                summaryValuesTraject[et] += [calcAUC(threshTrajectPerf[et][e], pFPR=0.05),
                                             calcAUC(threshTrajectPerf[et][e], pFPR=0.10),
                                             calcAUC(threshTrajectPerf[et][e], pFPR=0.20),
                                             calcAUC(threshTrajectPerf[et][e]),
                                             calcAUPR(threshTrajectPerf[et][e])]
        if evalTransactionLevel:
            rankings = calcRank(sequenceGroupedLines, rank=summaryRanks, mode=2, evalType=et)
            summaryValuesTransac[et] += list(rankings.values())
        if evalTrajectoryLevel:
            rankings = calcRank(sequenceGroupedLines, rank=summaryRanks, mode=0, evalType=et)
            summaryValuesTraject[et] += list(rankings.values())

        fileEvalType = "-norm" if et == TYPE_NORM else ""

        # output transaction-level evaluation performances
        if evalTransactionLevel:
            verbose(verbosity, 1, "Writing transaction-level evaluation results to file...")
            outputFile = filePathExt[0] + "-perf-transaction" + fileEvalType + filePathExt[1]
            writeResultPerfFile(outputFile, summaryHeader, summaryValuesTransac[et],
                                perfHeader, thresholdsTransac, threshTransacPerf[et], nEvaluations)

        # output trajectory-level evaluation performances
        if evalTrajectoryLevel:
            verbose(verbosity, 1, "Writing trajectory-level evaluation results to file...")
            outputFile = filePathExt[0] + "-perf-trajectory" + fileEvalType + filePathExt[1]
            writeResultPerfFile(outputFile, summaryHeader, summaryValuesTraject[et],
                                perfHeader, thresholdsTraject, threshTrajectPerf[et], nEvaluations)

        # write file by adding merged sequences and results information
        if outputMergedEvalFile:
            verbose(verbosity, 1, "Writing merged sequences and results to file...")
            outputFile = filePathExt[0] + "-eval" + fileEvalType + filePathExt[1]
            writeMergedEvalFile(outputFile, sequenceGroupedLines, evalBackwardCompatibility, et)


def readSequenceFileInput(csvSequencesFilePath, csvResultsFilePath, noHeaderSequences=False, noHeaderResults=False):
    """
    Read, check and handle input arguments for 'evalSequenceFilePerf'.
    Returns the read lines as a list of corresponding line 'SEQUENCES' and line 'RESULTS'.
    """
    assert(os.path.isfile(csvResultsFilePath))
    assert(os.path.isfile(csvSequencesFilePath))
    with open(csvResultsFilePath, 'r') as csvFileIn:
        csvReader = csv.reader(csvFileIn)
        allLinesRes = [line for line in csvReader]
        assert(len(allLinesRes) > 1)
        if not noHeaderResults:
            del allLinesRes[0] # remove header
    with open(csvSequencesFilePath, 'r') as csvFileIn:
        csvReader = csv.reader(csvFileIn)
        allLinesSeq = [line for line in csvReader]
        assert(len(allLinesSeq) > 1)
        if not noHeaderSequences:
            del allLinesSeq[0] # remove header
    assert(len(allLinesRes) == len(allLinesSeq))
    assert(len(allLinesRes) > 0)
    return [list(i) for i in zip(allLinesSeq,allLinesRes)]


def writeMergedEvalFile(mergedFilePath, sequenceGroupedLines, backComp=False, evalType=TYPE_RES):
    """
    Writes the merged file combining 'SEQUENCES' and 'RESULTS' fields, removing duplicate data and invalid tracks.
    Information must have been preprocessed, this function only handles writing operations, not the actual processing.
    """
    with open(mergedFilePath, 'wb') as csvFileOut:
        csvWriter = csv.writer(csvFileOut)
        if backComp:
            bestHeader = ["BEST_LABEL","BEST_SCORE"]
        else:
            bestHeader = ["BEST_LABEL","BEST_SCORE_RAW","BEST_SCORE_ACC"]
        sequenceHeader = [h for h in sorted(HEADER_INDEX_SEQUENCES, key=lambda i: HEADER_INDEX_SEQUENCES[i])]
        mergedHeader = sequenceHeader \
                     + [FILTER_FLAG] \
                     + ["SEQUENCE_TRACK_ID","SEQUENCE_NUMBER","FRAME_NUMBER","TRACK_COUNT_FILTERED","TRACK_COUNT",
                        "TARGET_COUNT","TRACK_NUMBER"] \
                     + bestHeader \
                     + ["ROI_TL_X","ROI_TL_Y","ROI_BR_X","ROI_BR_Y"]
        iTrackCount = getFieldIndex("TRACK_COUNT", evalType, backComp)
        targetCount = getTargetInfo(sequenceGroupedLines, backComp)[0]
        for t in range(targetCount):
            mergedHeader += ["TARGET_LABEL_" + str(t)]
            if backComp:
                mergedHeader += ["TARGET_SCORE_" + str(t)]
            else:
                mergedHeader += ["TARGET_SCORE_RAW_" + str(t), "TARGET_SCORE_ACC_" + str(t)]
        maxLength = len(mergedHeader)
        csvWriter.writerow(mergedHeader)
        for seq in sorted(sequenceGroupedLines):
            for line in sequenceGroupedLines[seq]:
                mergedLine = []
                mergedLine.extend(line[TYPE_SEQ])
                mergedLine.extend([line[FILTER_FLAG]])
                mergedLine.extend(line[evalType][:iTrackCount])  # add up to original 'TRACK_COUNT'
                mergedLine.extend([1])                           # add 'TRACK_COUNT_FILTERED' = 1
                mergedLine.extend(line[evalType][iTrackCount:])  # rest of results line
                csvWriter.writerow(mergedLine[:maxLength])       # don't output ConfusionMatrix objects


def writeResultPerfFile(perfFilePath, summaryHeader, summaryValues, perfHeader, perfThresholds, perfResults, perfCount):
    """
    Writes the summary and detailed (per-threshold) results of the specified containers.
    Information must have been preprocessed, this function only handles writing operations, not the actual processing.
    """
    with open(perfFilePath, 'wb') as csvFileOut:
        csvWriter = csv.writer(csvFileOut)
        csvWriter.writerow(summaryHeader)
        csvWriter.writerow(summaryValues)
        csvWriter.writerow(perfHeader)
        for t in perfThresholds:
            row = [t]
            for e in range(perfCount):
                row.extend(perfResults[e][t].asList())
            csvWriter.writerow(row)


def getFieldIndex(field, fileType, backComp=False, trackIndex=0, targetIndex=0, targetCount=0):
    """
    Returns the index of the requested <HEADER> field for the corresponding <fileType>={RESULTS,SEQUENCES}
    For track results, zero based indexes can be specified to obtain index of i-th track, and j-th target score.
    In the case of track or target indexes, the target count must also be specified for proper indexing.
    If the field cannot be found, returns None.
    """
    if fileType == TYPE_RES or fileType == TYPE_NORM:
        header = HEADER_INDEX_RESULTS[int(backComp)]
        if backComp:
            if field in ["BEST_SCORE_RAW","BEST_SCORE_ACC"]:
                field = "BEST_SCORE"
            elif field in ["TARGET_SCORE_RAW","TARGET_SCORE_ACC"]:
                field = "TARGET_SCORE"
            if field == "SEQUENCE_TRACK_ID":
                field = "SEQUENCE_NAME"
        index = header[field]
        if field in ["SEQUENCE_NAME","SEQUENCE_TRACK_ID","SEQUENCE_NUMBER","FRAME_NUMBER","TRACK_COUNT","TARGET_COUNT"]:
            return index
        if index < header["TARGET_LABEL"]:
            targetIndex = 0
        targetOffset = max(header.values()) - header["ROI_BR_Y"]
        resultOffset = header["TARGET_LABEL"] - header["TRACK_NUMBER"]
        return (resultOffset + targetOffset * targetCount) * trackIndex + targetOffset * targetIndex + index
    elif fileType == TYPE_SEQ and field in HEADER_INDEX_SEQUENCES:
        return HEADER_INDEX_SEQUENCES[field]
    return None


def getFieldInfo(infoContainer, mapping, fieldParams):
    """
    Using a top-level container (ex: SequenceGroupedLines, RawResultsLines, etc.) and a mapping function to reach data
    contained in this container, the method returns the field information specified with field parameters corresponding
    exactly to ordered input parameters of 'getFieldIndex' function.

    Usage:
    ------

    Given <sequencesGroupedLines> being a dictionary of per-track list of frame-lines, each being a dictionary
    of 'RESULTS' and 'SEQUENCES' information such that:

        sequencesLines['SEQUENCE_TRACK_ID'][0]['RESULTS'][getFieldInfo('TARGET_LABEL', 'RESULTS', False, 0, 1, 5)]

     >> 'ID0004' (2nd target label contained on the first frame-line 'RESULTS' of the 'SEQUENCE_TRACK_ID' track)

    The function can be employed like the following:

        fieldInfo = ['TARGET_LABEL', 'RESULTS', False, 0, 1, 5]
        getFieldInfo(sequencesLines, lambda x: x['SEQUENCE_TRACK_ID'][0]['RESULTS'], fieldInfo)

     >> 'ID0004'
    """
    return mapping(infoContainer)[getFieldIndex(*fieldParams)]


def getFrameLineInfo(frameLine, field, fileType, fieldParams):
    """
    Shortcut method to extract information from higher level 'frameLine' container.
    (ie: dictionary of corresponding {TYPE_RES: [<res_list_info>], TYPE_SEQ: [<seq_list_info>]} lines)

        info = frameLine[TYPE_RES][getFieldIndex(field, TYPE_RES, backComp, trackIndex, targetIndex, targetCount)]

    Becomes:

        info = getFrameLineInfo(frameLine, field, TYPE_RES, backComp, trackIndex, targetIndex, targetCount)
    """
    return frameLine[fileType][getFieldIndex(field, fileType, *fieldParams)]


def getTargetInfo(sequenceFrameLines, backComp=False):
    """
    Returns the number of targets and a list of their corresponding labels as tuple.

    This function searches for target labels given that 'RESULTS' fields can sometime contain 'empty' lines.
    Therefore, a static line index can yield undesired '' labels. Target labels must sometimes be searched over
    multiple sequences or frame-lines. The first valid found label values are returned.
    """

    # find target labels, searching for the first valid entry since 'RESULTS' fields can contain 'empty' lines
    # in such case, target labels are not set, we loop sequences and their corresponding lines until labels are found
    targetLabels = []
    for seqID in sequenceFrameLines:
        seq = sequenceFrameLines[seqID]
        for i, line in enumerate(seq):
            mapping = lambda x: x[i][TYPE_RES]
            nTracks = int(getFieldInfo(seq, mapping, ["TRACK_COUNT", TYPE_RES, backComp]))
            if nTracks < 1: continue  # skip empty frame-line, check next one in track sequence
            nTargets = int(getFieldInfo(seq, mapping, ["TARGET_COUNT", TYPE_RES, backComp]))
            for t in range(nTargets):
                label = getFieldInfo(seq, mapping, ["TARGET_LABEL", TYPE_RES, backComp, 0, t, nTargets])
                if label == '': break # skip invalid labels (if any)
                targetLabels.append(label)
            if len(targetLabels) == nTargets:
                return nTargets, targetLabels
    raise Exception("Could not find targets labels in any frame/track") # should never happend (empty results)


def isEyesInROI(roi, eyes):
    """
    Evaluates the correspondence between the specified ROI and the GT eye positions
    Expects ROI as tuple (tl_x,tl_y,br_x,br_y) as eye position as tuple (left_eye_x,left_eye_y,right_eye_x,right_eye_y)
    """
    eyeBetween = ((eyes[0] + eyes[2]) / 2, (eyes[1] + eyes[3]) / 2)
    return eyeBetween[0] >= roi[0] and eyeBetween[0] <= roi[2] and eyeBetween[1] >= roi[1] and eyeBetween[1] <= roi[3]


def filterBestTrack(sequenceFrameLines, backComp=False):
    """
    Filters the tracks keeping only the most probable one given GT eye positions and tracked ROIs.
    Function requires input <sequenceFrameLines> as a list of {TYPE_SEQ: line, TYPE_RES: line} for every frame-line.
    The specified lines are updated directly if more than one track is found.
    Tracks with lowest confidence (less probability to be expected track) are removed from the frame-lines.
    """
    # find number of tracks per line
    trackCounts = list()
    for frameLines in sequenceFrameLines:
        nTracks = int(frameLines[TYPE_RES][getFieldIndex("TRACK_COUNT", TYPE_RES, backComp)])
        trackCounts.append(nTracks)

    # group corresponding tracks across lines and evaluate confidence
    # process only if more than one track, otherwise current sequence is left untouched
    if any(n > 1 for n in trackCounts):
        groupedTracksConfidence = dict()
        nTargets = int(getFieldInfo(sequenceFrameLines, lambda x: x[0][TYPE_RES], ["TARGET_COUNT", TYPE_RES, backComp]))
        for line, n in zip(sequenceFrameLines, trackCounts):
            for i in range(n):
                # evaluate matching between GT eye positions and tracked ROI
                trackNum = line[TYPE_RES][getFieldIndex("TRACK_NUMBER", TYPE_RES, backComp, i, 0, nTargets)]
                lEyeX = int(line[TYPE_SEQ][getFieldIndex("LEFT_EYE_X",  TYPE_SEQ, backComp)])
                lEyeY = int(line[TYPE_SEQ][getFieldIndex("LEFT_EYE_Y",  TYPE_SEQ, backComp)])
                rEyeX = int(line[TYPE_SEQ][getFieldIndex("RIGHT_EYE_X", TYPE_SEQ, backComp)])
                rEyeY = int(line[TYPE_SEQ][getFieldIndex("RIGHT_EYE_Y", TYPE_SEQ, backComp)])
                eyes = (lEyeX, lEyeY, rEyeX, rEyeY)
                x1 = int(line[TYPE_RES][getFieldIndex("ROI_TL_X", TYPE_RES, backComp)])
                y1 = int(line[TYPE_RES][getFieldIndex("ROI_TL_Y", TYPE_RES, backComp)])
                x2 = int(line[TYPE_RES][getFieldIndex("ROI_BR_X", TYPE_RES, backComp)])
                y2 = int(line[TYPE_RES][getFieldIndex("ROI_BR_Y", TYPE_RES, backComp)])
                roi = (x1, y1, x2, y2)
                match = isEyesInROI(roi, eyes)
                groupedTracksConfidence.setdefault(trackNum, 0)
                groupedTracksConfidence[trackNum] += int(match)
        # evaluate confidence of track (sum of matches) to find most probable track
        bestTrack = -1
        trackConf = -1
        for track in groupedTracksConfidence:
            conf = groupedTracksConfidence[track]
            if conf > trackConf:
                trackConf = conf
                bestTrack = track
        # filter to keep only the best track if best was found
        if bestTrack != -1:
            baseTrackIdx = getFieldIndex("TRACK_NUMBER", TYPE_RES, backComp, 0)
            baseTrackEnd = getFieldIndex("TRACK_NUMBER", TYPE_RES, backComp, 1, 0, nTargets)
            for fl in range(len(sequenceFrameLines)):
                # update first track with best track (move values) if not already positioned as first
                line = sequenceFrameLines[fl][TYPE_RES]
                if len(line) > baseTrackIdx:                # skip if no track on line
                    if line[baseTrackIdx] != bestTrack:     # update if best not located as first track
                        for i in range(1, trackCounts[fl]):
                            nextTrack = getFieldIndex("TRACK_NUMBER", TYPE_RES, backComp, i)
                            if line[nextTrack] == bestTrack:
                                # move whole track values (best => first)
                                bestOffset = i * (baseTrackEnd - baseTrackIdx)
                                line[baseTrackIdx:baseTrackEnd] = line[baseTrackIdx+bestOffset:baseTrackEnd+bestOffset]
                                break # found and updated
                # trim excess information on line pass first 'best' track (update sequence directly)
                sequenceFrameLines[fl][TYPE_RES] = line[:baseTrackEnd]


def filterFileTrack(sequenceLines, filterSequences, backComp=False):
    """
    Adds 'FILTER_FLAG' value to each line of the track sequence according to specified list of filter sequences.
    """
    seqID = getFieldInfo(sequenceLines, lambda x: x[0][TYPE_RES], ["SEQUENCE_TRACK_ID", TYPE_RES, backComp])
    flag = seqID in filterSequences
    for line in sequenceLines:
        line[FILTER_FLAG] = flag


def generateEmptyResultLine(targetCount, backComp=False):
    """
    Returns the default (invalid) values to pad a 'RESULTS' line that did not contain any track scores.
    Expects empty track lines format as:

        SEQUENCE_TRACK_ID,SEQUENCE_NUMBER,FRAME_NUMBER,TRACK_COUNT,TARGET_COUNT   (no results here)

    Adds the missing results such that all labels are '', all scores are 0.0, and all ROI positions are -1.
    """
    if backComp:
        return [-1, '', 0.0, -1, -1, -1, -1] + ['', 0.0] * targetCount              # only trajectory-level scores
    else:
        return [-1, '', 0.0, 0.0, -1, -1, -1, -1] + ['', 0.0, 0.0] * targetCount    # transaction+trajectory scores


def addNormalizedScoreResults(sequenceGroupedLines):
    """
    Adds a 'NORMALIZED' section to 'sequenceGroupedLines' to contain normalized scores from the 'RESULTS' section.
    Scores are normalized across all 'frameLines' of the whole 'RESULTS' for transaction-level using MIN-MAX rule,
    then trajectory-level scores are re-evaluated and normalized using these found scores.
    """
    minScore = +float('inf')
    maxScore = -float('inf')
    backComp = False
    targetCount, targetLabels = getTargetInfo(sequenceGroupedLines, backComp)
    bestLabelIndex = getFieldIndex("BEST_LABEL", TYPE_RES, backComp)
    bestScoreIndex = getFieldIndex("BEST_SCORE_RAW", TYPE_RES, backComp)
    bestCumulIndex = getFieldIndex("BEST_SCORE_ACC", TYPE_RES, backComp)
    trackCountIndex = getFieldIndex("TRACK_COUNT", TYPE_RES, backComp)
    trackIdIndex = getFieldIndex("TRACK_NUMBER", TYPE_RES, backComp)
    emptyFrameLineResults = generateEmptyResultLine(targetCount, backComp)

    # find min/max
    for seqKey in sequenceGroupedLines:
        for frameLine in sequenceGroupedLines[seqKey]:
            # skip lines without any track score results
            if int(frameLine[TYPE_RES][trackCountIndex]) < 1:
                continue
            for iTarget in range(targetCount):
                iScoreIndex = getFieldIndex("TARGET_SCORE_RAW", TYPE_RES, backComp, 0, iTarget, targetCount)
                iScoreValue = float(frameLine[TYPE_RES][iScoreIndex])
                minScore = min(minScore, iScoreValue)
                maxScore = max(maxScore, iScoreValue)
    rangeScore = float(maxScore - minScore)

    # apply min/max
    for seqKey in sequenceGroupedLines:
        for iFrame, frameLine in enumerate(sequenceGroupedLines[seqKey]):
            frameLine[TYPE_NORM] = copy.deepcopy(frameLine[TYPE_RES])
            # add default empty track values if no results are available for a given frame along tracks
            if int(frameLine[TYPE_NORM][trackCountIndex]) < 1:
                frameLine[TYPE_NORM].extend(emptyFrameLineResults)
                continue
            for iTarget in range(targetCount):
                # normalize min/max raw score (transaction-level)
                iScoreIndex = getFieldIndex("TARGET_SCORE_RAW", TYPE_RES, backComp, 0, iTarget, targetCount)
                iScoreValue = float(frameLine[TYPE_RES][iScoreIndex])
                frameLine[TYPE_NORM][iScoreIndex] = (iScoreValue - minScore) / rangeScore
                # update trajectory-level score with normalized transaction-level score (using default Circular-Buffer)
                iCumulIndex = getFieldIndex("TARGET_SCORE_ACC", TYPE_NORM, backComp, 0, iTarget, targetCount)
                frameLine[TYPE_NORM][iCumulIndex] = frameLine[TYPE_NORM][iScoreIndex]
                # add accumulated scores up to previous track-frame to the current frame score when available
                frameTrackID = frameLine[TYPE_NORM][trackIdIndex]
                if iFrame > 0:
                    prevFrame = sequenceGroupedLines[seqKey][iFrame - 1]
                    prevTrackID = prevFrame[TYPE_NORM][trackIdIndex]
                    if prevTrackID == frameTrackID:  # ensure that we are within the same track (maybe is a new track)
                        cumulPrevFrame = prevFrame[TYPE_NORM][iCumulIndex]
                        frameLine[TYPE_NORM][iCumulIndex] += cumulPrevFrame
                # gradually remove accumulated scores out of the same continuous track-frame accumulation window
                if iFrame >= NORM_TRACK_LENGTH:  # ensure that at least enough frames are passed to do following checks
                    initFrame = sequenceGroupedLines[seqKey][iFrame - NORM_TRACK_LENGTH]
                    initTrackID = initFrame[TYPE_NORM][trackIdIndex]
                    if initTrackID == frameTrackID:  # ensure that we are within the same track (maybe track was lost)
                        cumulInitFrame = initFrame[TYPE_NORM][iScoreIndex]
                        frameLine[TYPE_NORM][iCumulIndex] -= cumulInitFrame
                # update current line 'BEST' transaction/trajectory-level scores and label across targets
                if iTarget == 0:
                    frameLine[TYPE_NORM][bestScoreIndex] = frameLine[TYPE_NORM][iScoreIndex]
                    frameLine[TYPE_NORM][bestCumulIndex] = frameLine[TYPE_NORM][iCumulIndex]
                    frameLine[TYPE_NORM][bestLabelIndex] = targetLabels[iTarget]
                else:
                    if frameLine[TYPE_NORM][iScoreIndex] > frameLine[TYPE_NORM][bestScoreIndex]:
                        frameLine[TYPE_NORM][bestScoreIndex] = frameLine[TYPE_NORM][iScoreIndex]
                    if frameLine[TYPE_NORM][iCumulIndex] > frameLine[TYPE_NORM][bestCumulIndex]:
                        frameLine[TYPE_NORM][bestCumulIndex] = frameLine[TYPE_NORM][iCumulIndex]
                        frameLine[TYPE_NORM][bestLabelIndex] = targetLabels[iTarget]
                frameLine[TYPE_NORM][bestCumulIndex] = max(0, frameLine[TYPE_NORM][bestCumulIndex])


def evalLineConfusionMatrix(frameLine, thresholdTransac, thresholdTraject, backComp=False, evalType=TYPE_RES):
    """
    Appends ConfusionMatrix results to line using specified scores, label and GT.
    Appended results are in the following order:

      T{,<CM>(i)}{,<CM>(j)}    for i in {BEST_SCORE_RAW,TARGET_SCORE_RAW_1,...,TARGET_SCORE_RAW_n}  up to n=TARGET_COUNT
                               for j in {BEST_SCORE_ACC,TARGET_SCORE_ACC_1,...,TARGET_SCORE_ACC_n}  up to n=TARGET_COUNT

      Where CM(i) are computed with the transaction-level threshold and CM(j) with the trajectory-level threshold.
      ConfusionMatrix are ordered for all targets transaction-level threshold, then followed by all trajectory-level.

    If using backward compatibility, only trajectory-level matrices are computed and are also appended to the line end.
    If 'FILTER_FLAG' was set, an empty ConfusionMatrix object is appended instead of original results.
    """
    groundTruth = frameLine[TYPE_SEQ][getFieldIndex("GT_LABEL", TYPE_SEQ, backComp)]
    targetCount = int(getFieldInfo(frameLine, lambda x: x[evalType], ["TARGET_COUNT", evalType, backComp]))
    labelFields = ["BEST_LABEL"] + ["TARGET_LABEL"] * targetCount
    if backComp:
        scoreFields = [["BEST_SCORE"] + ["TARGET_SCORE"] * targetCount]
        thresholds = [thresholdTraject]
    else:
        scoreFieldsTransac = ["BEST_SCORE_RAW"] + ["TARGET_SCORE_RAW"] * targetCount
        scoreFieldsTraject = ["BEST_SCORE_ACC"] + ["TARGET_SCORE_ACC"] * targetCount
        scoreFields = [scoreFieldsTransac, scoreFieldsTraject]
        thresholds = [thresholdTransac, thresholdTraject]
    for threshold, fields in zip(thresholds, scoreFields):
        for i in range(len(labelFields)):
            # use i=-1 for 'BEST_TARGET' so that following targets get the proper zero-based indexes, 'BEST' ignores i
            label = frameLine[evalType][getFieldIndex(labelFields[i],  evalType, backComp, 0, i-1, targetCount)]
            score = float(frameLine[evalType][getFieldIndex(fields[i], evalType, backComp, 0, i-1, targetCount)])
            frameLine[evalType].append(ConfusionMatrix())
            if not frameLine[FILTER_FLAG]:
                frameLine[evalType][-1].TP = (groundTruth == label and score >= threshold)   # TP
                frameLine[evalType][-1].FP = (groundTruth != label and score >= threshold)   # FP
                frameLine[evalType][-1].TN = (groundTruth != label and score <  threshold)   # TN
                frameLine[evalType][-1].FN = (groundTruth == label and score <  threshold)   # FN


def evalTrackConfusionMatrix(trackFrameLines, backComp=False, evalType=TYPE_RES):
    """
    Evaluates the TP,FP,TN,FN along a complete track given its pre-evaluated (with 'evalLineConfusionMatrix')
    and corresponding list of line-frames (each appended with ConfusionMatrix frame-line transaction recognitions).
    ConfusionMatrix objects per-frame using accumulated scores are expected all appended AT THE END of processed lines.

    Found TP,FP,TN,FN conditions are set to 1, others are set to 0.
    Returns TP,FP,TN,FN of the whole track, for every best/target evaluation, as a list of ConfusionMatrix.

    If 'FILTER_FLAG' was set, empty ConfusionMatrix objects are appended instead of original results.
    """
    targetCount = int(getFieldInfo(trackFrameLines, lambda x: x[0][evalType], ["TARGET_COUNT", evalType, backComp]))
    filterFlag = trackFrameLines[0][FILTER_FLAG]
    evalSize = len(trackFrameLines[0][evalType])
    evalCount = targetCount + 1     # +1 for 'best'
    evalStart = evalSize - evalCount
    evalCM = list()
    for i in range(evalCount):
        evalCM.append(ConfusionMatrix())
        if not filterFlag:
            fn = True
            for frameLine in trackFrameLines:
                cm = frameLine[evalType][evalStart + i]
                # return result on first positive match found along track
                # (accumulation threshold was reached)
                if cm.TP:
                    evalCM[i].TP = 1
                    break
                if cm.FP:
                    evalCM[i].FP = 1
                    break
                # reset FN on first non-match (all frame-lines must indicate FN for missed target individual)
                fn = cm.FN & fn
            # if whole track was processed without any positive match,
            # return the appropriate retained negative condition
            if not (evalCM[i].FP or evalCM[i].TP):
                evalCM[i].FN = int(fn)
                evalCM[i].TN = int(not fn)
    return evalCM


class ConfusionMatrix:
    """
    Class to contain TP,FP,TN,FN values and performance calculation methods.
    """
    def __init__(self, cmAsList=None):
        if cmAsList is None:
            self.fromList([0, 0, 0, 0])
        else:
            self.fromList(cmAsList)
    def __iadd__(self, other):
        self.TP += other.TP
        self.FP += other.FP
        self.TN += other.TN
        self.FN += other.FN
        return self
    @staticmethod
    def fields():
        return ['TP', 'FP', 'TN' ,'FN', 'FPR', 'TPR', 'PPV']
    def fromList(self, cmAsList):
        self.TP = cmAsList[0]
        self.FP = cmAsList[1]
        self.TN = cmAsList[2]
        self.FN = cmAsList[3]
    def asList(self):
        return [self.TP, self.FP, self.TN, self.FN, self.FPR(), self.TPR(), self.PPV()]
    def FPR(self):
        if self.FP + self.TN == 0: return -1
        return self.FP / float(self.FP + self.TN)
    def TPR(self):
        if self.TP + self.FN == 0: return -1
        return self.TP / float(self.TP + self.FN)
    def PPV(self):
        if self.TP + self.FP == 0: return -1
        return self.TP / float(self.TP + self.FP)


def calcAUC(dictCM, pFPR=1.0):
    """
    Calculate area under the ROC curve.
    Requires a dict of ConfusionMatrix with corresponding thresholds as keys.
    Partial area under the ROC curve for FPR=[0,pFPR] points can be calculated if pFPR is specified.
    """
    try:
        if pFPR <= 0 or pFPR > 1: return -1
        # get (FPR,TPR) points from confusion matrices
        ROC = [(dictCM[t].FPR(), dictCM[t].TPR()) for t in dictCM]
        # filter invalid points and sort ascending points (sorting keys employ FPR located first in tuple, then TPR)
        ROC = sorted([p for p in ROC if p[0] != -1 and p[1] != -1])
        if len(ROC) < 2: return 0
        # filter up to pFPR for pAUC, interpolate last point for requested pFPR if required
        pROC = [p for p in ROC if p[0] <= pFPR]
        if pROC[-1][0] != pFPR and len(ROC) > len(pROC):
            iROC = len(pROC)
            pTPR = pROC[-1][1] + (pFPR - pROC[-1][0]) * (ROC[iROC][1] - pROC[-1][1]) / (ROC[iROC][0] - pROC[-1][0])
            pROC += [(pFPR, pTPR)]
        # trapezoidal rule of partial areas
        auc = sum([(pROC[i][1] + pROC[i-1][1]) * (pROC[i][0] - pROC[i-1][0]) / 2 for i in range(1, len(pROC))])
        # divide by pFPR requested to obtain result in range [0,1]
        return auc / pFPR
    except Exception as e:
        print("Exception occurred: [" + repr(e) + "]")
        return -1


def calcAUPR(dictCM):
    """
    Calculate area under the Precision-Recall curve.
    Requires a dictionary of ConfusionMatrix objects with corresponding thresholds as keys.
    """
    try:
        # reverse sort thresholds for (PPV,TPR) points in ascending order
        PR = [(dictCM[t].PPV(),dictCM[t].TPR()) for t in dictCM]
        # filter invalid points
        PR = sorted([p for p in PR if p[0] != -1 and p[1] != -1])
        if len(PR) < 2: return 0
        # get min(PPV) and corresponding TPR to handle missing points in top of PR (when PPV doesn't start at 0)
        # get min(TPR) to handle missing invalid points (div by 0,-1) at bottom of PR (when TPR doesn't reach 0)
        PPV_minPPV = 1
        TPR_minPPV = 1
        PPV_minTPR = 1
        TPR_minTPR = 1
        for ppv,tpr in PR:
            if ppv < PPV_minPPV:
                PPV_minPPV = ppv
                TPR_minPPV = tpr
            if tpr < TPR_minTPR:
                TPR_minTPR = tpr
                PPV_minTPR = ppv
        # add point (0, TPR_minPPV) to get horizontal line from PPV=0 to first available PPV_minPPV point on plot
        # add point (PPV_minTPR, 0) to get vertical line from last available TPR_minTPR point down to TPR=0
        PR = [(0, TPR_minPPV)] + PR + [(PPV_minTPR, 0)]
        # trapezoidal rule of partial area
        return sum([(PR[i][1] + PR[i-1][1]) * (PR[i][0] - PR[i-1][0]) / 2 for i in range(1, len(PR))])
    except Exception as e:
        print("Exception occurred: [" + repr(e) + "]")
        return -1


def calcRank(sequenceGroupedLines, rank=1, backComp=False, mode=0, evalType=TYPE_RES):
    """
    Calculates rank-k values based on target recognition scores contained in sequences-grouped lines (per-track).
    Calculates by default only rank-1. Other rank value(s) can be specified using individual integer or iterable.

        ex: rank=[1,3,5] will calculate rank-1, rank-3, and rank-5 over specified target scores.

    Note that rank-k must be in range [1,k] where k equals the number of available target scores.

    Parameter <mode> indicates how rank-k calculation must be evaluated:
        0: maximum per-target value along a given track using corresponding trajectory scores (ACC)
        1: maximum per-target value along a given track using corresponding transaction scores (RAW)
        2: individually for each frame of each track using transaction scores (RAW)

    Returns a dictionary of rank-k values for requested rank(s).
    """
    try:
        # k value(s) validation, number of targets is always available even in 'empty' results lines
        if type(rank) is int:
            rank = [rank]
        rank = sorted(rank)
        nTargets, targetLabels = getTargetInfo(sequenceGroupedLines, backComp)
        for k in rank: assert(k >= 1 and k <= nTargets)

        # create a list of {label: score} for every frame/track according to transaction/trajectory modes
        targetScores = []
        labelsGT = []
        for seq in sequenceGroupedLines:
            track = sequenceGroupedLines[seq]
            labelGT = track[0][TYPE_SEQ][getFieldIndex("GT_LABEL", TYPE_SEQ, backComp)]
            if labelGT not in targetLabels: continue  # skip cases where individual on track is not any of the tragets

            # trajectory evaluation using RAW/ACC scores
            if mode == 0 or mode == 1:
                labelsGT.append(labelGT)
                targetScores.append({})
                for t, label in zip(range(nTargets), targetLabels):
                    if   mode == 0: scoreField = "TARGET_SCORE_ACC"
                    elif mode == 1: scoreField = "TARGET_SCORE_RAW"
                    iTargetScore = getFieldIndex(scoreField, evalType, backComp, 0, t, nTargets)
                    # find max score along track for each corresponding target individual
                    for frame in track:
                        targetScores[-1][label] = max(targetScores[-1].get(label, 0),
                                                      float(frame[evalType][iTargetScore]))

            # transaction evaluation using RAW scores
            elif mode == 2:
                for frame in track:
                    labelsGT.append(labelGT)
                    targetScores.append({})
                    for t, label in zip(range(nTargets), targetLabels):
                        iTargetScore = getFieldIndex("TARGET_SCORE_RAW", evalType, backComp, 0, t, nTargets)
                        targetScores[-1][label] = float(frame[evalType][iTargetScore])

        # evaluate ranks percentage of correctly identified targets
        rank_k = dict([k,0] for k in rank)
        for ts, gt in zip(targetScores, labelsGT):
            # sort scores in descending order, and check for top k scores if target was properly identified
            topLabels = sorted(ts, key=lambda label: ts[label], reverse=True)
            topScores = [ts[label] for label in topLabels]
            for k in rank:
                if ts[gt] in topScores[0:k]: rank_k[k] += 1
        total = float(len(targetScores))
        for k in rank_k:
            rank_k[k] = float(rank_k[k]) / total
        return rank_k
    except Exception as e:
        print("Exception occurred: [" + repr(e) + "]")
        return -1


def generateThresholds(start, end, incr):
    """
    Generates a float value list going from 'start' to 'end' by 'incr' intervals
    """
    thresholds = list(range(start, int(end / incr) + 1))
    for i,t in enumerate(thresholds):
        thresholds[i] = round(t * incr, 6)
    return thresholds


if __name__ == "__main__":
    evalSequenceFilePerf(*sys.argv)
