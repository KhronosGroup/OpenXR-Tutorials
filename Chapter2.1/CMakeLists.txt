cmake_minimum_required(VERSION 3.15)

file(GLOB SOURCES "*.cpp" )
file(GLOB HEADERS "*.h" )

add_executable(Chapter_2_1 ${SOURCES})
target_include_directories(Chapter_2_1 PUBLIC ${CMAKE_BINARY_DIR}/openxr/include)
target_link_directories(Chapter_2_1 PUBLIC ${CMAKE_BINARY_DIR}/openxr/x64/lib)
target_link_libraries(Chapter_2_1 openxr_loader) 

add_custom_command(TARGET Chapter_2_1 POST_BUILD                 # Adds a post-build event to the project
    COMMAND ${CMAKE_COMMAND} -E copy_if_different                # which executes "cmake - E copy_if_different..."
        "${CMAKE_BINARY_DIR}/openxr/x64/bin/openxr_loader.dll"   # <--this is in-file
        $<TARGET_FILE_DIR:Chapter_2_1>)                          # <--this is out-file path