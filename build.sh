#!/bin/bash

currrentDir=$(pwd)

#building bullet

cd extern/bullet/
mkdir -p build
cmake ..
make -j5

cd $currentDir

#Building app

mkdir -p build
cd build 
cmake .. 
make -j5
