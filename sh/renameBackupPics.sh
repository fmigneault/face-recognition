#!/bin/bash
picDir=/media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/cropped-faces-probes/cascades/posID24

for pic in $picDir/* ; do
    fileName=$(basename $pic)
    if [[ $fileName == *"~"* ]]; then
        echo $pic
        NEW_UUID=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
        echo $NEW_UUID.png
        mv $pic $picDir/$NEW_UUID.png
    fi
done