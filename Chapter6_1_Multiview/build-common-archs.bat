echo off

rmdir /s /q build\common_archs
mkdir build\common_archs

:LOOP
IF "%~1"=="D3D11" GOTO D3D11
IF "%~1"=="D3D12" GOTO D3D12
IF "%~1"=="OPENGL" GOTO OPENGL
IF "%~1"=="OPENGL_ES" GOTO OPENGL_ES
IF "%~1"=="VULKAN" GOTO VULKAN
IF "%~1"=="" GOTO END

rem D3D11
:D3D11
tar -a -cf build\common_archs\Common_D3D11.zip ^
    Common/GraphicsAPI.cpp ^
    Common/GraphicsAPI_D3D11.cpp ^
    Common/OpenXRDebugUtils.cpp ^
    Common/DebugOutput.h ^
    Common/GraphicsAPI.h ^
    Common/GraphicsAPI_D3D11.h ^
    Common/HelperFunctions.h ^
    Common/OpenXRDebugUtils.h ^
    Common/OpenXRHelper.h ^
    Common/xr_linear_algebra.h

SHIFT
GOTO LOOP

rem D3D12
:D3D12
tar -a -cf build\common_archs\Common_D3D12.zip ^
    Common/GraphicsAPI.cpp ^
    Common/GraphicsAPI_D3D12.cpp ^
    Common/OpenXRDebugUtils.cpp ^
    Common/DebugOutput.h ^
    Common/d3dx12.h ^
    Common/GraphicsAPI.h ^
    Common/GraphicsAPI_D3D12.h ^
    Common/HelperFunctions.h ^
    Common/OpenXRDebugUtils.h ^
    Common/OpenXRHelper.h ^
    Common/xr_linear_algebra.h

SHIFT
GOTO LOOP

rem OPENGL
:OPENGL
tar -a -cf build\common_archs\Common_OpenGL.zip ^
    Common/GraphicsAPI.cpp ^
    Common/GraphicsAPI_OpenGL.cpp ^
    Common/OpenXRDebugUtils.cpp ^
    Common/DebugOutput.h ^
    Common/GraphicsAPI.h ^
    Common/GraphicsAPI_OpenGL.h ^
    Common/HelperFunctions.h ^
    Common/OpenXRDebugUtils.h ^
    Common/OpenXRHelper.h ^
    Common/xr_linear_algebra.h

SHIFT
GOTO LOOP

rem OPENGL_ES
:OPENGL_ES
tar -a -cf build\common_archs\Common_OpenGL_ES.zip ^
    Common/GraphicsAPI.cpp ^
    Common/GraphicsAPI_OpenGL_ES.cpp ^
    Common/OpenXRDebugUtils.cpp ^
    Common/DebugOutput.h ^
    Common/GraphicsAPI.h ^
    Common/GraphicsAPI_OpenGL_ES.h ^
    Common/HelperFunctions.h ^
    Common/OpenXRDebugUtils.h ^
    Common/OpenXRHelper.h ^
    Common/xr_linear_algebra.h

SHIFT
GOTO LOOP

rem VULKAN
:VULKAN
tar -a -cf build\common_archs\Common_Vulkan.zip ^
    Common/GraphicsAPI.cpp ^
    Common/GraphicsAPI_Vulkan.cpp ^
    Common/OpenXRDebugUtils.cpp ^
    Common/DebugOutput.h ^
    Common/GraphicsAPI.h ^
    Common/GraphicsAPI_Vulkan.h ^
    Common/HelperFunctions.h ^
    Common/OpenXRDebugUtils.h ^
    Common/OpenXRHelper.h ^
    Common/xr_linear_algebra.h

SHIFT
GOTO LOOP

rem Full Folder
:END
tar -a -cf build\common_archs\Common.zip ^
    Common/GraphicsAPI.cpp ^
    Common/GraphicsAPI_D3D11.cpp ^
    Common/GraphicsAPI_D3D12.cpp ^
    Common/GraphicsAPI_OpenGL.cpp ^
    Common/GraphicsAPI_OpenGL_ES.cpp ^
    Common/GraphicsAPI_Vulkan.cpp ^
    Common/OpenXRDebugUtils.cpp ^
    Common/d3dx12.h ^
    Common/DebugOutput.h ^
    Common/GraphicsAPI.h ^
    Common/GraphicsAPI_D3D11.h ^
    Common/GraphicsAPI_D3D12.h ^
    Common/GraphicsAPI_OpenGL.h ^
    Common/GraphicsAPI_OpenGL_ES.h ^
    Common/GraphicsAPI_Vulkan.h ^
    Common/HelperFunctions.h ^
    Common/OpenXRDebugUtils.h ^
    Common/OpenXRHelper.h ^
    Common/xr_linear_algebra.h