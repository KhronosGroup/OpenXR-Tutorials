#!/bin/bash

rm -rf build/common_archs
mkdir -p build/common_archs

for api in "$@"
do
    if [[ "$api" == "D3D11" ]]; then
        # D3D11
        echo "$api"
        zip -r build/common_archs/Common_D3D11.zip \
            Common/GraphicsAPI.cpp \
            Common/GraphicsAPI_D3D11.cpp \
            Common/OpenXRDebugUtils.cpp \
            Common/DebugOutput.h \
            Common/GraphicsAPI.h \
            Common/GraphicsAPI_D3D11.h \
            Common/HelperFunctions.h \
            Common/OpenXRDebugUtils.h \
            Common/OpenXRHelper.h \
            Common/xr_linear_algebra.h
    fi
    if [[ "$api" == "D3D12" ]]; then
        # D3D12
        echo "$api"
        zip -r build/common_archs/Common_D3D12.zip \
            Common/GraphicsAPI.cpp \
            Common/GraphicsAPI_D3D12.cpp \
            Common/OpenXRDebugUtils.cpp \
            Common/d3dx12.h \
            Common/DebugOutput.h \
            Common/GraphicsAPI.h \
            Common/GraphicsAPI_D3D12.h \
            Common/HelperFunctions.h \
            Common/OpenXRDebugUtils.h \
            Common/OpenXRHelper.h \
            Common/xr_linear_algebra.h
    fi
    if [[ "$api" == "OPENGL" ]]; then
        # OPENGL
        echo "$api"
        zip -r build/common_archs/Common_OpenGL.zip \
            Common/GraphicsAPI.cpp \
            Common/GraphicsAPI_OpenGL.cpp \
            Common/OpenXRDebugUtils.cpp \
            Common/DebugOutput.h \
            Common/GraphicsAPI.h \
            Common/GraphicsAPI_OpenGL.h \
            Common/HelperFunctions.h \
            Common/OpenXRDebugUtils.h \
            Common/OpenXRHelper.h \
            Common/xr_linear_algebra.h
    fi
    if [[ "$api" == "OPENGL_ES" ]]; then
        # OPENGL_ES
        echo "$api"
        zip -r build/common_archs/Common_OpenGL_ES.zip \
            Common/GraphicsAPI.cpp \
            Common/GraphicsAPI_OpenGL_ES.cpp \
            Common/OpenXRDebugUtils.cpp \
            Common/DebugOutput.h \
            Common/GraphicsAPI.h \
            Common/GraphicsAPI_OpenGL_ES.h \
            Common/HelperFunctions.h \
            Common/OpenXRDebugUtils.h \
            Common/OpenXRHelper.h \
            Common/xr_linear_algebra.h
    fi
    if [[ "$api" == "VULKAN" ]]; then
        # VULKAN
        echo "$api"
        zip -r build/common_archs/Common_Vulkan.zip \
            Common/GraphicsAPI.cpp \
            Common/GraphicsAPI_Vulkan.cpp \
            Common/OpenXRDebugUtils.cpp \
            Common/DebugOutput.h \
            Common/GraphicsAPI.h \
            Common/GraphicsAPI_Vulkan.h \
            Common/HelperFunctions.h \
            Common/OpenXRDebugUtils.h \
            Common/OpenXRHelper.h \
            Common/xr_linear_algebra.h
    fi
done

# Full Folder
echo "ALL"
zip -r build/common_archs/Common.zip \
    Common/GraphicsAPI.cpp \
    Common/GraphicsAPI_D3D11.cpp \
    Common/GraphicsAPI_D3D12.cpp \
    Common/GraphicsAPI_OpenGL.cpp \
    Common/GraphicsAPI_OpenGL_ES.cpp \
    Common/GraphicsAPI_Vulkan.cpp \
    Common/OpenXRDebugUtils.cpp \
    Common/d3dx12.h \
    Common/DebugOutput.h \
    Common/GraphicsAPI.h \
    Common/GraphicsAPI_D3D11.h \
    Common/GraphicsAPI_D3D12.h \
    Common/GraphicsAPI_OpenGL.h \
    Common/GraphicsAPI_OpenGL_ES.h \
    Common/GraphicsAPI_Vulkan.h \
    Common/HelperFunctions.h \
    Common/OpenXRDebugUtils.h \
    Common/OpenXRHelper.h  \
    Common/xr_linear_algebra.h
