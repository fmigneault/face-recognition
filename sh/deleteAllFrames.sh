uncompressedVideosDirectory=/home/sam/data/original/chokepoint/uncompressed/
outputDirectory=/media/sam/passport/data/chokepoint/3face_compressive_localized_opencv3.2-3/

for sequenceDirectory in $uncompressedVideosDirectory*/ ; do
    for filePath in $sequenceDirectory*.mkv ; do
        # Create Directory for current video
        fileName=$(basename $filePath)
        fileNameWithoutExtension="${fileName%.*}"
        newDir=$outputDirectory$fileNameWithoutExtension

        echo $newDir/output/
        rm -rf $newDir/output/
    done
done


# ./FAST-DT.exe ~/data/original/chokepoint/uncompressed/P1E_S1/P1E_S1_C1.mkv ~/data/original/chokepoint/uncompressed/P1E_S1/P1E_S1_C1/ $HAAR_CASCADE_DIR -o output/

