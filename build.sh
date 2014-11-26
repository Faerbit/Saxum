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
    cmake="cmake -DCMAKE_SYSTEM_NAME='Windows' -DCMAKE_FIND_ROOT_PATH='/usr/bin:/usr/x86_64-w64-mingw32' -DCMAKE_CXX_COMPILER='x86_64-w64-mingw32-g++' -DCMAKE_C_COMPILER='x86_64-w64-mingw32-gcc'"
fi

currentDir=$(pwd)
threads=$(($(nproc)+1))

#building bullet

cd extern/bullet/
mkdir -p build
cd build
if hash ninja 2>/dev/null
then
    $cmake -DBUILD_DEMOS=0 -DBUILD_EXTRAS=0 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 -GNinja ..
    ninja
else
    $cmake -DBUILD_DEMOS=0 -DBUILD_EXTRAS=0 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 ..
    make -j$threads
fi

cd $currentDir

#Building app

mkdir -p build
cd build 
if hash ninja 2>/dev/null
then
    $cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 -GNinja ..
    ninja
else
    $cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 ..
    make -j$threads
fi
