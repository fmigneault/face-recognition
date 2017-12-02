import evalSequenceFile as e
import os, sys, csv
import argparse


def results_list_from_file(results_file):
    assert(os.path.isfile(results_file))
    with open(results_file, 'r') as f:
        csvr = csv.reader(f)
        results_list = [line for line in csvr if line != '' and len(line) > 0]
    return results_list


def append_missing_thresholds(results_list, increment=0.1):
    TL = e.generateThresholds(0, (len(results_list) - 1) * increment, increment)
    for i, line in enumerate(results_list):
        results_list[i] = [TL[i]] + line
    return results_list


def process_summary_from_results_list(results_list):
    cmList = results_list_to_confusion_matrix_dict(results_list)
    results = [
        e.calcAUC(cmList, pFPR=0.05),
        e.calcAUC(cmList, pFPR=0.10),
        e.calcAUC(cmList, pFPR=0.20),
        e.calcAUC(cmList),
        e.calcAUPR(cmList),
    ]
    return results


def results_list_to_confusion_matrix_dict(results_list):
    cmList = dict()
    for r in results_list:
        T = float(r[0])
        TP, FP, TN, FN = [float(r[i]) for i in range(1, 5)]
        cmList[T] = e.ConfusionMatrix([TP, FP, TN, FN])
    return cmList


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run evaluations on results files using corresponding sequences files')
    parser.add_argument('-L', '--list', type=str, nargs=1, dest='resultsList', metavar='resultsList', default=None,
                        help="give a python list directly as input, formatted by entries of [T,TP,FP,TN,FN]")
    parser.add_argument('-f', '--file', type=str, nargs=1, dest='resultsFile', metavar='resultsFile', default=None,
                        help="give a file path containing formatted entries of [T,TP,FP,TN,FN]")
    args = parser.parse_args()
    if args.resultsFile is None and args.resultsList is None or \
       args.resultsFile is not None and args.resultsList is not None:
        raise Exception("One of '-L' or '-f' argument must be specified")
    if args.resultsFile is not None:
        results_list = results_list_from_file(args.resultsFile)
        process_summary_from_results_list(results_list)
    if args.resultsList is not None:
        process_summary_from_results_list(eval(args.resultsList))
