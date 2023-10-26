# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22.1)
set(PROJECT_NAME OpenXRTutorialChapter6)
project("${PROJECT_NAME}") # Needed for Android

# Additional Directories for find_package() to search within.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include("../cmake/graphics_api_select.cmake")

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_TESTS
    OFF
    CACHE INTERNAL "Build tests"
)
set(BUILD_API_LAYERS
    ON
    CACHE INTERNAL "Use OpenXR layers"
)
FetchContent_Declare(
    OpenXR
    URL_HASH MD5=81930f0ccecdca852906e1a22aee4a45
    URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.28.zip
        SOURCE_DIR
        openxr
)
FetchContent_MakeAvailable(OpenXR)

# Files
set(SOURCES
    main.cpp
    ../Common/GraphicsAPI.cpp
    ../Common/GraphicsAPI_D3D11.cpp
    ../Common/GraphicsAPI_D3D12.cpp
    ../Common/GraphicsAPI_OpenGL.cpp
    ../Common/GraphicsAPI_OpenGL_ES.cpp
    ../Common/GraphicsAPI_Vulkan.cpp
    ../Common/OpenXRDebugUtils.cpp)
set(HEADERS
    ../Common/DebugOutput.h
    ../Common/GraphicsAPI.h
    ../Common/GraphicsAPI_D3D11.h
    ../Common/GraphicsAPI_D3D12.h
    ../Common/GraphicsAPI_OpenGL.h
    ../Common/GraphicsAPI_OpenGL_ES.h
    ../Common/GraphicsAPI_Vulkan.h
    ../Common/HelperFunctions.h
    ../Common/OpenXRDebugUtils.h
    ../Common/OpenXRHelper.h)

set(DX11_SHADERS
    "../ShadersMultiview/VertexShader_DX11_MV.hlsl"
    "../ShadersMultiview/PixelShader_DX11_MV.hlsl")
set(DX12_SHADERS
    "../ShadersMultiview/VertexShader_DX12_MV.hlsl"
    "../ShadersMultiview/PixelShader_DX12_MV.hlsl")
set(GL_SHADERS
    "../ShadersMultiview/VertexShader_GL_MV.glsl"
    "../ShadersMultiview/PixelShader_GL_MV.glsl")
set(VK_SHADERS
    "../ShadersMultiview/VertexShader_VK_MV.glsl"
    "../ShadersMultiview/PixelShader_VK_MV.glsl")
set(ES_GLSL_SHADERS
    "../ShadersMultiview/VertexShader_GLES_MV.glsl"
    "../ShadersMultiview/PixelShader_GLES_MV.glsl")

if (ANDROID) # Android
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
    target_include_directories(${PROJECT_NAME} PRIVATE 
        # In this repo
        ../Common/
        # From OpenXR repo
        "${openxr_SOURCE_DIR}/src/common"
        "${openxr_SOURCE_DIR}/external/include"
    )

    # export ANativeActivity_onCreate for java to call.
    set_property(
        TARGET ${PROJECT_NAME}
        APPEND_STRING
        PROPERTY LINK_FLAGS " -u ANativeActivity_onCreate"
    )

    # native_app_glue
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()

    target_link_libraries(${PROJECT_NAME} android native_app_glue openxr_loader)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-cast-calling-convention)
    AddGraphicsAPIDefine(${PROJECT_NAME})
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_ENABLE_MULTIVIEW)

    # VulkanNDK
    find_library(vulkan-lib vulkan)
    if (vulkan-lib)
        target_include_directories(${PROJECT_NAME} PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include)
        target_link_libraries(${PROJECT_NAME} ${vulkan-lib})
        target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_VULKAN)
    endif()
    
    include(../cmake/gfxwrapper.cmake)
    if(TARGET openxr-gfxwrapper)
        target_link_libraries(${PROJECT_NAME} openxr-gfxwrapper)
        target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_OPENGL_ES)
    endif()

    # Vulkan GLSL
    set(SHADER_DEST "${CMAKE_CURRENT_SOURCE_DIR}/app/src/main/assets/shaders")
    include(glsl_shader)
    set_source_files_properties(../ShadersMultiview/VertexShader_VK_MV.glsl PROPERTIES ShaderType "vert")
    set_source_files_properties(../ShadersMultiview/PixelShader_VK_MV.glsl PROPERTIES ShaderType "frag")

    foreach(FILE ${VK_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        glsl_spv_shader(
           INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
           OUTPUT "${SHADER_DEST}/${FILE_WE}.spv"
           STAGE ${shadertype}
           ENTRY_POINT main
           TARGET_ENV vulkan1.0
        )
        # Make our project depend on these files
        target_sources(${PROJECT_NAME} PRIVATE "${SHADER_DEST}/${FILE_WE}.spv")
    endforeach()

    #OpenGL ES GLSL
    set(SHADER_DEST "${CMAKE_CURRENT_SOURCE_DIR}/app/src/main/assets/shaders")
    foreach(FILE ${ES_GLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        add_custom_command(
            OUTPUT "${SHADER_DEST}/${FILE_WE}.glsl"
            COMMAND 
                ${CMAKE_COMMAND} -E copy 
                "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
                "${SHADER_DEST}/${FILE_WE}.glsl"
            COMMENT "GLSL ${FILE}"
            MAIN_DEPENDENCY "${FILE}"
            DEPEND "${FILE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            VERBATIM
        )
        # Make our project depend on these files
        target_sources(${PROJECT_NAME} PRIVATE "${SHADER_DEST}/${FILE_WE}.glsl")
    endforeach(FILE)

else() # Windows / Linux
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    if(NOT "${XR_RUNTIME_JSON}" STREQUAL "")
        set_target_properties( ${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "XR_RUNTIME_JSON=${XR_RUNTIME_JSON}")
    endif()

    target_include_directories(${PROJECT_NAME} PRIVATE 
        # In this repo
        ../Common/
        # From OpenXR repo
        "${openxr_SOURCE_DIR}/src/common"
        "${openxr_SOURCE_DIR}/external/include"
    )
    target_link_libraries(${PROJECT_NAME} openxr_loader)
    AddGraphicsAPIDefine(${PROJECT_NAME})
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_ENABLE_MULTIVIEW)

    if (WIN32) # Windows 
        # D3D11
        target_link_libraries(${PROJECT_NAME} "d3d11.lib")
        target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_D3D11)
        # D3D12
        target_link_libraries(${PROJECT_NAME} "d3d12.lib")
        target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_D3D12)
        # D3D11 / D3D12
        target_link_libraries(${PROJECT_NAME} "dxgi.lib")
    else() # Linux
        target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_LINUX_XLIB)
    endif()
    
    # VulkanSDK
    find_package(Vulkan)
    if (Vulkan_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
        target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_VULKAN)
    endif()
    
    # OpenGL
    include(../cmake/gfxwrapper.cmake)
    if(TARGET openxr-gfxwrapper)
        target_link_libraries(${PROJECT_NAME} openxr-gfxwrapper)
        target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_OPENGL)
    endif()

    # D3D11 and D3D12 HLSL
    if(WIN32)
        include(fxc_shader)
        set_property(SOURCE ${DX11_SHADERS} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")
        set_source_files_properties(../ShadersMultiview/VertexShader_DX11_MV.hlsl PROPERTIES ShaderType "vs")
        set_source_files_properties(../ShadersMultiview/PixelShader_DX11_MV.hlsl PROPERTIES ShaderType "ps")
        
        include(dxc_shader)
        set_property(SOURCE ${DX12_SHADERS} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")
        set_source_files_properties(../ShadersMultiview/VertexShader_DX12_MV.hlsl PROPERTIES ShaderType "vs")
        set_source_files_properties(../ShadersMultiview/PixelShader_DX12_MV.hlsl PROPERTIES ShaderType "ps")

        # D3D11: Using Shader Model 5.0
        set(shadermodel 5_0)
        foreach(FILE ${DX11_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
            fxc_shader(
                INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso"
                OUTPUT_PDB "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb"
                PROFILE ${shadertype}_${shadermodel}
                ENTRY_POINT main
            )
            # Make our project depend on these files
            target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso")
        endforeach()

        # D3D12: Using Shader Model 6.1 for SV_ViewID
        set(shadermodel 6_1)
        foreach(FILE ${DX12_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
            dxc_shader(
                INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso"
                OUTPUT_PDB "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb"
                PROFILE ${shadertype}_${shadermodel}
                ENTRY_POINT main
            )
            # Make our project depend on these files
            target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso")
        endforeach()
    endif()

    # Vulkan GLSL
    set(SHADER_DEST "${CMAKE_CURRENT_BINARY_DIR}")
    if (Vulkan_FOUND)
        include(glsl_shader)
        set_source_files_properties(../ShadersMultiview/VertexShader_VK_MV.glsl PROPERTIES ShaderType "vert")
        set_source_files_properties(../ShadersMultiview/PixelShader_VK_MV.glsl PROPERTIES ShaderType "frag")
    
        foreach(FILE ${VK_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
            glsl_spv_shader(
               INPUT "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
               OUTPUT "${SHADER_DEST}/${FILE_WE}.spv"
               STAGE ${shadertype}
               ENTRY_POINT main
               TARGET_ENV vulkan1.0
            )
            # Make our project depend on these files
            target_sources(${PROJECT_NAME} PRIVATE "${SHADER_DEST}/${FILE_WE}.spv")
        endforeach()
    endif()

    # OpenGL GLSL
    set(SHADER_DEST "${CMAKE_CURRENT_BINARY_DIR}")
    foreach(FILE ${GL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        add_custom_command(
            OUTPUT "${SHADER_DEST}/${FILE_WE}.glsl"
            COMMAND 
                ${CMAKE_COMMAND} -E copy 
                "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
                "${SHADER_DEST}/${FILE_WE}.glsl"
            COMMENT "GLSL ${FILE}"
            MAIN_DEPENDENCY "${FILE}"
            DEPEND "${FILE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            VERBATIM
        )
        # Make our project depend on these files
        target_sources(${PROJECT_NAME} PRIVATE "${SHADER_DEST}/${FILE_WE}.glsl")
    endforeach(FILE)
endif() # EOF
