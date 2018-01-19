import os, sys, csv
import argparse


def csv_extract_lines(csv_reader, only_first_column=False):
    lines = [line for line in csv_reader if len(line) > 0 and line[0] != '']
    if only_first_column:
        return [line[0] for line in lines]
    return lines


def get_probe_results(sequencesFile, resultsFile, probesFile, filterFile=None):
    print("Sequences File: '{0}'".format(sequencesFile))
    print("Results File:   '{0}'".format(resultsFile))
    print("Probes File:    '{0}'".format(probesFile))
    assert(os.path.isfile(sequencesFile))
    assert(os.path.isfile(resultsFile))
    assert(os.path.isfile(probesFile))
    if filterFile is not None:
        assert(os.path.isfile(filterFile))

    # remove old files
    resultsDir, resultsFileName = os.path.split(resultsFile)
    resultsFileName, ext = os.path.splitext(resultsFileName)
    probes_res_file = os.path.join(resultsDir, resultsFileName + '_probes' + ext)
    if os.path.isfile(probes_res_file):
        os.remove(probes_res_file)
    sequencesFileName = os.path.split(sequencesFile)[1]
    sequencesFileName, ext = os.path.splitext(sequencesFileName)
    probes_seq_file = os.path.join(resultsDir, sequencesFileName + '_probes' + ext)
    if os.path.isfile(probes_seq_file):
        os.remove(probes_seq_file)

    # get filter probes
    with open(probesFile) as f:
        csvf = csv.reader(f)
        probeIDs = csv_extract_lines(csvf, only_first_column=True)
    print("Probe IDs:")
    print("  " + repr(probeIDs))
    assert(len(probeIDs)>0)

    # get filter sequences
    filterSequences = []
    if filterFile is not None:
        with open(filterFile) as f:
            csvf = csv.reader(f)
            filterSequences = csv_extract_lines(csvf, only_first_column=True)
            print("Filter Sequences:")
            print("  " + repr(filterSequences))
            assert(len(filterSequences)>0)

    # get sequences info
    sequencesFrames = []
    with open(sequencesFile) as f:
        csvf = csv.reader(f)
        sequencesFrames = csv_extract_lines(csvf)
    sequencesHeader = sequencesFrames[0]
    sequencesLines = sequencesFrames[1:]  # remove header

    # get results info
    resultsLines = []
    with open(resultsFile) as f:
        csvf = csv.reader(f)
        resultsLines = [line for line in csvf]
    resultsHeader = resultsLines[0]
    resultsLines = resultsLines[1:]

    # filter by sequences
    if len(filterSequences) > 0:
        filterRes = []
        filterSeq = []
        for lineRes, lineSeq in zip(resultsLines, sequencesLines):
            seqID = lineSeq[0]
            if seqID in filterSequences:
                filterRes.append(lineRes)
                filterSeq.append(lineSeq)
        resultsLines = filterRes
        sequencesLines = filterSeq

    # filter by probes IDs
    linesProbesRes = [resultsHeader]
    linesProbesSeq = [sequencesHeader]
    for lineRes, lineSeq in zip(resultsLines, sequencesLines):        
        seqTargetID = lineSeq[4]
        if seqTargetID in probeIDs:
            linesProbesRes.append(lineRes)
            linesProbesSeq.append(lineSeq)

    with open(probes_res_file, 'w') as f:
        csvf = csv.writer(f)
        csvf.writerows(linesProbesRes)

    with open(probes_seq_file, 'w') as f:
        csvf = csv.writer(f)
        csvf.writerows(linesProbesSeq)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Extract probes from results file')
    parser.add_argument(dest='sequencesFile', metavar='sequencesFile', type=str)
    parser.add_argument(dest='resultsFile', metavar='resultsFile', type=str)
    parser.add_argument(dest='probesFile', metavar='probesFile', type=str)
    parser.add_argument('-f', dest='filterFile', metavar='filterFile', type=str, default=None,
                        help='sequences to preserve for evaluation (ignore others)')
    args = parser.parse_args()
    get_probe_results(args.sequencesFile, args.resultsFile, args.probesFile, args.filterFile)
