# -*- coding: utf-8 -*-
"""
Calls 'frameRenameGenerate' for every directory specified in 'sequences.txt'
"""

import os, posixpath
import frameRenameGenerate as FRG

def main():
    sequencesFile = open('./sequences.txt', 'r')
    sequencesPaths = sequencesFile.read().split("\n")
    sequencesFile.close()
    for p in sequencesPaths:
        outdir = posixpath.join(p, 'corrected')
        if (not os.path.exists(outdir)):
            os.mkdir(outdir)
        FRG.frameRenameGenerate(p, '.jpg', '08d', outdir, overwrite=False)

if __name__ == "__main__":
    main()