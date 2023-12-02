# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

# XR_DOCS_TAG_BEGIN_CMakeLists
cmake_minimum_required(VERSION 3.22.1)

project(openxr-tutorial)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Optional override runtime
set(XR_RUNTIME_JSON
    "$ENV{XR_RUNTIME_JSON}"
    CACHE PATH
    "Optional location of a specific OpenXR runtime configuration file."
)
# XR_DOCS_TAG_END_CMakeLists

option(XR_TUTORIAL_BUILD_DOCUMENTATION "Build the tutorial documentation?" OFF)
option(XR_TUTORIAL_BUILD_PROJECTS "Build the tutorial projects?" ON)

if (XR_TUTORIAL_BUILD_DOCUMENTATION)
    add_subdirectory(tutorial)

    #Build end of chapter archives
    if (WIN32)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/build-eoc-archs.bat" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    elseif(LINUX)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/build-eoc-archs.sh" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    #Build Common folder archives
    if (WIN32)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/build-common-archs.bat" "D3D11" "D3D12" "OPENGL" "OPENGL_ES" "VULKAN" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    elseif(LINUX)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/build-common-archs.sh" "D3D11" "D3D12" "OPENGL" "OPENGL_ES" "VULKAN" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    #Build Common folder archives - Chapter 6
    if (WIN32)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/Chapter6_1_Multiview/build-common-archs.bat" "D3D11" "D3D12" "OPENGL" "OPENGL_ES" "VULKAN" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Chapter6_1_Multiview/")
    elseif(LINUX)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/Chapter6_1_Multiview/build-common-archs.sh" "D3D11" "D3D12" "OPENGL" "OPENGL_ES" "VULKAN" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Chapter6_1_Multiview/")
    endif()

    #Build Android Build Folder Archives
    if (WIN32)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/build-android-arch.bat" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    elseif(LINUX)
        execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/build-android-arch.sh" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
endif()

if (XR_TUTORIAL_BUILD_PROJECTS)
    # XR_DOCS_TAG_BEGIN_AddChapter1
    add_subdirectory(Chapter1)
    # XR_DOCS_TAG_END_AddChapter1

    # XR_DOCS_TAG_BEGIN_AddChapter2
    add_subdirectory(Chapter2)
    # XR_DOCS_TAG_END_AddChapter2

    # XR_DOCS_TAG_BEGIN_AddChapter3
    add_subdirectory(Chapter3)
    # XR_DOCS_TAG_END_AddChapter3

    # XR_DOCS_TAG_BEGIN_AddChapter4
    add_subdirectory(Chapter4)
    # XR_DOCS_TAG_END_AddChapter4

    # XR_DOCS_TAG_BEGIN_AddChapter5
    add_subdirectory(Chapter5)
    # XR_DOCS_TAG_END_AddChapter5
endif()

if (WIN32) # Windows only
    add_subdirectory(GraphicsAPI_Test)
endif()

# Check license information
add_subdirectory(reuse)