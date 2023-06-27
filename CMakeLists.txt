cmake_minimum_required(VERSION 3.15)

# For FetchContent_Declare()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
if(WIN32)
    FetchContent_Populate(
            OpenXR
            URL_HASH MD5=5ba6b56162ce6fcb2acd53069856d862
            URL https://github.com/KhronosGroup/OpenXR-SDK-Source/releases/download/release-1.0.27/openxr_loader_windows-1.0.27.zip
            SOURCE_DIR openxr)
elseif(UNIX)
    set(BUILD_ALL_EXTENSIONS ON CACHE INTERNAL "Build loader and layers with all extensions")
    set(BUILD_TESTS OFF CACHE INTERNAL "Build tests")
    set(BUILD_CONFORMANCE_TESTS OFF CACHE INTERNAL "Build conformance tests")
    set(BUILD_API_LAYERS ON CACHE INTERNAL "Use OpenXR layers")
    FetchContent_Declare(
            OpenXR
            URL_HASH MD5=390455e9395a92d6b730afb8f23fac8e
            URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.27.zip
            SOURCE_DIR openxr)
     FetchContent_MakeAvailable(OpenXR)
else()
    message( FATAL_ERROR "Unknown platform. Unable to fetch content for OpenXR." )
endif()

project(openxr-tutorial)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

add_subdirectory(tutorial)
add_subdirectory(Chapter2.1)
