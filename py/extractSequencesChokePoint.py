# -*- coding: utf-8 -*-
"""
Operations applied specifically to sequences specified as 'frontal' views in ChokePoint dataset.
"""

import os
import csv


FRONTAL_SEQUENCES = [
    "P1E_S1_C1",
    "P1E_S2_C2",
    "P1E_S3_C3",
    "P1E_S4_C1",
    "P1L_S1_C1",
    "P1L_S2_C2",
    "P1L_S3_C3",
    "P1L_S4_C1",
    "P2E_S1_C3",
    "P2E_S2_C2",
    "P2E_S3_C1",
    "P2E_S4_C2",
    "P2L_S1_C1",
    "P2L_S2_C2",
    "P2L_S3_C3",
    "P2L_S4_C2"
]

"""
Extracts the results corresponding to sequences that matches any of 'FRONTAL_SEQUENCES'.
The function expects the test sequence 'RESULTS' output format specified in 'evalSequenceFile'.
"""
def extractFrontalChokePoint(resultsFile, noHeaderResults=False):
    assert(os.path.isfile(resultsFile))
    outFile, extFile = os.path.splitext(resultsFile)
    frontalFile = outFile + "-frontal" + extFile

    with open(resultsFile, 'rb') as csvFile:
        csvReader = csv.reader(csvFile)
        resultsLines = [line for line in csvReader]
        assert(len(resultsLines) > 1)

    with open(frontalFile, 'wb') as csvFile:
        csvWriter = csv.writer(csvFile)
        startIndex = 0
        if not noHeaderResults:
            startIndex = 1
            csvWriter.writerow(resultsLines[0])

        for r in resultsLines[startIndex:]:
            if r[0] in FRONTAL_SEQUENCES or r[0].split("_T")[0] in FRONTAL_SEQUENCES:
                csvWriter.writerow(r)

"""
Extracts the results corresponding to each specific session number (S1,S2,S3,S4) to create independant results.
The function expects the test sequence 'RESULTS' output format specified in 'evalSequenceFile'.
"""
def extractSessionsChokePoint(resultsFile, noHeaderResults=False):
    assert(os.path.isfile(resultsFile))
    outFile, extFile = os.path.splitext(resultsFile)
    sessionFiles = [outFile + "-S" + str(s+1) + extFile for s in range(4)]

    with open(resultsFile, 'rb') as csvFile:
        csvReader = csv.reader(csvFile)
        resultsLines = [line for line in csvReader]
        assert(len(resultsLines) > 1)

    files = [open(f, 'wb') for f in sessionFiles]
    csvWriters = [csv.writer(f) for f in files]
    startIndex = 0
    if not noHeaderResults:
        startIndex = 1
        [w.writerow(resultsLines[0]) for w in csvWriters]

    for r in resultsLines[startIndex:]:
        session = int(r[0].split("_S")[1].split("_C")[0]) - 1
        csvWriters[session].writerow(r)

    [f.close() for f in files]


if __name__ == "__main__":
    extractFrontalChokePoint('')
