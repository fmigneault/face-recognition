# -*- coding: utf-8 -*-
"""
Frame id offset applied to start with first frame at '0'.
Also pads missing frames ids by duplicating the previous frame.
All output (rename/generated frames) is printed in the console and written to debug file.
"""

import os, re, shutil, posixpath


def correctFrameSequence(frameBasePath, extension='.jpg', fmt='08d', outdir='', overwrite=False):
    assert(os.path.isdir(frameBasePath))
    first = -1
    prev = 0

    if (outdir == ''):
        outputDir = frameBasePath
    else:
        outputDir = outdir

    outputFile = posixpath.join(outputDir, 'corrected.txt')
    outputFile = open(outputFile, 'w+')
    regex = re.compile(r'\d+')
    for fileName in os.listdir(frameBasePath):
        if fileName.endswith(extension):
            if (first < 0):
                first = int(regex.findall(fileName)[0])
                prev = first

            # Get frame id with offset
            newValue = int(regex.findall(fileName)[0]) - first

            # Generate if missing frames
            while (newValue - prev > 1):
                nextValue = prev + 1
                nextName = format(nextValue, fmt) + extension
                nextFile = posixpath.join(outputDir, nextName)
                curFile = posixpath.join(frameBasePath, fileName)
                if (os.path.exists(nextFile) and not overwrite):
                    print("Skipped: " + nextName + " already exists [" + outputDir + "]")
                    print("Skipped: " + nextName + " already exists [" + outputDir + "]", file=outputFile)
                else:
                    shutil.copyfile(curFile, nextFile)
                    print("Generated: " + nextName + " [" + outputDir + "]")
                    print("Generated: " + nextName + " [" + outputDir + "]", file=outputFile)
                prev = nextValue
            prev = newValue

            # Rename existing frame id
            newName = format(newValue, fmt) + extension
            newFile = posixpath.join(outputDir, newName)
            oldFile = posixpath.join(frameBasePath, fileName)
            if (os.path.exists(newFile) and not overwrite):
                print("Skipped: " + newName + " already exists [" + outputDir + "]")
                print("Skipped: " + newName + " already exists [" + outputDir + "]", file=outputFile)
                continue
            if (outputDir == frameBasePath):
                os.rename(oldFile, newFile)
                print("Renamed: " + fileName + " -> " + newName + " [" + outputDir + "]")
                print("Renamed: " + fileName + " -> " + newName + " [" + outputDir + "]", file=outputFile)
            else:
                shutil.copyfile(oldFile, newFile)
                print("Copied: " + fileName + " -> " + newName + " [" + outputDir + "]")
                print("Copied: " + fileName + " -> " + newName + " [" + outputDir + "]", file=outputFile)

    outputFile.close()

if __name__ == "__main__":
    correctFrameSequence('')