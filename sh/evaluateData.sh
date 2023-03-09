baseDir=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/yolo_org_1/
groundTruthDir=/home/livia/data/original/chokepoint_groundtruth/
nodeProgPath=/home/livia/code/evaluation

for sequenceDirectory in $baseDir*/ ; do
    sequenceName=$(basename $sequenceDirectory)
    groundTruthPath=$groundTruthDir$sequenceName
    outputBboxesPath=$sequenceDirectory/outputBboxes.txt
    echo $groundTruthPath.xml
    echo $outputBboxesPath
    node $nodeProgPath/index.js $groundTruthPath.xml $outputBboxesPath
    cp $nodeProgPath/output.txt $sequenceDirectory/evaluation.txt
    # rm $nodeProgPath/output.txt
    # fileName=outputBboxes.txt
    # echo $sequenceDirectory$fileName
    # grep -v '^\.' $sequenceDirectory$fileName > $sequenceDirectory/temp.txt
    # cp $baseDir/output.txt $sequenceDirectory$fileName
    # rm  $sequenceDirectory/temp.txt
done
