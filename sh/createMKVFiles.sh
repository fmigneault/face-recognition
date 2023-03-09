baseDir=/home/sam/data/original/chokepoint/uncompressed/

for sequenceDirectory in $baseDir*/ ; do
    for cameraSubdirectory in $sequenceDirectory*/; do 
        # fileName=$(basename $cameraSubdirectory)
        echo $cameraSubdirectory
        ls -1 $cameraSubdirectory*.jpg > /dev/null 2>&1
        if [ "$?" = "0" ]; then # .jpg exists in this file
            fileName=$(basename $cameraSubdirectory)
            ffmpeg -i $cameraSubdirectory%*.jpg -codec copy $sequenceDirectory$fileName.mkv
        else  # Need to go a level deeper
            for cameraSubSubdirectory in $cameraSubdirectory*/; do 
                fileName=$(basename $cameraSubSubdirectory)
                ffmpeg -i $cameraSubSubdirectory%*.jpg -codec copy $sequenceDirectory$fileName.mkv
            done
        fi
    done
done