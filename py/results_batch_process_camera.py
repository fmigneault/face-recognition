# -*- coding: utf-8 -*-
import os, sys, csv
import argparse
import copy
import evalSequenceFile as ev
import extract_probes as ex


SEQUENCES = [
        "P1E_C1",
        "P1E_C2",
        "P1E_C3",
        "P1L_C1",
        "P1L_C2",
        "P1L_C3",
        "P2E_C1",
        "P2E_C2",
        "P2E_C3",
        "P2L_C1",
        "P2L_C2",
        "P2L_C3"
    ]


def get_file_paths(sequencesDir, resultsDir, cameraSequence):
    cam = cameraSequence
    res = "results-"
    seq = "sequences-"
    info = "-info"
    transac = "-perf-transaction"
    traject = "-perf-trajectory"
    probes = "_probes"
    norm = "-norm"
    ext = ".txt"
    return {
        "ALL": {
            "SEQUENCES_FILE" :      os.path.join(sequencesDir, seq + cam + info + ext),
            "RESULTS_FILE":         os.path.join(resultsDir, res + cam + ext),
            "RESULTS_RAW": {
                "TRANSACTION_FILE": os.path.join(resultsDir, res + cam + transac + ext),
                "TRAJECTORY_FILE":  os.path.join(resultsDir, res + cam + traject + ext),
            },
            "RESULTS_NORM": {
                "TRANSACTION_FILE": os.path.join(resultsDir, res + cam + transac + norm + ext),
                "TRAJECTORY_FILE":  os.path.join(resultsDir, res + cam + traject + norm + ext),
            }
        },
        "PROBES": {                 # probes sequences are generated inside results dir
            "SEQUENCES_FILE":       os.path.join(resultsDir, seq + cam + info + probes + ext),
            "RESULTS_FILE":         os.path.join(resultsDir, res + cam + probes + ext),
            "PROBES_FILE":          os.path.join(resultsDir, "probes_list" + ext),
            "RESULTS_RAW": {
                "TRANSACTION_FILE": os.path.join(resultsDir, res + cam + probes + transac + ext),
                "TRAJECTORY_FILE":  os.path.join(resultsDir, res + cam + probes + traject + ext),
            },
            "RESULTS_NORM": {
                "TRANSACTION_FILE": os.path.join(resultsDir, res + cam + probes + transac + norm + ext),
                "TRAJECTORY_FILE":  os.path.join(resultsDir, res + cam + probes + traject + norm + ext),
            }
        }
    }


def batch_process_camera(sequencesDir, resultsDir, probesFilePath, overwrite):
    global SEQUENCES
    
    for s in SEQUENCES:
        pathInfo = get_file_paths(sequencesDir, resultsDir, s)
        probesFilePath = pathInfo["PROBES"]["PROBES_FILE"] if probesFilePath is None else probesFilePath
        sequencesFilePath = pathInfo["ALL"]["SEQUENCES_FILE"]
        resultsFilePath = pathInfo["ALL"]["RESULTS_FILE"]
        assert(os.path.isfile(probesFilePath))
        assert(os.path.isfile(sequencesFilePath))
        assert(os.path.isfile(resultsFilePath))
        ex.get_probe_results(resultsFilePath, resultsFilePath, probesFilePath)

        for info in pathInfo:
            print("SEQ: " + pathInfo[info]["SEQUENCES_FILE"])
            print("RES: " + pathInfo[info]["RESULTS_FILE"])
            if not overwrite \
            and os.path.isfile(pathInfo[info]["RESULTS_RAW"]["TRANSACTION_FILE"]) \
            and os.path.isfile(pathInfo[info]["RESULTS_RAW"]["TRAJECTORY_FILE"]) \
            and os.path.isfile(pathInfo[info]["RESULTS_NORM"]["TRANSACTION_FILE"]) \
            and os.path.isfile(pathInfo[info]["RESULTS_NORM"]["TRAJECTORY_FILE"]) :
                print("Skipping existing results generation (exist and 'overwrite=True'")
                continue
            ev.evalSequenceFilePerf(pathInfo[info]["SEQUENCES_FILE"], pathInfo[info]["RESULTS_FILE"],
                                    filterSequencesFilePath="", noHeaderSequences=False, noHeaderResults=False,
                                    evalNormalizedScores=True, evalTransactionLevel=True, evalTrajectoryLevel=True,
                                    outputMergedEvalFile=True, evalBackwardCompatibility=False, verbosity=2)


def batch_analysis_camera(sequencesDir, resultsDir):
    global SEQUENCES
    COLUMN_WIDTH = 10

    def raise_not_exists(filePath):
        if not os.path.isfile(filePath):
            raise Exception("Missing required file: " + str(filePath))

    RESULTS_HEADER = ['pAUC(5%)','pAUC(10%)','pAUC(20%)','AUC','AUPR']
    RESULTS = dict([(h, 0) for h in RESULTS_HEADER])

    def print_summary_result(sequence, results):
        res = ["{0:.5f}".format(results[r]).rjust(COLUMN_WIDTH) for r in RESULTS_HEADER]
        print(",".join([sequence] + res))

    def print_section_header(probes_mode, results_norm, track_mode):
        print("VARIATION: " + " - ".join([probes_mode, results_norm, track_mode]))
        header = ["{0}".format(h).rjust(COLUMN_WIDTH) for h in RESULTS_HEADER]
        print(",".join(["SEQUENCE"] + header))

    for p in ["ALL", "PROBES"]:
        for n in ["RESULTS_RAW", "RESULTS_NORM"]:
            for t in ["TRANSACTION_FILE", "TRAJECTORY_FILE"]:
                seqRes = {"AVG": copy.deepcopy(RESULTS)}
                print_section_header(p, n, t)
                for s in SEQUENCES:
                    pathInfo = get_file_paths(sequencesDir, resultsDir, s)
                    raise_not_exists(pathInfo[p][n][t])
                    seqRes[s] = copy.deepcopy(RESULTS)
                    with open(pathInfo[p][n][t], 'r') as f:
                        csvr = csv.reader(f)
                        lines = [_ for _ in csvr]
                    for i, r in enumerate(RESULTS_HEADER):
                        seqRes[s][r] = float(lines[1][i])
                        seqRes["AVG"][r] += seqRes[s][r]
                    print_summary_result(s, seqRes[s])
                for r in RESULTS_HEADER:
                    seqRes["AVG"][r] /= float(len(SEQUENCES))
                print("="*COLUMN_WIDTH*(1+len(RESULTS_HEADER)))
                print_summary_result("AVG", seqRes["AVG"])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='batch process camera-specific results')
    parser.add_argument(dest='sequencesDir', metavar='sequencesDir', type=str, help="path to 'sequences-info' files")
    parser.add_argument(dest='resultsDir', metavar='resultsDir', type=str, help="path to 'results' files")
    parser.add_argument('-s', '--summary', dest='summaryOnly', action='store_true',
                        help="execute only summary analysis of pre-generated evaluation files"
                             "(skip perf/eval generation, corresponding files must exist)")
    parser.add_argument('-p', '--probes', dest='probesFile', metavar='probesFile', type=str, default=None,
                        help='path to probes file list (default: `probes_list.txt` in current directory)')
    parser.add_argument('-o', '--overwrite', dest='overwrite', action='store_true', default=False,
                        help='overwrite results files (re-evaluate) if they already exist')
    args = parser.parse_args()
    if not args.summaryOnly:
        batch_process_camera(args.sequencesDir, args.resultsDir, args.probesFile, args.overwrite)
    else:
        print("Skipping directly to summary analysis without evaluation file generation")
    batch_analysis_camera(args.sequencesDir, args.resultsDir)

