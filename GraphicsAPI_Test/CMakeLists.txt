cmake_minimum_required(VERSION 3.22.1)

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_ALL_EXTENSIONS ON CACHE INTERNAL "Build loader and layers with all extensions")
set(BUILD_TESTS OFF CACHE INTERNAL "Build tests")
set(BUILD_CONFORMANCE_TESTS OFF CACHE INTERNAL "Build conformance tests")
set(BUILD_API_LAYERS ON CACHE INTERNAL "Use OpenXR layers")
FetchContent_Declare(
        OpenXR
        # URL_HASH MD5=390455e9395a92d6b730afb8f23fac8e
        # URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.27.zip
        URL_HASH MD5=81930f0ccecdca852906e1a22aee4a45
        URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.28.zip
        SOURCE_DIR openxr)
FetchContent_MakeAvailable(OpenXR)

# Files
set(SOURCES 
    "main.cpp"
    "../Common/GraphicsAPI.cpp"
    "../Common/GraphicsAPI_D3D11.cpp"
    "../Common/GraphicsAPI_D3D12.cpp"
    "../Common/GraphicsAPI_OpenGL.cpp"
    "../Common/GraphicsAPI_OpenGL_ES.cpp"
    "../Common/GraphicsAPI_Vulkan.cpp")
set(HEADERS 
    "../Common/DebugOutput.h"
    "../Common/GraphicsAPI.h"
    "../Common/GraphicsAPI_D3D11.h"
    "../Common/GraphicsAPI_D3D12.h"
    "../Common/GraphicsAPI_OpenGL.h"
    "../Common/GraphicsAPI_OpenGL_ES.h"
    "../Common/GraphicsAPI_Vulkan.h"
    "../Common/HelperFunctions.h")

 # Windows
add_executable(GraphicsAPI_Test ${SOURCES} ${HEADERS})
target_include_directories(GraphicsAPI_Test PUBLIC "../Common/")
target_link_libraries(GraphicsAPI_Test openxr_loader)

if (WIN32)
    target_link_libraries(GraphicsAPI_Test "d3d11.lib") 
    target_link_libraries(GraphicsAPI_Test "d3d12.lib") 
    target_link_libraries(GraphicsAPI_Test "dxgi.lib") 
    target_link_libraries(GraphicsAPI_Test "d3dcompiler.lib") 
endif()

find_package(Vulkan)
if (Vulkan_FOUND)
    target_include_directories(GraphicsAPI_Test PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(GraphicsAPI_Test ${Vulkan_LIBRARIES})
endif()

# Fix redirection to a different project's build folder
target_include_directories(GraphicsAPI_Test PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../Chapter2/openxr/src/common)
target_link_libraries(GraphicsAPI_Test openxr-gfxwrapper)
