uncompressedVideosDirectory=/home/sam/data/original/chokepoint/uncompressed/
outputDirectory=/media/sam/passport/data/chokepoint2/3LBP_camshift3/

for sequenceDirectory in $uncompressedVideosDirectory*/ ; do
    for filePath in $sequenceDirectory*.mkv ; do
        # Create Directory for current video
        fileName=$(basename $filePath)
        echo $sequenceDirectory
        fileNameWithoutExtension="${fileName%.*}"
        newDir=$outputDirectory$fileNameWithoutExtension

        # Find where the originals images are
        if [[ $fileNameWithoutExtension == *"."*  ]] ; then
            intermediaryFolderName=${fileNameWithoutExtension%.*}
            picDirectory=$sequenceDirectory$intermediaryFolderName/$fileNameWithoutExtension
        else 
            picDirectory=$sequenceDirectory$fileNameWithoutExtension
        fi
        
        if mkdir $newDir ; then 
            echo "filePath"
            echo $filePath
            echo $newDir
            echo $picDirectory
            ./FAST-DT.exe $filePath $OPENCV_DATA_DIR -o output/ -p $picDirectory

            # Copy all files/folders to the 
            cp generalOutput.txt $newDir 
            cp timings.txt $newDir
            cp outputBboxes.txt $newDir
            cp -r images $newDir
            ffmpeg -y -i ./output/%*.png -vcodec h264 $newDir.mp4
            cp $newDir.mp4 $newDir

            echo $fileNameWithoutExtension
        else 
            echo -e "\033[33mOutput directory $newDir already exists! Did not do anything\033[0m"
        fi
    done
done


# ./FAST-DT.exe ~/data/original/chokepoint/uncompressed/P1E_S1/P1E_S1_C1.mkv ~/data/original/chokepoint/uncompressed/P1E_S1/P1E_S1_C1/ $HAAR_CASCADE_DIR -o output/

