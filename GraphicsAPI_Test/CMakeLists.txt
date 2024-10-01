# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22.1)

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_ALL_EXTENSIONS
    ON
    CACHE INTERNAL "Build loader and layers with all extensions"
)
set(BUILD_TESTS
    OFF
    CACHE INTERNAL "Build tests"
)
set(BUILD_CONFORMANCE_TESTS
    OFF
    CACHE INTERNAL "Build conformance tests"
)
set(BUILD_API_LAYERS
    ON
    CACHE INTERNAL "Use OpenXR layers"
)
FetchContent_Declare(
    OpenXR
    URL_HASH MD5=924a94a2da0b5ef8e82154c623d88644
    URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.34.zip
        DOWNLOAD_EXTRACT_TIMESTAMP
        TRUE
        SOURCE_DIR
        openxr
)
FetchContent_MakeAvailable(OpenXR)

# Files
set(SOURCES
    "main.cpp"
    "../Common/GraphicsAPI.cpp"
    "../Common/GraphicsAPI_D3D11.cpp"
    "../Common/GraphicsAPI_D3D12.cpp"
    "../Common/GraphicsAPI_OpenGL.cpp"
    "../Common/GraphicsAPI_OpenGL_ES.cpp"
    "../Common/GraphicsAPI_Vulkan.cpp"
)
set(HEADERS
    "../Common/DebugOutput.h"
    "../Common/GraphicsAPI.h"
    "../Common/GraphicsAPI_D3D11.h"
    "../Common/GraphicsAPI_D3D12.h"
    "../Common/GraphicsAPI_OpenGL.h"
    "../Common/GraphicsAPI_OpenGL_ES.h"
    "../Common/GraphicsAPI_Vulkan.h"
    "../Common/HelperFunctions.h"
)

set(PROJECT_NAME GraphicsAPI_Test)

set(HLSL_SHADERS "../Shaders/VertexShader.hlsl" "../Shaders/PixelShader.hlsl")
set(GLSL_SHADERS "../Shaders/VertexShader.glsl" "../Shaders/PixelShader.glsl")
set(ES_GLSL_SHADERS "../Shaders/VertexShader_GLES.glsl"
                    "../Shaders/PixelShader_GLES.glsl"
)

# Windows
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC "../Common/")
target_link_libraries(${PROJECT_NAME} openxr_loader)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} "d3d11.lib")
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_D3D11)
    target_link_libraries(${PROJECT_NAME} "d3d12.lib")
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_D3D12)
    target_link_libraries(${PROJECT_NAME} "dxgi.lib")
endif()

find_package(Vulkan)
if(Vulkan_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_VULKAN)
endif()

# Fix redirection to a different project's build folder
target_include_directories(
    ${PROJECT_NAME} PUBLIC ${openxr_SOURCE_DIR}/src/common
)
target_link_libraries(${PROJECT_NAME} openxr-gfxwrapper)
target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_OPENGL)

#Shaders
if(WIN32)
    # HLSL Shaders
    set_source_files_properties(
        ../Shaders/VertexShader.hlsl PROPERTIES ShaderType "vs"
    )
    set_source_files_properties(
        ../Shaders/PixelShader.hlsl PROPERTIES ShaderType "ps"
    )

    # D3D11
    set_source_files_properties(${HLSL_SHADERS} PROPERTIES ShaderModel "5_0")
    foreach(FILE ${HLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        get_source_file_property(shadermodel ${FILE} ShaderModel)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                fxc.exe /nologo /Emain /T${shadertype}_${shadermodel}
                $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo
                ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso /Fd
                ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb
                ${FILE} MAIN_DEPENDENCY ${FILE}
            COMMENT "HLSL ${FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )
    endforeach(FILE)
    # D3D12
    set_source_files_properties(${HLSL_SHADERS} PROPERTIES ShaderModel "5_1")
    foreach(FILE ${HLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        get_source_file_property(shadermodel ${FILE} ShaderModel)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                fxc.exe /nologo /Emain /T${shadertype}_${shadermodel}
                $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo
                ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso /Fd
                ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb
                ${FILE} MAIN_DEPENDENCY ${FILE}
            COMMENT "HLSL ${FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )
    endforeach(FILE)
endif()

if(Vulkan_FOUND)
    # GLSL Shaders
    set_source_files_properties(
        ../Shaders/VertexShader.glsl PROPERTIES ShaderType "vert"
    )
    set_source_files_properties(
        ../Shaders/PixelShader.glsl PROPERTIES ShaderType "frag"
    )

    #Vulkan
    foreach(FILE ${GLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                glslangValidator.exe -V ${FILE} -e main -S ${shadertype}
                $<IF:$<CONFIG:DEBUG>,-Od,> -g -o
                ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv MAIN_DEPENDENCY
                ${FILE}
            COMMENT "GLSL ${FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM COMMAND_EXPAND_LISTS
        )
    endforeach(FILE)
endif()

#OpenGL
foreach(FILE ${GLSL_SHADERS})
    get_filename_component(FILE_WE ${FILE} NAME_WE)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMAND
            ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
            ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.glsl MAIN_DEPENDENCY ${FILE}
        COMMENT "GLSL ${FILE}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
    )
endforeach(FILE)
