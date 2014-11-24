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

#./build.sh clean

if hash ninja 2>/dev/null
then
    ./build.sh $platform ninja
else
    ./build.sh $platform
fi

rc=$?

if [[ $rc == 0 ]]
then
    rm -rf "MarbleGame_${platform^}" "MarbleGame_${platform^}.zip"
    mkdir  "MarbleGame_${platform^}"
    cp -r binaries Levels Shader "MarbleGame_${platform^}"
    zip -r "MarbleGame_${platform^}.zip" "MarbleGame_${platform^}"
    if hash megacmd 2>/dev/null
    then
        megacmd -force put "MarbleGame_${platform^}.zip" mega:/GameBuilds/
    fi
    rm -rf "MarbleGame_${platform^}" 
fi
