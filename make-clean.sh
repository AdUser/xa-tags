#!/bin/bash

make clean

rm -f *.cmake
rm -f CMakeCache.txt
rm -f DartConfiguration.tcl

find ./ -type d -a -name Testing    -exec rm -rf {} +
find ./ -type d -a -name CMakeFiles -exec rm -rf {} +
find ./ -type f -a -name *.cmake    -exec rm     {} +
find ./ -type f -a -name Makefile   -exec rm     {} +
