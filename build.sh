#!/bin/bash

if [[ $1 == "clean" ]]
then
    rm -rf build
    rm -rf binaries
    rm -rf extern/bullet/build
    exit
fi

cmake="cmake"

if [[ $1 == "windows" ]]
then
    cmake=x86_64-w64-mingw32-cmake
fi

currentDir=$(pwd)
threads=$(($(nproc)+1))

#building bullet

cd extern/bullet/
mkdir -p build
cd build
$cmake -DBUILD_DEMOS=0 -DBUILD_EXTRAS=0 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 ..
make -j$threads

cd $currentDir

#Building app

mkdir -p build
cd build 
$cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 ..
make -j$threads
