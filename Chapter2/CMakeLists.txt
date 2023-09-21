cmake_minimum_required(VERSION 3.16)
project(OpenXRTutorialChapter2)

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
    URL_HASH MD5=81930f0ccecdca852906e1a22aee4a45
    URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.28.zip
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
    add_executable(${PROJECT_NAME} ${SOURCES})

    if(NOT "${XR_RUNTIME_JSON}" STREQUAL "")
        set_target_properties(
            ${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT
                                       "XR_RUNTIME_JSON=${XR_RUNTIME_JSON}"
        )
    endif()
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
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../Chapter2/openxr/src/common
)
