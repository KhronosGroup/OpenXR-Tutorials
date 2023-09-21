cmake_minimum_required(VERSION 3.16)
project(GraphicsAPI_Test)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

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
    # URL_HASH MD5=390455e9395a92d6b730afb8f23fac8e
    # URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.27.zip
    URL_HASH MD5=81930f0ccecdca852906e1a22aee4a45
    URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.28.zip
        DOWNLOAD_EXTRACT_TIMESTAMP
        TRUE
        SOURCE_DIR
        openxr
)
FetchContent_MakeAvailable(OpenXR)

# Files
set(SOURCES
    main.cpp
    ../Common/DebugOutput.h
    ../Common/GraphicsAPI.cpp
    ../Common/GraphicsAPI.h
    ../Common/GraphicsAPI_D3D11.cpp
    ../Common/GraphicsAPI_D3D11.h
    ../Common/GraphicsAPI_D3D12.cpp
    ../Common/GraphicsAPI_D3D12.h
    ../Common/GraphicsAPI_OpenGL.cpp
    ../Common/GraphicsAPI_OpenGL.h
    ../Common/GraphicsAPI_OpenGL_ES.cpp
    ../Common/GraphicsAPI_OpenGL_ES.h
    ../Common/GraphicsAPI_Vulkan.cpp
    ../Common/GraphicsAPI_Vulkan.h
    ../Common/HelperFunctions.h
    ../Common/OpenXRDebugUtils.cpp
    ../Common/OpenXRDebugUtils.h
    ../Common/OpenXRHelper.h
)

set(HLSL_SHADERS "../Shaders/VertexShader.hlsl" "../Shaders/PixelShader.hlsl")
set(GLSL_SHADERS "../Shaders/VertexShader.glsl" "../Shaders/PixelShader.glsl")
set(ES_GLSL_SHADERS "../Shaders/VertexShader_GLES.glsl"
                    "../Shaders/PixelShader_GLES.glsl"
)

find_package(Vulkan)

# A nested build of a helper library found in the OpenXR source
include(../cmake/gfxwrapper.cmake)

if(ANDROID)
    # native_app_glue
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()

    # On Android the native code must be a shared library instead of an
    # executable
    add_library(${PROJECT_NAME} MODULE ${SOURCES})

    # export ANativeActivity_onCreate for java to call.
    set_property(
        TARGET ${PROJECT_NAME}
        APPEND_STRING
        PROPERTY LINK_FLAGS " -u ANativeActivity_onCreate"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE android native_app_glue)

else()
    # Windows / Linux
    set(SHADERS ${GLSL_SHADERS})
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SHADERS})

    if(NOT "${XR_RUNTIME_JSON}" STREQUAL "")
        set_target_properties(
            ${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT
                                       "XR_RUNTIME_JSON=${XR_RUNTIME_JSON}"
        )
    endif()
endif()

if(WIN32)

    # HLSL Shaders
    set_source_files_properties(
        ../Shaders/VertexShader.hlsl PROPERTIES ShaderType "vs"
    )
    set_source_files_properties(
        ../Shaders/PixelShader.hlsl PROPERTIES ShaderType "ps"
    )

    # D3D11: using shader model 5.0
    set(shadermodel 5_0)
    foreach(FILE ${HLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        # TODO this should be a custom command that creates a source file linked
        # into the executable, the current command is not quite valid
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                fxc.exe /nologo /Emain /T${shadertype}_${shadermodel}
                $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo
                "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso" /Fd
                "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb"
                ${FILE}
            COMMENT "HLSL ${FILE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            VERBATIM
        )
    endforeach()
    # D3D12: using shader model 5.1
    set(shadermodel "5_1")
    foreach(FILE ${HLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        # TODO this should be a custom command that creates a source file linked
        # into the executable, the current command is not quite valid
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                fxc.exe /nologo /Emain /T${shadertype}_${shadermodel}
                $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo
                "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.cso" /Fd
                "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}_${shadermodel}.pdb"
                ${FILE}
            COMMENT "HLSL ${FILE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            VERBATIM
        )
    endforeach()
endif()

# TODO these should not need to be separate between android and not! If they had to be this separate it would be easier to have totally different build systems
if(NOT ANDROID)
    if(Vulkan_FOUND)
        # GLSL Shaders
        set_source_files_properties(
            ../Shaders/VertexShader.glsl PROPERTIES ShaderType vert
        )
        set_source_files_properties(
            ../Shaders/PixelShader.glsl PROPERTIES ShaderType frag
        )

        # Vulkan
        foreach(FILE ${GLSL_SHADERS})
            get_filename_component(FILE_WE ${FILE} NAME_WE)
            get_source_file_property(shadertype ${FILE} ShaderType)
            message(
                "${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${FILE} -e main -S ${shadertype} $<$<CONFIG:Debug>:-Od> -g -o ${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv"
            )
            # TODO this should be a custom command that creates a source file linked
            # into the executable, the current command is not quite valid
            add_custom_command(
                TARGET ${PROJECT_NAME}
                COMMAND
                    "${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}" -V ${FILE} -e main
                    -S ${shadertype} -Od -g -o
                    "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv"
                COMMENT "GLSL ${FILE}"
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                VERBATIM
            )
        endforeach()
    endif()
    # OpenGL
    foreach(FILE ${GLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.glsl"
            COMMENT "GLSL ${FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )
    endforeach()
else()
    # GLSL Shaders
    set_source_files_properties(
        ../Shaders/VertexShader.glsl PROPERTIES ShaderType "vertex"
    )
    set_source_files_properties(
        ../Shaders/PixelShader.glsl PROPERTIES ShaderType "fragment"
    )

    #Vulkan
    file(GLOB glslc-folders ${ANDROID_NDK}/shader-tools/*)
    find_program(GLSLC glslc HINTS ${glslc-folders})
    foreach(FILE ${GLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_source_file_property(shadertype ${FILE} ShaderType)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                ${GLSLC} -fentry-point=main -fshader-stage=${shadertype} -g -o
                ${CMAKE_CURRENT_SOURCE_DIR}/app/src/main/assets/shaders/${FILE_WE}.spv
                ${FILE} MAIN_DEPENDENCY ${FILE}
            COMMENT "GLSL ${FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )
    endforeach()
    # OpenGL ES
    foreach(FILE ${ES_GLSL_SHADERS})
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMAND
                ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
                ${CMAKE_CURRENT_SOURCE_DIR}/app/src/main/assets/shaders/${FILE_WE}.glsl
                MAIN_DEPENDENCY ${FILE}
            COMMENT "GLSL ${FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )
    endforeach(FILE)
endif()
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        # In this repo
        ../Common
        ../third_party
        # From OpenXR repo
        "${openxr_SOURCE_DIR}/src/common"
        "${openxr_SOURCE_DIR}/external/include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenXR::openxr_loader)

if(TARGET openxr-gfxwrapper)
    target_link_libraries(${PROJECT_NAME} PRIVATE openxr-gfxwrapper)
    if(ANDROID)

        target_compile_definitions(
            ${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_OPENGL_ES
        )
    else()
        target_compile_definitions(
            ${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_OPENGL
        )

    endif()
    if(NOT WIN32 AND NOT ANDROID)
        # Linux
        target_compile_definitions(
            ${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_LINUX_XLIB
        )
    endif()
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE d3d11 d3d12 dxgi)
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_D3D11)
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_D3D12)
endif()

if(Vulkan_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
    target_compile_definitions(${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_VULKAN)
endif()

# Fix redirection to a different project's build folder TODO why is this needed?
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../Chapter4/openxr/src/common
)
