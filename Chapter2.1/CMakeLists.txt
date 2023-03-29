cmake_minimum_required(VERSION 3.15)

file(GLOB SOURCES "*.cpp" )
file(GLOB HEADERS "*.h" )

cmake_policy(SET CMP0091 NEW)

set(CMAKE_MSVC_RUNTIME_LIBRARY
$<$<CONFIG:DEBUG>:"MultiThreadedDLL">
$<$<CONFIG:RELEASE>:"MultiThreaded">
)

add_executable(Chapter_2_1 ${SOURCES})
target_include_directories(Chapter_2_1 PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/openxr-sdk/include)
target_link_directories(Chapter_2_1 PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/openxr-sdk/lib)
target_link_libraries(Chapter_2_1 debug openxr_loaderd optimized openxr_loader) 
