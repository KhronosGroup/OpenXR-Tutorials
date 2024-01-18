#!/bin/bash

mkdir -p build/_deps
pushd build/_deps
git clone --depth 1 https://github.com/KhronosGroup/OpenXR-SDK.git openxr-build
popd

python3 -m pip install -r requirements.txt

export OPENXR_TUTORIALS_GIT_TAG=$1

sphinx-build -c tutorial -M html tutorial build index.rst -t OPENXR_MAINSITE -D html_baseurl=https://openxr-tutorial.com/

sphinx-build -M html tutorial build/windows/vulkan -t windows -t vulkan -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/ 
sphinx-build -M html tutorial build/windows/opengl -t windows -t opengl -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/ 
sphinx-build -M html tutorial build/windows/d3d11 -t windows -t d3d11 -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/ 
sphinx-build -M html tutorial build/windows/d3d12 -t windows -t d3d12 -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/

sphinx-build -a -M html tutorial build/linux/vulkan -t linux -t vulkan -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/
sphinx-build -a -M html tutorial build/linux/opengl -t linux -t opengl -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/

sphinx-build -a -M html tutorial build/android/vulkan -t android -t vulkan -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/
sphinx-build -a -M html tutorial build/android/opengles -t android -t opengles -t OPENXR_SUBSITE -D html_baseurl=https://openxr-tutorial.com/ 

unset OPENXR_TUTORIALS_GIT_TAG

tar -czvf site.tar.gz build/html
