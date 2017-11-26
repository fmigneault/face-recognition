import os, sys, csv
import argparse
import evalSequenceFile as e
import results_extract_probes as p


def batch_process_camera(sequencesDir, resultsDir):

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

    for s in SEQUENCES:
        res = "./results_" + s + ".txt"
        
        if os.path.isdir(s):
            for f in os.listdir(s):
                os.remove(os.path.join(s,f))
            os.rmdir(s)
        os.mkdir(s)
        
        seqFile = os.path.join(sequencesDir, "sequences-" + s + "-info.txt")
        resFile = os.path.join(resultsDir, "results_" + s + ".txt")
        
        # probes sequnecs generated inside results dir
        seqFileProbes = os.path.join(resultsDir, "sequences-" + s + "-info_probes.txt")  
        resFileProbes = os.path.join(resultsDir, "results_" + s + "_probes.txt")
        probesFile = os.path.join(resultsDir, "probes_list.txt")
        p.get_probe_results(seqFile, resFile, probesFile)
        
        for si, ri in [(seqFile, resFile), (seqFileProbes, resFileProbes)]:
                        
            e.evalSequenceFilePerf(si, ri, filterSequencesFilePath="", 
                                   noHeaderSequences=False, noHeaderResults=False, evalNormalizedScores=True,
                                   evalTransactionLevel=True, evalTrajectoryLevel=True,
                                   outputMergedEvalFile=True, evalBackwardCompatibility=False, verbosity=2)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='batch process camera-specific results')
    parser.add_argument(dest='sequencesDir', metavar='sequencesDir', type=str, help="path to 'sequences-info' files")
    parser.add_argument(dest='resultsDir', metavar='resultsDir', type=str, help="path to 'results' files")
    args = parser.parse_args()
    batch_process_camera(args.sequencesDir, args.resultsDir)
    
