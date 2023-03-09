# -*- coding: utf-8 -*-
"""
Generation of frame sequences using the information from one of the utilities in 'extractGroundTruthChokePoint'
    -> 'extractAllChokePointGT'
    -> 'combineSequencesChokePointGT'
"""

import os, shutil, csv, posixpath
import extractGroundTruthChokePoint as ex


"""
Using the obtained frame numbers with individual-specific tracks, copies the frames to a separate directory and
renames them in sequential incrementing names of format (%08d) to allow loading into VideoCapture, ffmpeg, etc.

Additionally outputs 3 information files (*):

    (1) "sequences-info.csv" CSV file formatted as:

        SEQUENCE,TRACK#,FRAME#_NEW,FRAME#_OLD,GT_LABEL

    (2) "sequences-path.txt" containing all sequence-track directories generated.

    (3) "sequences.log" containing all the copy calls for traceback.

    (*) only "sequences-info.csv" is updated if <onlyInfo> is True

"""
def generateSequencesChokePoint(pathChokePointRoot, outDirPath='', combineSequences=False, onlyInfo=False):

    # make paths and validate them
    pathChokePointRoot = pathChokePointRoot.replace('\\','/')
    pathGT = posixpath.join(pathChokePointRoot, "Ground Truth")
    pathVideos = posixpath.join(pathChokePointRoot, "Videos")
    if combineSequences:
        seqInfo = ex.combineSequencesChokePointGT(pathGT)
    else:
        seqInfo = ex.extractAllChokePointGT(pathGT)
    assert(os.path.isdir(pathChokePointRoot))
    assert(os.path.isdir(pathGT))
    assert(os.path.isdir(pathVideos))

    # cleanup or make output directory
    if outDirPath == '':
        outDirPath = posixpath.join(pathChokePointRoot, "generated")
    if not onlyInfo:
        if os.path.isdir(outDirPath):
            cleanDirectory(outDirPath)
        else:
            os.makedirs(outDirPath)

    # output file header
    outputFile = posixpath.join(outDirPath, "sequences-info.csv")
    with open(outputFile, 'w', newline='') as csvFile:
        fields = ["SEQUENCE","FRAME#_NEW","FRAME#_OLD","TRACK#","GT_LABEL",
                  "LEFT_EYE_X","LEFT_EYE_Y","RIGHT_EYE_X","RIGHT_EYE_Y"]
        csvWriter = csv.DictWriter(csvFile, fieldnames=fields)
        csvWriter.writeheader()

        outputLog = posixpath.join(outDirPath, "sequences.log")
        with open(outputLog, 'w') as logFile:

            # generate frames and add information to files
            for seqName in sorted(seqInfo):
                for track in seqInfo[seqName]:
                    trackID = "ID" + track["id"]
                    trackName = makeTrackName(track["track"], seqName, trackID)
                    if not onlyInfo:
                        os.makedirs(posixpath.join(outDirPath, trackName))
                    for f in range(len(track["frames"])):
                        frameNum = track["frames"][f]["number"]
                        frameInfo = track["frames"][f]["info"]
                        seqDirName = track["sequence"]

                        # execution copy and logging of frames unless not requested
                        if not onlyInfo:
                            framePathSrc = makeFramePath(frameNum, seqDirName, pathVideos)
                            framePathDst = makeFramePath(f, trackName, outDirPath)
                            shutil.copyfile(framePathSrc, framePathDst)
                            logFile.write("[" + framePathSrc + "] -> [" + framePathDst + "]\n")

                        # update information to CSV file
                        csvWriter.writerow({"SEQUENCE_NAME":    seqDirName,
                                            "FRAME_NUMBER_NEW": makeFrameName(f),
                                            "FRAME_NUMBER_OLD": makeFrameName(frameNum),
                                            "TRACK_NUMBER":     makeTrackName(track["track"]),
                                            "GT_LABEL":         trackID,
                                            "LEFT_EYE_X":       frameInfo["leftEye"][0],
                                            "LEFT_EYE_Y":       frameInfo["leftEye"][1],
                                            "RIGHT_EYE_X":      frameInfo["rightEye"][0],
                                            "RIGHT_EYE_Y":      frameInfo["rightEye"][1]
                                           })

    # update sequences list after generation unless not requested
    if not onlyInfo:
        outputFileListDir(outDirPath, "sequences-path.txt", "%08d.jpg")

"""
Removes all contents from within specified directory path, but doesn't remove the specified root directory.
"""
def cleanDirectory(dirPath):
    for content in os.listdir(dirPath):
        contentPath = posixpath.join(dirPath, content)
        if os.path.isfile(contentPath):
            os.remove(contentPath)
        else:
            shutil.rmtree(contentPath)

"""
Build all the path of every sequence frames generated as:
    <dirPath>[/<regexFileName>]

Outputs the resulting paths to <fileName> under <dirPath>.
"""
def outputFileListDir(dirPath, fileName, regexFileName=''):
    allFileDirs = os.listdir(dirPath)
    allDirs = sorted([posixpath.join(dirPath, d, regexFileName) + '\n'
                      for d in allFileDirs
                      if os.path.isdir(posixpath.join(dirPath, d))
                     ])
    filePath = posixpath.join(dirPath, fileName)
    with open(filePath, 'w') as outputFile:
        outputFile.writelines(allDirs)

"""
Returns the track number as '####' zero padded.
Appends '_T####' zero padded after <sequenceName> if <sequenceName> is specified.
Appends '_ID####' zero padded after <sequenceName> and <trackNumber> if <personID> is specified.

Overall possible output:    <sequenceName>_T####_ID####     with corresponding <trackNumber> and <personID>
"""
def makeTrackName(trackNumber, sequenceName='', personID=''):
    trackName = str(trackNumber).zfill(4)
    if sequenceName != '':
        trackName = sequenceName + "_T" + trackName
    if personID != '':
        trackName += "_" + personID
    return trackName

"""
Returns the frame number as '########' zero padded.
"""
def makeFrameName(frameNumber):
    return str(frameNumber).zfill(8)

"""
Returns <basePath>/<sequenceDir>/<frameNumber>.jpg
"""
def makeFramePath(frameNumber, sequenceDir, basePath):
    frameName = makeFrameName(frameNumber) + ".jpg"
    return posixpath.join(basePath, sequenceDir, frameName)

if __name__ == "__main__":
    generateSequencesChokePoint('')