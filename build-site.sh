#!/bin/bash

mkdir -p build/_deps
pushd build/_deps
git clone --depth 1 https://github.com/KhronosGroup/OpenXR-SDK.git openxr-build
popd

python3 -m pip install -r requirements.txt

mkdir -p build/html/android/opengles
mkdir -p build/html/android/vulkan
mkdir -p build/html/linux/opengl
mkdir -p build/html/linux/vulkan
mkdir -p build/html/windows/d3d11
mkdir -p build/html/windows/d3d12
mkdir -p build/html/windows/opengl
mkdir -p build/html/windows/vulkan

export OPENXR_TUTORIALS_GIT_TAG=$1

sphinx-build -M html tutorial build index.rst -t OPENXR_MAINSITE -D html_baseurl=https://openxr-tutorial.com/

sphinx-build -M html tutorial build/android/opengles -t android -t opengles -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/
sphinx-build -M html tutorial build/android/vulkan -t android -t vulkan -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/

sphinx-build -M html tutorial build/linux/opengl -t linux -t opengl -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/
sphinx-build -M html tutorial build/linux/vulkan -t linux -t vulkan -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/

sphinx-build -M html tutorial build/windows/d3d11 -t windows -t d3d11 -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/
sphinx-build -M html tutorial build/windows/d3d12 -t windows -t d3d12 -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/
sphinx-build -M html tutorial build/windows/opengl -t windows -t opengl -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/
sphinx-build -M html tutorial build/windows/vulkan -t windows -t vulkan -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/

unset OPENXR_TUTORIALS_GIT_TAG

cp -r build/android/opengles/html/. build/html/android/opengles
cp -r build/android/vulkan/html/.   build/html/android/vulkan
cp -r build/linux/opengl/html/.     build/html/linux/opengl
cp -r build/linux/vulkan/html/.     build/html/linux/vulkan
cp -r build/windows/d3d11/html/.    build/html/windows/d3d11
cp -r build/windows/d3d12/html/.    build/html/windows/d3d12
cp -r build/windows/opengl/html/.   build/html/windows/opengl
cp -r build/windows/vulkan/html/.   build/html/windows/vulkan

tar -czvf site.tar.gz build/html
