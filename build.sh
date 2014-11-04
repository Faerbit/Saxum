#!/bin/bash

currentDir=$(pwd)
threads=$(($(nproc)+1))

#building bullet

cd extern/bullet/
mkdir -p build
cd build
cmake ..
make -j$threads

cd $currentDir

#Building app

mkdir -p build
cd build 
cmake .. 
make -j$threads
