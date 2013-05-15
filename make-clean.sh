#!/bin/sh

make clean

rm -f *.cmake
rm -f CMakeCache.txt
rm -f DartConfiguration.tcl
rm -f ./po/*.mo
rm -f ./po/*.pot
rm -f ./tests/scripts/*.test.out
rm -f debian/*.log
rm -f debian/*.substvars
rm -f debian/xa-tags-embedded/ -r
rm -f debian/xa-tags/ -r
rm -f debian/files
rm -f debian/tmp/ -r
rm -f obj*/ -r

find ./ -type d -a -name Testing    -exec rm -rf {} +
find ./ -type d -a -name CMakeFiles -exec rm -rf {} +
find ./ -type f -a -name Makefile   -exec rm     {} +
find ./tests/ -type f -a -name *.cmake    -exec rm     {} +
find ./src/   -type f -a -name *.cmake    -exec rm     {} +
find ./po/    -type f -a -name *.cmake    -exec rm     {} +
