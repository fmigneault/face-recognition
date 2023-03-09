fastDtChokepointDataPath=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/cropped-faces-fast-dt/color_livia/
nodeProgPath=/home/livia/code/prepare-fast-dt-images-for-tests
groundTruthDir=/home/livia/data/original/chokepoint_groundtruth/

for sequenceDirectory in $fastDtChokepointDataPath*/ ; do
    sequenceName=$(basename $sequenceDirectory)
    groundTruthPath=$groundTruthDir$sequenceName
    echo $groundTruthPath.xml
    echo $sequenceDirectory
    node $nodeProgPath/index.js $groundTruthPath.xml $sequenceDirectory
done
