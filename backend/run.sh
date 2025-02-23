#!/bin/bash

cd build
rm -rf *

cmake ..
make 
./git_b

