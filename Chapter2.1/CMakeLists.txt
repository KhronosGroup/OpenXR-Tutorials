cmake_minimum_required(VERSION 3.15)

file(GLOB SOURCES "*.cpp" )
file(GLOB HEADERS "*.h" )

add_executable(Chapter_2_1 ${SOURCES})
target_include_directories(Chapter_2_1 PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/openxr-sdk/include)
target_link_directories(Chapter_2_1 PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/openxr-sdk/x64/lib)
target_link_libraries(Chapter_2_1 openxr_loader) 
