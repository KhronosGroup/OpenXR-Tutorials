cmake_minimum_required(VERSION 3.15)

project(openxr-tutorial)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Optional selected runtime
set(XR_RUNTIME_JSON "$ENV{XR_RUNTIME_JSON}" CACHE PATH "Optional location of a specific OpenXR runtime configuration file.")
	
#add_subdirectory(tutorial)
add_subdirectory(Chapter2)
#add_subdirectory(Chapter3)
#add_subdirectory(Chapter4)
#add_subdirectory(GraphicsAPI_Test)