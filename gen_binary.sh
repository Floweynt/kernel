#!/bin/sh
set -xe

filename=$4
maxsize=2000000
filesize=$(stat -c%s "$filename")
echo "Size of $filename = $filesize bytes."

if (( filesize > maxsize )); then
    echo "nope"
    exit -1
else
    echo "fine"
fi

echo "Root:" $1
echo "Boot:" $2
echo "Stage 2:" $3
echo "Kernel:" $4
echo "Out:" $5
cat "$1"/"$2" "$1"/"$3" "$1"/"$4" > "$1"/"$5"
truncate -s 10321920 "$1"/"$5"
