#!/bin/bash

if [[ $1 == "" ]]
then
    echo "Please select a target platform (linux or windows)"
    exit
elif [[ $1 == "linux" ]]
then
    platform="linux"
elif [[ $1 == "windows" ]]
then
    platform="windows"
fi

binaries=$(ls binaries)
if [[ $binaries == *.exe && $platform == linux ]]
then
    ./build.sh clean
elif [[ $binaries != *.exe && $platform == windows ]]
then
    ./build.sh clean
fi

./build.sh $platform Release

rc=$?

if [[ $rc == 0 ]]
then
    cd binaries; strip -s *; cd ..
    mkdir  "Saxum_${platform^}"
    cp -r binaries data LICENSE.txt "Saxum_${platform^}"
    zip -r "Saxum_${platform^}.zip" "Saxum_${platform^}"
    rm -rf "Saxum_${platform^}"
fi
