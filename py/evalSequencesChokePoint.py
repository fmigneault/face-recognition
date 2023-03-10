# -*- coding: utf-8 -*-
"""
Created on Thu Jun 15 10:20:22 2017

@author: Francis Charette Migneault
"""

import evalSequenceFile, extractSequencesChokePoint
from printUtils import verbose
from os import path as p, remove as rm
import argparse


def evalSequencesChokePoint(sequencesFilesDir, resultsFilesDir, filterSequencesFilePath=None,
                            evalTransactionLevel=True, evalTrajectoryLevel=True, evalNormalizedScores=False,
                            overwriteResultsFiles=False, overwriteEvaluationFiles=False,
                            overwritePerformanceFiles=False, appendFileModifier=None,
                            evalBackwardCompatibility=False, verbosity=2):
    """
    Evaluates expected ChokePoint 'RESULTS' file, and all other variations generated from it with corresponding
    'SEQUENCES' files. If complete 'results' file is found and others are not, they get generated before evaluation.

    Transaction/Trajectory-level evaluation are both available.
    Backward compatibility supported for trajectory-level only if using old 'RESULTS' formats (no transaction scores).
    See 'evalSequenceFile.evalSequenceFilePerf' for more details.
    """

    assert(p.isdir(sequencesFilesDir))
    assert(p.isdir(resultsFilesDir))

    sequencesFileName = "sequences"
    sequencesDetails = "-info"
    sequencesFileExt = ".txt"
    sequencesFileBase = p.join(sequencesFilesDir, sequencesFileName)

    resultsFileName = "results"
    resultsFileExt = ".txt"
    resultsFrontal = ["-frontal"]
    resultsSession = ["-S1", "-S2", "-S3", "-S4"]
    resultsEval = "-eval"
    resultsPerf = "-perf"
    resultsPerfTransac = resultsPerf + "-transaction"
    resultsPerfTraject = resultsPerf + "-trajectory"
    resultsVariations = [""] + resultsFrontal + resultsSession
    resultsEvalVariations = [""] + (["-norm"] if evalNormalizedScores else [])
    resultsFileBase = p.join(resultsFilesDir, resultsFileName)

    mod = "" if appendFileModifier is None else appendFileModifier
    refSequencesFile = sequencesFileBase + sequencesDetails + mod + sequencesFileExt
    refResultsFile = resultsFileBase + mod + resultsFileExt

    verbose(verbosity, 1, "Sequences format is:      [{base}<var>{detail}{mod}{ext}]"
            .format(base=sequencesFileBase, detail=sequencesDetails, mod=mod, ext=sequencesFileExt))
    verbose(verbosity, 1, "Results format is:        [{base}<var>{mod}{ext}]"
            .format(base=resultsFileBase, mod=mod, ext=resultsFileExt))
    verbose(verbosity, 1, "Results <var> are:        {variations}".format(variations=resultsVariations))
    verbose(verbosity, 1, "Evaluation <var> are:     {variations}".format(variations=resultsEvalVariations))
    verbose(verbosity, 1, "Reference sequences file: [{ref}]".format(ref=refSequencesFile))
    verbose(verbosity, 1, "Reference results file:   [{ref}]".format(ref=refResultsFile))

    assert(p.isfile(refSequencesFile))
    assert(p.isfile(refResultsFile))

    if overwriteEvaluationFiles:
        verbose(verbosity, 1, "Cleaning up old evaluation files (-eval)...")
        for var in resultsVariations:
            for evar in resultsEvalVariations:
                # clean previous results files if overwrite requested
                resultsVarEvalFilePath = resultsFileBase + var + mod + resultsEval + evar + resultsFileExt
                if p.isfile(resultsVarEvalFilePath):
                    rm(resultsVarEvalFilePath)

    if overwritePerformanceFiles:
        verbose(verbosity, 1, "Cleaning up old performance files (-perf)...")
        for var in resultsVariations:
            for evar in resultsEvalVariations:
                # clean previous results files if overwrite requested and matching backward compatibility
                resultsVarPerfFilePath = resultsFileBase + var + mod + resultsPerf + evar + resultsFileExt
                resultsVarPerfTransacFilePath = resultsFileBase + var + mod + resultsPerfTransac + evar + resultsFileExt
                resultsVarPerfTrajectFilePath = resultsFileBase + var + mod + resultsPerfTraject + evar + resultsFileExt
                if p.isfile(resultsVarPerfFilePath) and evalBackwardCompatibility:
                    rm(resultsVarPerfFilePath)
                if p.isfile(resultsVarPerfTransacFilePath) and not evalBackwardCompatibility:
                    rm(resultsVarPerfTransacFilePath)
                if p.isfile(resultsVarPerfTrajectFilePath) and not evalBackwardCompatibility:
                    rm(resultsVarPerfTrajectFilePath)

    verbose(verbosity, 1, "Extracting sessions and frontal sequences from complete results file...")
    for var in resultsVariations:
        # generate results file variations if missing or overwrite requested
        resultsVarFilePath = resultsFileBase + var + mod + resultsFileExt
        if not p.isfile(resultsVarFilePath) or overwriteResultsFiles:
            if var in resultsFrontal:
                extractSequencesChokePoint.extractFrontalChokePoint(resultsFileBase + mod + resultsFileExt)
            elif var in resultsSession:
                extractSequencesChokePoint.extractSessionsChokePoint(resultsFileBase + mod + resultsFileExt)

    verbose(verbosity, 1, "Starting evaluation...")
    for var in resultsVariations:
        # skip generation of evaluation file variation if applicable
        evalTransac = evalTransactionLevel
        evalTraject = evalTrajectoryLevel
        resultsVarName = resultsFileName + var + mod
        resultsVarPerfName = resultsVarName + resultsPerf
        resultsVarPerfTransacName = resultsVarName + resultsPerfTransac
        resultsVarPerfTrajectName = resultsVarName + resultsPerfTraject
        resultsVarEvalFilePath = resultsFileBase + var + mod + resultsEval + resultsFileExt
        resultsVarPerfFilePath = resultsFileBase + var + mod + resultsPerf + resultsFileExt
        resultsVarPerfTransacFilePath = resultsFileBase + var + mod + resultsPerfTransac + resultsFileExt
        resultsVarPerfTrajectFilePath = resultsFileBase + var + mod + resultsPerfTraject + resultsFileExt
        if not overwritePerformanceFiles:
            if evalBackwardCompatibility and p.isfile(resultsVarPerfFilePath):
                verbose(verbosity, 1, "Skipping generation of [" + resultsVarPerfName + "] (already exists)")
                continue
            if not evalBackwardCompatibility and p.isfile(resultsVarPerfTransacFilePath):
                verbose(verbosity, 1, "Skipping generation of [" + resultsVarPerfTransacName + "] (already exists)")
                evalTransac = False
            if not evalBackwardCompatibility and p.isfile(resultsVarPerfTrajectFilePath):
                verbose(verbosity, 1, "Skipping generation of [" + resultsVarPerfTrajectName + "] (already exists)")
                evalTraject = False
            if not evalTransac and not evalTraject:
                continue
        mergeEval = True
        if not overwriteEvaluationFiles and p.isfile(resultsVarEvalFilePath):
            verbose(verbosity, 1, "Skipping generation of [" + resultsVarName + resultsEval + "] (already exists)")
            mergeEval = False

        # produce evaluation files
        sequencesFileVar = sequencesFileBase + var + sequencesDetails + mod + sequencesFileExt
        resultsFileVar = resultsFileBase + var + mod + resultsFileExt
        verbose(verbosity, 1, "Runing generation with sequences: [{seq}]".format(seq=sequencesFileVar))
        verbose(verbosity, 1, "Runing generation for results:    [{res}]".format(res=resultsFileVar))
        evalSequenceFile.evalSequenceFilePerf(sequencesFileVar, resultsFileVar,
                                              filterSequencesFilePath=filterSequencesFilePath,
                                              evalNormalizedScores=evalNormalizedScores,
                                              evalTransactionLevel=evalTransac,
                                              evalTrajectoryLevel=evalTraject,
                                              outputMergedEvalFile=mergeEval,
                                              evalBackwardCompatibility=evalBackwardCompatibility,
                                              verbosity=verbosity)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run evaluations on results files using corresponding sequences files')
    parser.add_argument('-s', '--sequences', type=str, default='./bin/sequences/', nargs='?',
                        dest='sequencesFilesDir', metavar='sequencesFilesDir',
                        help="path to directory containing 'sequences-info*.csv' files as evaluation references "
                             "(default: %(default)s)")
    parser.add_argument('-r', '--results', type=str, default='./bin/results/', nargs='?',
                        dest='resultsFilesDir', metavar='resultsFilesDir',
                        help="path to directory containing 'results*.txt' files to evaluate (default: %(default)s)")
    parser.add_argument('--transaction', default=True, action='store_false',
                        dest='evalTransactionLevel',
                        help="disable evaluation for transaction-level scores (default: %(default)s)")
    parser.add_argument('--trajectory', default=True, action='store_false',
                        dest='evalTrajectoryLevel',
                        help="disable evaluation for trajectory-level scores (default: %(default)s)")
    parser.add_argument('-m', '--modifier', type=str, default=None, nargs=1,
                        dest='appendFileModifier', metavar='appendFileModifier',
                        help="pre-append a string modifier to files (default: %(default)s) "
                             "[ex: 'results<var>-eval.txt' => 'results<var>_probes-eval.txt' by specifying '_probes']")
    parser.add_argument('-n', '--normalized', default=False, action='store_true',
                        dest='evalNormalizedScores',
                        help="run extra evaluation with normalized scores across whole results (default: %(default)s)")
    parser.add_argument('-o', '--overwrite-results', default=False, action='store_true',
                        dest='overwriteResultsFiles',
                        help="overwrite any existing and conflicting results file (default: %(default)s)")
    parser.add_argument('-e', '--overwrite-evaluations', default=False, action='store_true',
                        dest='overwriteEvaluationFiles',
                        help="overwrite any existing and conflicting evaluation file (default: %(default)s)")
    parser.add_argument('-p', '--overwrite-performance', default=False, action='store_true',
                        dest='overwritePerformanceFiles',
                        help="overwrite any existing and conflicting performance file (default: %(default)s)")
    parser.add_argument('-f', '--filter', type=str, default=None, nargs=1,
                        dest='filterSequencesFilePath', metavar='filterSequencesFilePath',
                        help="path to file with sequences IDs to filter in evaluation (default: none used)")
    parser.add_argument('-b', '--backward-compatibility', default=False, action='store_true',
                        dest='evalBackwardCompatibility',
                        help="enable backward compatibility mode - old results format (default: %(default)s)")
    parser.add_argument('-v', '--verbose', type=int, default=2, choices=[0, 1, 2],
                        dest='verbosity', metavar='verbosity',
                        help="verbosity level (default: %(default)s) (0: none, 1: minimal, 2: progress)")
    args = parser.parse_args()
    evalSequencesChokePoint(args.sequencesFilesDir, args.resultsFilesDir,
                            None if args.filterSequencesFilePath is None else args.filterSequencesFilePath[0],
                            args.evalTransactionLevel, args.evalTrajectoryLevel, args.evalNormalizedScores,
                            args.overwriteResultsFiles, args.overwriteEvaluationFiles, args.overwritePerformanceFiles,
                            None if args.appendFileModifier is None else args.appendFileModifier[0],
                            args.evalBackwardCompatibility, args.verbosity)
