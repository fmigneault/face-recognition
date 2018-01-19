import os, sys, csv
import argparse
import extract_probes as ex
import extractSequencesChokePoint

SEQUENCES_SESSION = [
    "frontal",
    "S1",
    "S2",
    "S3",
    "S4",
]


SEQUENCES_CAMERA = [
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
    "P2L_C3",
]


FILES_EXTENSION = '.txt'


def get_probe_results(sequences_dir, results_dir, probes_file, skip_camera_sequences, skip_session_sequences):
    assert(os.path.isdir(sequences_dir))
    assert(os.path.isdir(results_dir))
    if skip_camera_sequences and skip_session_sequences:
        print("Nothing to execute with every sequence skipped")
        return
    
    sequences = list()
    if not skip_camera_sequences:
        sequences += SEQUENCES_CAMERA
    if not skip_session_sequences:
        sequences += SEQUENCES_SESSION        
        
    for seq in sequences:
        sequences_file = os.path.join(sequences_dir, 'sequences-' + seq + '-info' + FILES_EXTENSION)
        results_file = os.path.join(results_dir, 'results-' + seq + FILES_EXTENSION)
        ex.get_probe_results(sequences_file, results_file, probes_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Extract probes from results file of every camera domain')
    parser.add_argument(dest='sequencesFileDir', metavar='sequencesFileDir', type=str)
    parser.add_argument(dest='resultsFileDir', metavar='resultsFileDir', type=str)
    parser.add_argument(dest='probesFile', metavar='probesFile', type=str)
    parser.add_argument('--no-camera', dest='no_camera', action='store_true')
    parser.add_argument('--no-session', dest='no_session', action='store_true')    
    args = parser.parse_args()
    get_probe_results(args.sequencesFileDir, args.resultsFileDir, args.probesFile, args.no_camera, args.no_session)
    
