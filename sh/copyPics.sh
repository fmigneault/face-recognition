#!/bin/bash
picDir=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/cropped-faces-fast-dt/color_livia/
targetNegDir=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/cropped-faces-probes/cascades/neg
targetPosDir=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/cropped-faces-probes/cascades/posID24

for sequenceDirectory in $picDir*/ ; do
    fileName=$(basename $sequenceDirectory)
    echo ${fileName:4:2}
    if [ ${fileName:4:2} == 'S1' ]; then # if you only want 1 sequence etc
        for personIdDirectory in $sequenceDirectory*/; do 
            probeNeg1="0002/"
            probeNeg2="0004/"
            probeNeg3="0012/"
            probeNeg4="0014/"
            probeNeg5="0015/"
            probeNeg6="0016/"
            probeNeg7="0019/"
            probeNeg8="0022/"
            probeNeg9="0023/"
            probePos1="0024/"
            echo $sequenceDirectory$dir4
            echo $personIdDirectory

            # if [ $personIdDirectory == "$sequenceDirectory$probeNeg1" ] || [ $personIdDirectory == "$sequenceDirectory$probeNeg2" ] || 
            #    [ $personIdDirectory == "$sequenceDirectory$probeNeg3" ] || [ $personIdDirectory == "$sequenceDirectory$probeNeg4" ] ||
            #    [ $personIdDirectory == "$sequenceDirectory$probeNeg5" ] || [ $personIdDirectory == "$sequenceDirectory$probeNeg6" ] || 
            #    [ $personIdDirectory == "$sequenceDirectory$probeNeg7" ] || [ $personIdDirectory == "$sequenceDirectory$probeNeg8" ] || 
            #    [ $personIdDirectory == "$sequenceDirectory$probeNeg9" ] ||  [ $personIdDirectory == "$sequenceDirectory$probeNeg10" ]; then
            #     for file in $personIdDirectory*; do
            #         echo $file
            #         cp -r --backup=numbered $file $targetNegDir
            #     done 
            # fi

            if [ $personIdDirectory == "$sequenceDirectory$probePos1" ]; then
                for file in $personIdDirectory*; do
                    echo $file
                    cp -r --backup=numbered $file $targetPosDir
                done 
            fi
        done
    fi
done