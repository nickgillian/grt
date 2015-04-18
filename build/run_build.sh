#!/bin/bash

#if the temporary build directory does not exist, make it
if [ ! -d "$build" ]; then
    mkdir build
fi

#cd to the temp build directory
cd build

#run cmake
cmake ..

#run make
make

