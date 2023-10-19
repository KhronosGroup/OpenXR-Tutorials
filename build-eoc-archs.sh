#!/bin/bash

#Make destination folders
mkdir -p eoc_archs/Chapter2
mkdir -p eoc_archs/Chapter3
mkdir -p eoc_archs/Chapter4
mkdir -p eoc_archs/Chapter5

#Copy the required files to the eoc-archs folder
cp Chapter2/main.cpp Chapter2/CMakeLists.txt eoc_archs/Chapter2
cp Chapter3/main.cpp Chapter3/CMakeLists.txt eoc_archs/Chapter3
cp Chapter4/main.cpp Chapter4/CMakeLists.txt eoc_archs/Chapter4
cp Chapter5/main.cpp Chapter5/CMakeLists.txt eoc_archs/Chapter5

#Zip the folders
pushd eoc_archs
zip -r Chapter2.zip Chapter2
zip -r Chapter3.zip Chapter3
zip -r Chapter4.zip Chapter4
zip -r Chapter5.zip Chapter5
popd

#Remove destination folders
rm -rf eoc_archs/Chapter2
rm -rf eoc_archs/Chapter3
rm -rf eoc_archs/Chapter4
rm -rf eoc_archs/Chapter5