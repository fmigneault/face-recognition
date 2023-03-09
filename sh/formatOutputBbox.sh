# baseDir=/home/sam/data/outputs/chokepoint/
baseDir=/home/sam/Desktop/link_to_passport/chokepoint/output-FAST-DT/

for sequenceDirectory in $baseDir*/ ; do
    fileName=outputBboxes.txt
    echo $sequenceDirectory$fileName
    grep -v '^\.' $sequenceDirectory$fileName > $sequenceDirectory/temp.txt
    cp $sequenceDirectory/temp.txt $sequenceDirectory$fileName
    # rm  $sequenceDirectory/temp.txt
done
