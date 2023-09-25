cmake_minimum_required(VERSION 3.16)

project(openxr-tutorial)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

option(TUTORIAL_BUILD_TUTORIAL "Build the tutorial docs?" ON)

# Optional override runtime
set(XR_RUNTIME_JSON
    "$ENV{XR_RUNTIME_JSON}"
    CACHE PATH
          "Optional location of a specific OpenXR runtime configuration file."
)

if(TUTORIAL_BUILD_TUTORIAL)
    add_subdirectory(tutorial)
endif()
add_subdirectory(Chapter2)
add_subdirectory(Chapter3)
add_subdirectory(Chapter4)
add_subdirectory(Chapter5)
if(WIN32)
    # Windows only
    add_subdirectory(GraphicsAPI_Test)
endif()
