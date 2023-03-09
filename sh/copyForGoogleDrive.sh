inputDir=/home/sam/Desktop/link_to_passport/chokepoint/stricter-face-threshold/
outputDir=/home/sam/Desktop/link_to_passport/chokepoint/drive-fast-dt-stricter-face-threshold/

for sequenceDirectory in $inputDir*/ ; do
    fileName=$(basename $sequenceDirectory)
    echo $fileName
    if mkdir $outputDir$fileName ; then 
        cp $sequenceDirectory$fileName.mp4 $outputDir$fileName
        cp $sequenceDirectory/outputBboxes.txt $outputDir$fileName
        cp $sequenceDirectory/generalOutput.txt $outputDir$fileName
        cp $sequenceDirectory/evaluation.txt $outputDir$fileName
        cp -r $sequenceDirectory/images/ $outputDir$fileName
    else 
        echo -e "\033[33mOutput directory $newDir already exists! Did not do anything\033[0m"
    fi
    # outputDir=output
    # fileDir=$sequenceDirectory$outputDir
    # ffmpeg -y -i $fileDir/%*.png -vcodec h264 $sequenceDirectory$fileName.mp4
done