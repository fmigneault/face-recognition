# -*- coding: utf-8 -*-
"""
Created on Thu Jun 15 10:20:22 2017

@author: Francis Charette Migneault
"""

import evalSequenceFile, extractSequencesChokePoint
from os import path as p, remove as rm
import argparse


def evalSequencesChokePoint(sequencesFilesDir, resultsFilesDir, filterSequencesFilePath="",
                            evalTransactionLevel=True, evalTrajectoryLevel=True,
                            overwriteResultsFiles=False, overwriteEvaluationFiles=False,
                            evalBackwardCompatibility=False):
    """
    Evaluates expected ChokePoint 'RESULTS' file, and all other variations generated from it with corresponding
    'SEQUENCES' files. If complete 'results' file is found and others are not, they get generated before evaluation.

    Transaction/Trajectory-level evaluation are both available.
    Backward compatibility supported for trajectory-level only if using old 'RESULTS' formats (no transaction scores).
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
    parser = argparse.ArgumentParser(description='Run evaluations on results files using corresponding sequences files')
    parser.add_argument('-s', '--sequences', type=str, default='./bin/sequences/', nargs='?',
                        dest='sequencesFilesDir', metavar='sequencesFilesDir',
                        help="path to 'sequences-info*.csv' files as references for evaluation (default: %(default)s)")
    parser.add_argument('-r', '--results', type=str, default='./bin/results/', nargs='?',
                        dest='resultsFilesDir', metavar='resultsFilesDir',
                        help="path to 'results.txt' file to evaluate (default: %(default)s)")
    parser.add_argument('--transaction', type=bool, default=True, nargs='?',
                        dest='evalTransactionLevel', metavar='evalTransactionLevel',
                        help="run evaluation for transaction-level scores (default: %(default)s)")
    parser.add_argument('--trajectory', type=bool, default=True, nargs='?',
                        dest='evalTrajectoryLevel', metavar='evalTrajectoryLevel',
                        help="run evaluation for trajectory-level scores (default: %(default)s)")
    parser.add_argument('-o', '--overwrite-results', type=bool, default=False, nargs='?',
                        dest='overwriteResultsFiles', metavar='overwriteResultsFiles',
                        help="overwrite any existing and conflicting results file (default: %(default)s)")
    parser.add_argument('-e', '--overwrite-evaluations', type=bool, default=False, nargs='?',
                        dest='overwriteEvaluationFiles', metavar='overwriteEvaluationFiles',
                        help="overwrite any existing and conflicting evaluation file (default: %(default)s)")
    parser.add_argument('-f', '--filter', type=str, default="", nargs='?',
                        dest='filterSequencesFilePath', metavar='filterSequencesFilePath',
                        help="path to file with sequences IDs to filter in evaluation (default: none used)")
    parser.add_argument('-b', '--backward-compatibility', type=bool, default=False, nargs='?',
                        dest='evalBackwardCompatibility', metavar='evalBackwardCompatibility',
                        help="enable backward compatibility mode - old results format (default: %(default)s)")
    args = parser.parse_args()
    evalSequencesChokePoint(args.sequencesFilesDir, args.resultsFilesDir, args.filterSequencesFilePath,
                            args.evalTransactionLevel, args.evalTrajectoryLevel,
                            args.overwriteResultsFiles, args.overwriteEvaluationFiles,
                            args.evalBackwardCompatibility)
