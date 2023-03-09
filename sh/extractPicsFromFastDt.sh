picDir=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/ghenadie-vids/

for sequenceDirectory in $picDir*/ ; do
    mkdir $sequenceDirectory/allImages
    for personIdDirectory in $sequenceDirectory/images/*; do 
        echo $personIdDirectory
        cp $personIdDirectory/* $sequenceDirectory/allImages
    done
done