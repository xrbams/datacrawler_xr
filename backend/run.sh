#!/bin/bash

mkdir -p build && cd build

# Run make clean if build directory already exists
if [ -f Makefile ]; then
    make clean
fi

cmake ..
make -j$(nproc)
./Dunna

