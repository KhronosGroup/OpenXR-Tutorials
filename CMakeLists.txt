cmake_minimum_required(VERSION 3.16)

project(openxr-tutorial)

option(BUILD_TUTORIAL "Build the tutorial docs?" OFF)

# Optional override runtime
set(XR_RUNTIME_JSON
    "$ENV{XR_RUNTIME_JSON}"
    CACHE PATH
          "Optional location of a specific OpenXR runtime configuration file."
)

if(BUILD_TUTORIAL)
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
