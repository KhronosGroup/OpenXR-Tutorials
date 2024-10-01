#!/bin/bash

#Make destination folders
mkdir -p build/eoc_archs/Chapter1
mkdir -p build/eoc_archs/Chapter2
mkdir -p build/eoc_archs/Chapter3
mkdir -p build/eoc_archs/Chapter4
mkdir -p build/eoc_archs/Chapter5
mkdir -p build/eoc_archs/cmake
mkdir -p build/eoc_archs/Common
mkdir -p build/eoc_archs/Shaders

#Copy the required files and folder to the eoc_archs folder
cp Chapter1/main.cpp Chapter1/CMakeLists.txt build/eoc_archs/Chapter1
cp Chapter2/main.cpp Chapter2/CMakeLists.txt build/eoc_archs/Chapter2
cp Chapter3/main.cpp Chapter3/CMakeLists.txt build/eoc_archs/Chapter3
cp Chapter4/main.cpp Chapter4/CMakeLists.txt build/eoc_archs/Chapter4
cp Chapter5/main.cpp Chapter5/CMakeLists.txt build/eoc_archs/Chapter5
cp -r cmake                                  build/eoc_archs
cp -r Common                                 build/eoc_archs
cp -r Shaders                                build/eoc_archs

#Remove XR_DOCS_TAG_ lines from files
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter1/main.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter2/main.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter3/main.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter4/main.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter5/main.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter1/CMakeLists.txt
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter2/CMakeLists.txt
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter3/CMakeLists.txt
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter4/CMakeLists.txt
python eoc-archs-rm-tags.py -f build/eoc_archs/Chapter5/CMakeLists.txt

python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_D3D11.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_D3D11.h
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_D3D12.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_D3D12.h
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_OpenGL.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_OpenGL.h
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_OpenGL_ES.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_OpenGL_ES.h
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_Vulkan.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI_Vulkan.h
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/GraphicsAPI.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/HelperFunctions.h
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/OpenXRDebugUtils.cpp
python eoc-archs-rm-tags.py -f build/eoc_archs/Common/OpenXRHelper.h

#Zip the folders
pushd build/eoc_archs
cp -f ./../../thirdparty/CMakeLists_Ch1_Main.txt CMakeLists.txt
zip -r Chapter1.zip Chapter1 CMakeLists.txt cmake Common

cp -f ./../../thirdparty/CMakeLists_Ch2_Main.txt CMakeLists.txt
zip -r Chapter2.zip Chapter2 CMakeLists.txt cmake Common

cp -f ./../../thirdparty/CMakeLists_Ch3_Main.txt CMakeLists.txt
zip -r Chapter3.zip Chapter3 CMakeLists.txt cmake Common Shaders

cp -f ./../../thirdparty/CMakeLists_Ch4_Main.txt CMakeLists.txt
zip -r Chapter4.zip Chapter4 CMakeLists.txt cmake Common Shaders

cp -f ./../../thirdparty/CMakeLists_Ch5_Main.txt CMakeLists.txt
zip -r Chapter5.zip Chapter5 CMakeLists.txt cmake Common Shaders
popd

#Remove destination file and folders
rm -rf build/eoc_archs/Chapter1
rm -rf build/eoc_archs/Chapter2
rm -rf build/eoc_archs/Chapter3
rm -rf build/eoc_archs/Chapter4
rm -rf build/eoc_archs/Chapter5
rm -rf build/eoc_archs/cmake
rm -rf build/eoc_archs/Common
rm -rf build/eoc_archs/Shaders
rm build/eoc_archs/CMakeLists.txt
