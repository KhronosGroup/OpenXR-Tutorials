# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28.3)

project(openxr-tutorial)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Optional override runtime
set(XR_RUNTIME_JSON
    "$ENV{XR_RUNTIME_JSON}"
    CACHE PATH
    "Optional location of a specific OpenXR runtime configuration file."
)

add_subdirectory(Chapter5)
