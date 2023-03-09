# -*- coding: utf-8 -*-
"""
Created on Thu Jun 15 10:20:22 2017

@author: Francis Charette Migneault
"""

import evalSequenceFile, extractSequencesChokePoint
from os import path as p, remove as rm


def evalSequencesChokePoint(sequencesFilesDir, resultsFilesDir, filterSequencesFilePath="",
                            evalTransactionLevel=True, evalTrajectoryLevel=True,
                            overwriteResultsFiles=False, overwriteEvaluationFiles=False,
                            evalBackwardCompatibility=False):
    """
    Evaluates expected ChokePoint 'RESULTS' file, and all other variations generated from it with corresponding
    'SEQUENCES' files. If complete 'results' file is found and others are not, they get generated before evaluation.

    Transaction/Trajectory-level evaluation are both available.
    Backward compatibility supported for tracjectory-level only if using old 'RESULTS' formats (no transaction scores).
    See 'evalSequenceFile.evalSequenceFilePerf' for more details.
    """

    assert(p.isdir(sequencesFilesDir))
    assert(p.isdir(resultsFilesDir))

    sequencesFileName = "sequences-info"
    sequencesFileExt = ".csv"
    resultsFileName = "results"
    resultsFileExt = ".txt"
    resultsFrontal = ["-frontal"]
    resultsSession = ["-S1", "-S2", "-S3", "-S4"]
    resultsEval = "-eval"
    resultsPerf = "-perf"
    resultsPerfTransac = resultsPerf + "-transaction"
    resultsPerfTraject = resultsPerf + "-trajectory"
    resultsVariations = [""] + resultsFrontal + resultsSession

    sequencesFileBase = p.join(sequencesFilesDir, sequencesFileName)
    resultsFileBase = p.join(resultsFilesDir, resultsFileName)
    assert(p.isfile(sequencesFileBase + sequencesFileExt))
    assert(p.isfile(resultsFileBase + resultsFileExt))

    for var in resultsVariations:
        # clean previous results files if overwrite requested and matching backward compatibility
        resultsVarEvalFilePath = resultsFileBase + var + resultsEval + resultsFileExt
        resultsVarPerfFilePath = resultsFileBase + var + resultsPerf + resultsFileExt
        resultsVarPerfTransacFilePath = resultsFileBase + var + resultsPerfTransac + resultsFileExt
        resultsVarPerfTrajectFilePath = resultsFileBase + var + resultsPerfTraject + resultsFileExt
        if overwriteEvaluationFiles and p.isfile(resultsVarEvalFilePath):
            rm(resultsVarEvalFilePath)
        if overwriteEvaluationFiles and p.isfile(resultsVarPerfFilePath) and evalBackwardCompatibility:
            rm(resultsVarPerfFilePath)
        if overwriteEvaluationFiles and p.isfile(resultsVarPerfTransacFilePath) and not evalBackwardCompatibility:
            rm(resultsVarPerfTransacFilePath)
        if overwriteEvaluationFiles and p.isfile(resultsVarPerfTrajectFilePath) and not evalBackwardCompatibility:
            rm(resultsVarPerfTrajectFilePath)

        # generate results file variations if missing or overwrite requested
        resultsVarFilePath = resultsFileBase + var + resultsFileExt
        if not p.isfile(resultsVarFilePath) or overwriteResultsFiles:
            if var in resultsFrontal:
                extractSequencesChokePoint.extractFrontalChokePoint(resultsFileBase + resultsFileExt)
            elif var in resultsSession:
                extractSequencesChokePoint.extractSessionsChokePoint(resultsFileBase + resultsFileExt)

    for var in resultsVariations:
        # skip generation of evaluation file variation if applicable
        tmpEvalTransac = evalTransactionLevel
        tmpEvalTraject = evalTrajectoryLevel
        resultsVarName = resultsFileName + var
        resultsVarPerfFilePath = resultsFileBase + var + resultsPerf + resultsFileExt
        resultsVarPerfTransacFilePath = resultsFileBase + var + resultsPerfTransac + resultsFileExt
        resultsVarPerfTrajectFilePath = resultsFileBase + var + resultsPerfTraject + resultsFileExt
        if not overwriteEvaluationFiles:
            if evalBackwardCompatibility and p.isfile(resultsVarPerfFilePath):
                print("Skipping generation of '" + resultsVarName + resultsPerf + "' (already exists)")
                continue
            if not evalBackwardCompatibility and p.isfile(resultsVarPerfTransacFilePath):
                print("Skipping generation of '" + resultsVarName + resultsPerfTransac + "' (already exists)")
                tmpEvalTransac = False
            if not evalBackwardCompatibility and p.isfile(resultsVarPerfTrajectFilePath):
                print("Skipping generation of '" + resultsVarName + resultsPerfTraject + "' (already exists)")
                tmpEvalTraject = False
            if not tmpEvalTransac and not tmpEvalTraject:
                continue

        # produce evaluation files
        evalSequenceFile.evalSequenceFilePerf(sequencesFileBase + var + sequencesFileExt,
                                              resultsFileBase + var + resultsFileExt,
                                              filterSequencesFilePath,
                                              evalTransactionLevel=tmpEvalTransac,
                                              evalTrajectoryLevel=tmpEvalTraject,
                                              evalBackwardCompatibility=evalBackwardCompatibility)


if __name__ == "__main__":
    evalSequencesChokePoint()
