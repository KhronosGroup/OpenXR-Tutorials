cmake_minimum_required(VERSION 3.22.1)
project("OpenXRTutorialChapter4") # Needed for Android

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_ALL_EXTENSIONS ON CACHE INTERNAL "Build loader and layers with all extensions")
set(BUILD_TESTS OFF CACHE INTERNAL "Build tests")
set(BUILD_CONFORMANCE_TESTS OFF CACHE INTERNAL "Build conformance tests")
set(BUILD_API_LAYERS ON CACHE INTERNAL "Use OpenXR layers")

set( OPENXR_GRAPHICS_API_TYPE "VULKAN" CACHE STRING "Which graphics API to use" )

FetchContent_Declare(
        OpenXR
        # URL_HASH MD5=390455e9395a92d6b730afb8f23fac8e
        # URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.27.zip
        URL_HASH MD5=81930f0ccecdca852906e1a22aee4a45
        URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.28.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SOURCE_DIR openxr)
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
    "../Common/OpenXRDebugUtils.cpp")
set(HEADERS 
    "../Common/DebugOutput.h"
    "../Common/GraphicsAPI.h"
    "../Common/GraphicsAPI_D3D11.h"
    "../Common/GraphicsAPI_D3D12.h"
    "../Common/GraphicsAPI_OpenGL.h"
    "../Common/GraphicsAPI_OpenGL_ES.h"
    "../Common/GraphicsAPI_Vulkan.h"
    "../Common/HelperFunctions.h"
    "../Common/OpenXRDebugUtils.h"
    "../Common/OpenXRHelper.h"
    )

set(HLSL_SHADERS
    "../Shaders/VertexShader.hlsl"
    "../Shaders/PixelShader.hlsl")
set(GLSL_SHADERS
    "../Shaders/VertexShader.glsl"
    "../Shaders/PixelShader.glsl")
set(ES_GLSL_SHADERS
    "../Shaders/VertexShader_GLES.glsl"
    "../Shaders/PixelShader_GLES.glsl")



if (ANDROID)
    set(SHADERS ${GLSL_SHADERS} ${ES_GLSL_SHADERS} )
	SET_PROPERTY(CACHE OPENXR_GRAPHICS_API_TYPE PROPERTY STRINGS "OPENG_ES VULKAN")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate") # export ANativeActivity_onCreate for java to call.
    add_library(OpenXRTutorialChapter4 SHARED ${SOURCES} ${HEADERS})
    target_include_directories(OpenXRTutorialChapter4 PUBLIC "../Common/")

    # native_app_glue
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()

    # vulkan - Found in the NDK
    find_library(vulkan-lib vulkan)
    target_include_directories(OpenXRTutorialChapter4 PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    # openxr-gfxwrapper - Manually build from OpenXR for OpenGL ES
    add_library(openxr-gfxwrapper STATIC ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.c ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.h)
    target_include_directories(openxr-gfxwrapper PUBLIC ${openxr_SOURCE_DIR}/external/include)
    find_library(GLES3-lib GLESv3)
    find_library(EGL-lib EGL)
    target_link_libraries(openxr-gfxwrapper PUBLIC ${GLES3-lib})
    target_link_libraries(openxr-gfxwrapper PUBLIC ${EGL-lib})

    target_include_directories(OpenXRTutorialChapter4 PUBLIC ${openxr_SOURCE_DIR}/src/common)
    target_include_directories(OpenXRTutorialChapter4 PUBLIC ${openxr_SOURCE_DIR}/external/include)
    target_include_directories(OpenXRTutorialChapter4 PUBLIC ../third_party)

    # Final link
    target_link_libraries(OpenXRTutorialChapter4
            android
            native_app_glue
            openxr_loader
            ${vulkan-lib}
            openxr-gfxwrapper)
    target_compile_options(OpenXRTutorialChapter4 PRIVATE -Wno-cast-calling-convention)

    #Shaders
    find_package(Vulkan)
    #if (Vulkan_FOUND) #TODO: Use NDK, not Vulkan SDK.
        # GLSL Shaders
        set_source_files_properties(../Shaders/VertexShader.glsl PROPERTIES ShaderType "vert")
        set_source_files_properties(../Shaders/PixelShader.glsl PROPERTIES ShaderType "frag")
    
        #Vulkan
        foreach(FILE ${GLSL_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
            add_custom_command(TARGET OpenXRTutorialChapter4
                               COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${FILE} -e main -S ${shadertype} -g -o ${CMAKE_CURRENT_SOURCE_DIR}/app/src/main/assets/shaders/${FILE_WE}.spv
                               MAIN_DEPENDENCY ${FILE}
                               COMMENT "GLSL ${FILE}"
                               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                               VERBATIM)
        endforeach(FILE)
    #endif()

    #OpenGL ES
    foreach(FILE ${ES_GLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        add_custom_command(TARGET OpenXRTutorialChapter4 
                           COMMAND ${CMAKE_COMMAND} -E copy 
                           ${CMAKE_CURRENT_SOURCE_DIR}/${FILE} 
                           ${CMAKE_CURRENT_SOURCE_DIR}/app/src/main/assets/shaders/${FILE_WE}.glsl
                           MAIN_DEPENDENCY ${FILE}
                           COMMENT "GLSL ${FILE}"
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           VERBATIM)
    endforeach(FILE)

else()
	set(SHADERS ${GLSL_SHADERS})
    add_executable(OpenXRTutorialChapter4 ${SOURCES} ${HEADERS} ${SHADERS})

    if(NOT "${XR_RUNTIME_JSON}" STREQUAL "")
        set_target_properties( OpenXRTutorialChapter4 PROPERTIES VS_DEBUGGER_ENVIRONMENT "XR_RUNTIME_JSON=${XR_RUNTIME_JSON}")
    endif()
    target_include_directories(OpenXRTutorialChapter4 PUBLIC "../Common/")
    target_link_libraries(OpenXRTutorialChapter4 openxr_loader)

    if (WIN32)
		SET_PROPERTY(CACHE OPENXR_GRAPHICS_API_TYPE PROPERTY STRINGS VULKAN OPENGL D3D11 D3D12 ) 
        target_link_libraries(OpenXRTutorialChapter4 "d3d11.lib") 
        target_link_libraries(OpenXRTutorialChapter4 "d3d12.lib") 
        target_link_libraries(OpenXRTutorialChapter4 "dxgi.lib") 
		target_compile_definitions(OpenXRTutorialChapter4 PUBLIC NOMINMAX OPENXR_GRAPHICS_API_TYPE=${OPENXR_GRAPHICS_API_TYPE})
	else()
		SET_PROPERTY(CACHE OPENXR_GRAPHICS_API_TYPE PROPERTY STRINGS "VULKAN OPENGL" ) 
    endif()

    find_package(Vulkan)
    if (Vulkan_FOUND)
        target_include_directories(OpenXRTutorialChapter4 PUBLIC ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(OpenXRTutorialChapter4 ${Vulkan_LIBRARIES})
    endif()

    # Fix redirection to a different project's build folder
    target_include_directories(OpenXRTutorialChapter4 PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../Chapter2/openxr/src/common)
    target_link_libraries(OpenXRTutorialChapter4 openxr-gfxwrapper)

    set_source_files_properties(../Shaders/VertexShader.hlsl PROPERTIES ShaderType "vs")
    set_source_files_properties(../Shaders/PixelShader.hlsl PROPERTIES ShaderType "ps")
    set_source_files_properties(${HLSL_SHADERS} PROPERTIES ShaderModel "4_0")

    #Shaders
    if (WIN32)
        # HLSL Shaders
        set_source_files_properties(../Shaders/VertexShader.hlsl PROPERTIES ShaderType "vs")
        set_source_files_properties(../Shaders/PixelShader.hlsl PROPERTIES ShaderType "ps")
        
        # D3D11
        set_source_files_properties(${HLSL_SHADERS} PROPERTIES ShaderModel "5_0")
        foreach(FILE ${HLSL_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
            get_source_file_property(shadermodel ${FILE} ShaderModel)
            add_custom_command(TARGET OpenXRTutorialChapter4
                               COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso /Fd ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb ${FILE}
                               MAIN_DEPENDENCY ${FILE}
                               COMMENT "HLSL ${FILE}"
                               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                               VERBATIM)
        endforeach(FILE)
        # D3D12
        set_source_files_properties(${HLSL_SHADERS} PROPERTIES ShaderModel "5_1")
        foreach(FILE ${HLSL_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
            get_source_file_property(shadermodel ${FILE} ShaderModel)
            add_custom_command(TARGET OpenXRTutorialChapter4
                               COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso /Fd ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb ${FILE}
                               MAIN_DEPENDENCY ${FILE}
                               COMMENT "HLSL ${FILE}"
                               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                               VERBATIM)
        endforeach(FILE)
    endif()

    if (Vulkan_FOUND)
        # GLSL Shaders
        set_source_files_properties(../Shaders/VertexShader.glsl PROPERTIES ShaderType vert)
        set_source_files_properties(../Shaders/PixelShader.glsl PROPERTIES ShaderType frag)
    
        #Vulkan
        foreach(FILE ${GLSL_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
			message ("${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${FILE} -e main -S ${shadertype} $<$<CONFIG:Debug>:-Od> -g -o ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv")
            add_custom_command(TARGET OpenXRTutorialChapter4
                               COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${FILE} -e main -S ${shadertype} -Od -g -o ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv
                               MAIN_DEPENDENCY ${FILE}
                               COMMENT "GLSL ${FILE}"
                               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                               VERBATIM)
        endforeach(FILE)
    endif()
    
    #OpenGL
    foreach(FILE ${GLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        add_custom_command(TARGET OpenXRTutorialChapter4 
                           COMMAND ${CMAKE_COMMAND} -E copy 
                           ${CMAKE_CURRENT_SOURCE_DIR}/${FILE} 
                           ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.glsl
                           MAIN_DEPENDENCY ${FILE}
                           COMMENT "GLSL ${FILE}"
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           VERBATIM)
    endforeach(FILE)
endif()
