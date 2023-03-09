#!/bin/bash
for file in $(ls -p | grep -v / | tail -822)
do
    cp $file /media/livia/0a6bd2c7-af4e-476b-993f-e1e7f8dddb99/cropped-faces-probes/cascades/total-pos-id3/seg1/
done