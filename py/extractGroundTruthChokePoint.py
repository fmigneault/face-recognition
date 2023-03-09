# -*- coding: utf-8 -*-
"""
Utility functions that allow extraction of ground truth informations from all ChokePoint XML files.

    * Only for single person per track supported.
"""

import os, posixpath
import xml.etree.ElementTree as et

"""
Returns the frame number of the specified frame XML element as an int, or -1 if not found
"""
def xmlFrameNumber(xmlElementFrame):
    if type(xmlElementFrame) != et.Element:
        return -1
    frameNumStr = xmlElementFrame.attrib.get("number")
    if (frameNumStr == "" or frameNumStr == None):
        return -1
    return int(frameNumStr)

"""
Validation function that goes with 'xmlFrameNumber'
"""
def isValidFrameNumber(number):
    if type(number) != int:
        return False
    if number < 0:
        return False
    return True

"""
Returns the person ID of the specified frame XML element, or None if not found or not available
"""
def xmlFramePersonID(xmlElementFrame):
    if type(xmlElementFrame) != et.Element:
        return None
    childs = xmlElementFrame.getchildren()
    if len(childs) > 0:
        if (childs[0].tag == "person"):
            return childs[0].attrib.get("id")
    return None

"""
Validation function that goes with 'xmlFramePersonID'
"""
def isValidPersonID(personID):
    if type(personID) != str:
        return False
    if personID == "":
        return False
    return True

"""
Returns the persons information as a dictionary.
Returns an an empty dictionary if not available.
Contains eye positions as:

    {'leftEye': (x,y), 'rightEye': (x,y)}
"""
def xmlFramePersonInfo(xmlElementFrame):
    if type(xmlElementFrame) != et.Element:
        return dict()
    person = xmlElementFrame.getchildren()
    if len(person) < 1:
        return dict()
    info = person[0]
    if type(info) != et.Element:
        return dict()
    info = info.getchildren()
    dictInfo = dict()
    for i in range(len(info)):
        if info[i].tag in ["leftEye","rightEye"]:
            dictInfo[info[i].tag] = (info[i].attrib.get('x'), info[i].attrib.get('y'))
    return dictInfo

"""
Validation function that goes with 'xmlFramePersonInfo'
"""
def isValidPersonInfo(personInfo):
    if type(personInfo) != dict:
        return False
    leftEye = personInfo.get("leftEye")
    rightEye = personInfo.get("rightEye")
    if leftEye == None or rightEye == None:
        return False
    if type(leftEye) != tuple or type(rightEye) != tuple:
        return False
    if len(leftEye) != 2 or len(rightEye) != 2:
        return False
    return True

"""
Parses a ChokePoint Ground Truth XML file for frames with person ID specified to extract them per corresponding track.
Outputs a list in the format:

    [ {'sequence': P#T_S#_C#, 'track': #, 'id': ####, 'frames': [<frame1>, <frame2>, ...], ... ]

Where each <frame> contains:

    {"number": #, "info": <info> }      with <info> generated with 'xmlFramePersonInfo' format

"""
def xmlParseChokePointGT(filepath):

    # load xml and validate basic root with filename
    tree = et.parse(filepath)
    root = tree.getroot()
    if root.tag != "dataset":
        return list()
    seqName = root.attrib.get("name")
    seqFileName = os.path.split(filepath)[1]        # filename + ext
    seqFileName = os.path.splitext(seqFileName)[0]  # remove ext
    seqNameShort = os.path.splitext(seqFileName)[0] # remove '.#' sometime added at end if sequence name
    if seqNameShort != seqName:
        return list()

    # loop through frames and regroup them per track
    trackFrames = list()
    prevFrame = None
    for frame in root:
        frameNum = xmlFrameNumber(frame)
        if not isValidFrameNumber(frameNum):
            continue
        if prevFrame == None:   # first frame
            prevFrame = frame
            continue

        # check for frame containing a person
        personID = xmlFramePersonID(frame)
        if isValidPersonID(personID):
            info = xmlFramePersonInfo(frame)
            if isValidPersonInfo(info):
                prevPersonID = xmlFramePersonID(prevFrame)
                prevFrameNum = xmlFrameNumber(prevFrame)

                # create new track on new person but not in previous frame or on non-subsequent frames
                if not isValidPersonID(prevPersonID) or (frameNum - prevFrameNum > 1):
                    trackFrames.append({"sequence": seqFileName,
                                        "track":    len(trackFrames),
                                        "id":       personID,
                                        "frames":   [{"number": frameNum, "info": info}]
                                       })
                # append last created track on same person and in previous frame
                elif prevPersonID == personID:
                    idx = len(trackFrames) - 1
                    trackFrames[idx]["frames"].append({"number": frameNum, "info": info})

        prevFrame = frame
    return trackFrames

"""
Extract track-based ground truths from all ChokePoint sequences.
Returns a dictionary of tracks grouped by sequence file names.
"""
def extractAllChokePointGT(basePathChokePointGT):
    allSequencesGT = dict()
    if os.path.isdir(basePathChokePointGT):
        files = sorted([file for file in os.listdir(basePathChokePointGT) if file.endswith(".xml")])
        for xmlFileGT in files:
            seqGT = xmlParseChokePointGT(posixpath.join(basePathChokePointGT, xmlFileGT))
            if (len(seqGT) < 1):
                continue
            seqName = os.path.splitext(xmlFileGT)[0]
            allSequencesGT[seqName] = seqGT
    return allSequencesGT

"""
Combines track ground truths information for basic sequences (P#T_S#_C#) into larger sequences (S#).
Reassigns the track ID numbers for appended sequences.
Original 'sequence' field is left intact to allow finding the frame numbers in their corresponding directories.
"""
def combineSequencesChokePointGT(basePathChokePointGT):
    seq = extractAllChokePointGT(basePathChokePointGT)
    groupedSeq = {"S1": [], "S2": [], "S3": [], "S4": []}
    for s in sorted(seq):
        seqName = s[4:6]
        groupedSeq[seqName].extend(seq[s])
    for s in groupedSeq:
        for i in range(len(groupedSeq[s])):
            groupedSeq[s][i]["track"] = i
    return groupedSeq

if __name__ == "__main__":
    extractAllChokePointGT('')