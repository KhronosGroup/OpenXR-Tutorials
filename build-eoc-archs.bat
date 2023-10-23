echo off

rem Make destination folders
mkdir build\eoc_archs\Chapter2
mkdir build\eoc_archs\Chapter3
mkdir build\eoc_archs\Chapter4
mkdir build\eoc_archs\Chapter5
mkdir build\eoc_archs\cmake
mkdir build\eoc_archs\Common
mkdir build\eoc_archs\Shaders

rem Copy the required files and folder to the eoc-archs folder
copy Chapter2\main.cpp       build\eoc_archs\Chapter2
copy Chapter2\CMakeLists.txt build\eoc_archs\Chapter2
copy Chapter3\main.cpp       build\eoc_archs\Chapter3
copy Chapter3\CMakeLists.txt build\eoc_archs\Chapter3
copy Chapter4\main.cpp       build\eoc_archs\Chapter4
copy Chapter4\CMakeLists.txt build\eoc_archs\Chapter4
copy Chapter5\main.cpp       build\eoc_archs\Chapter5
copy Chapter5\CMakeLists.txt build\eoc_archs\Chapter5
copy cmake                   build\eoc_archs\cmake
copy Common                  build\eoc_archs\Common
copy Shaders                 build\eoc_archs\Shaders

rem Remove XR_DOCS_TAG_ lines from files
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter2\main.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter3\main.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter4\main.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter5\main.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter2\CMakeLists.txt
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter3\CMakeLists.txt
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter4\CMakeLists.txt
python eoc-archs-rm-tags.py -f build\eoc_archs\Chapter5\CMakeLists.txt

python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_D3D11.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_D3D11.h
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_D3D12.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_D3D12.h
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_OpenGL.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_OpenGL.h
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_OpenGL_ES.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_OpenGL_ES.h
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_Vulkan.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI_Vulkan.h
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\GraphicsAPI.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\HelperFunctions.h
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\OpenXRDebugUtils.cpp
python eoc-archs-rm-tags.py -f build\eoc_archs\Common\OpenXRHelper.h

rem Copy in custom CMakeLists.txt file and create zip archives
pushd build\eoc_archs
copy /y .\..\..\thirdparty\CMakeLists_Ch2_Main.txt CMakeLists.txt
tar -a -cf Chapter2.zip Chapter2 CMakeLists.txt cmake Common

copy /y .\..\..\thirdparty\CMakeLists_Ch3_Main.txt CMakeLists.txt
tar -a -cf Chapter3.zip Chapter3 CMakeLists.txt cmake Common Shaders

copy /y .\..\..\thirdparty\CMakeLists_Ch4_Main.txt CMakeLists.txt
tar -a -cf Chapter4.zip Chapter4 CMakeLists.txt cmake Common Shaders

copy /y .\..\..\thirdparty\CMakeLists_Ch5_Main.txt CMakeLists.txt
tar -a -cf Chapter5.zip Chapter5 CMakeLists.txt cmake Common Shaders
popd

rem Remove destination file and folders
rmdir /s /q build\eoc_archs\Chapter2
rmdir /s /q build\eoc_archs\Chapter3
rmdir /s /q build\eoc_archs\Chapter4
rmdir /s /q build\eoc_archs\Chapter5
rmdir /s /q build\eoc_archs\cmake
rmdir /s /q build\eoc_archs\Common
rmdir /s /q build\eoc_archs\Shaders
del build\eoc_archs\CMakeLists.txt