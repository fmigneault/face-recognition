baseDir=/media/sam/passport/data/chokepoint/1face_camshift/

for sequenceDirectory in $baseDir*/ ; do
    fileName=$(basename $sequenceDirectory)
    echo $fileName
    outputDir=output
    fileDir=$sequenceDirectory$outputDir
    ffmpeg -y -i $fileDir/%*.png -vcodec h264 $sequenceDirectory$fileName.mp4
done