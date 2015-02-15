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

./build.sh $platform

rc=$?

if [[ $rc == 0 ]]
then
    mkdir  "MarbleGame_${platform^}"
    cp -r binaries data "MarbleGame_${platform^}"
    zip -r "MarbleGame_${platform^}.zip" "MarbleGame_${platform^}"
    rm -rf "MarbleGame_${platform^}" 
fi
