#!/bin/bash

if ./build.sh
then
    cd binaries; ./saxum
fi
