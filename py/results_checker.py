import os, sys, csv
import argparse


def assert_print(test, msg):
    try:
        assert(test)
    except Exception as e:
        raise AssertionError(msg)


def check(sequencesFile, resultsFile):
    assert_print(os.path.isfile(resultsFile),   "resultsFile: '{0}'".format(resultsFile))
    assert_print(os.path.isfile(sequencesFile), "sequencesFile: '{0}'".format(sequencesFile))

    with open(sequencesFile, 'rb') as csvFileIn:
        csvReader = csv.reader(csvFileIn)
        allLinesSeq = [line for line in csvReader]
        assert_print(len(allLinesSeq) > 1, "len(allLinesSeq): '{0}'".format(len(allLinesSeq)))
    with open(resultsFile, 'rb') as csvFileIn:
        csvReader = csv.reader(csvFileIn)
        allLinesRes = [line for line in csvReader]
        assert_print(len(allLinesRes) > 1, "len(allLinesRes): '{0}'".format(len(allLinesRes)))

    assert_print(allLinesRes[0][0] == 'SEQUENCE_TRACK_ID', "allLinesRes[0]: '{0}'".format(allLinesRes[0]))
    assert_print(allLinesSeq[0][0] == 'SEQUENCE_NAME',     "allLinesSeq[0]: '{0}'".format(allLinesSeq[0]))

    # skip checked headers
    allLinesRes = allLinesRes[1:]
    allLinesSeq = allLinesSeq[1:]

    for i in range(min(len(allLinesRes),len(allLinesSeq))):
        try:
            s = allLinesSeq[i]
            r = allLinesRes[i]
            si = s[0]+'_T'+s[3]+'_'+s[4]+'_'+s[1]
            ri = r[0]+'_'+r[2]
            assert_print(si==ri, "Not equivalent line IDs at corresponding indexes, i={0}, si={1}, ri={2}".format(i, si, ri))
        except Exception as e:
            print("Something weird happened here: " + str(i) + " [" + repr(e) + "]")
            raise
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Check a result file against a sequence file')
    parser.add_argument('-s', dest='sequencesFile', metavar='sequencesFile',
                        type=str, default='./sequences/sequences-info.csv', nargs='?',
                        help="path to 'sequences-info' file to check (default: %(default)s))")
    parser.add_argument('-r', dest='resultsFile', metavar='resultsFile',
                        type=str, default='./results/results.txt', nargs='?',
                        help="path to 'results' file to check (default: %(default)s)")
    args = parser.parse_args()
    assert_print(check(args.sequencesFile, args.resultsFile) == 0, "Check failed somewhere")
    print("Everything seems ok!")
