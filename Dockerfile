from ubuntu:trusty

run apt-get update
run apt-get upgrade

run apt-get install -y build-essential
run apt-get install -y gcc-mingw-w64
run apt-get install -y cmake
run apt-get install -y ninja-build
run apt-get install -y xorg-dev
run apt-get install -y freeglut3-dev
run apt-get install -y zip
