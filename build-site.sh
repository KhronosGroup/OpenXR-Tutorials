#!/bin/bash

mkdir -p build/_deps
pushd build/_deps
git clone --depth 1 https://github.com/KhronosGroup/OpenXR-SDK.git openxr-build
popd

python3 -m pip install -r requirements.txt

sphinx-build -M html tutorial build

tar -czvf site.tar.gz build/html
