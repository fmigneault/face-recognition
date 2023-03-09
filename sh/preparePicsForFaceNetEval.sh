picDir=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/yolo_org_1/
newLocation=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/cropped-faces-for-tests/

for sequenceDirectory in $picDir*/ ; do
    sequenceName=$(basename $sequenceDirectory)
    if [[ ${sequenceName:4:2} == 'S1' ]]; then 
        for personIdDirectory in $sequenceDirectory/*; do 
            personId=$(basename $personIdDirectory)
            if [ ${#personId} -eq 4 ]; then 
                for pics in $personIdDirectory/*; do 
                    picFilename=$(basename $pics)
                    newPicFilename=$personId-$sequenceName-$picFilename 
                    newPicFilepath=$newLocation/$newPicFilename
                    cp $pics $newPicFilepath
                    echo $pics
                done
            fi
        done
    fi
done