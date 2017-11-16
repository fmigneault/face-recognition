import os, sys, csv
import argparse


def getLineID(line):
    return line[0]+'_'+line[2]


def merge(resultsDir):
    # remove old merge file
    merge_file = os.path.join(resultsDir, 'results_merged.txt')
    if os.path.isfile(merge_file):
        os.remove(merge_file)

    # merge all found files
    allResults = []
    result_files = [f for f in sorted(os.listdir(resultsDir)) if f.startswith('result') and f.endswith('.txt')]
    for filename in result_files:
        print("Merging: " + filename)
        with open(os.path.join(resultsDir, filename)) as f:
            csvf = csv.reader(f)
            lines = [l for l in csvf]
            allResults.extend(lines)

    # make unique line results in case of error merging
    allResByID = {}
    for line in allResults:
        if len(line) < 1:    # skip empty lines
            continue
        lineID = getLineID(line)
        if lineID not in allResByID:
            allResByID[lineID] = line

    # sort lines and move header to top
    allLinesSort = [allResByID[lineID] for lineID in sorted(allResByID)]
    headerID = "SEQUENCE_TRACK_ID_FRAME_NUMBER"
    if headerID in allResByID:
        lastLine = allLinesSort[-1]
        lastLineID = getLineID(lastLine)
        if headerID == lastLineID:
            allLinesSort = [allLinesSort[-1]] + allLinesSort[:-1]

    # save results
    with open(merge_file, 'w') as f:
        csvf = csv.writer(f)
        csvf.writerows(allLinesSort)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Merge result files')
    parser.add_argument(dest='resultsDir', metavar='resultsDir', type=str, default='./results', nargs='?',
                        help="location of 'results*.txt' files to merge (sorted by name) (default: %(default)s)")
    args = parser.parse_args()
    merge(args.resultsDir)
