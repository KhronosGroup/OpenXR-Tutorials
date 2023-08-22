cmake_minimum_required(VERSION 3.22.1)
project("OpenXRTutorialChapter3") # Needed for Android

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_ALL_EXTENSIONS ON CACHE INTERNAL "Build loader and layers with all extensions")
set(BUILD_TESTS OFF CACHE INTERNAL "Build tests")
set(BUILD_CONFORMANCE_TESTS OFF CACHE INTERNAL "Build conformance tests")
set(BUILD_API_LAYERS ON CACHE INTERNAL "Use OpenXR layers")
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
    "../Common/OpenXRHelper.h")

set(PROJECT_NAME OpenXRTutorialChapter3)

if (ANDROID) # Android
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate") # export ANativeActivity_onCreate for java to call.
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
    target_include_directories(${PROJECT_NAME} PUBLIC "../Common/")

    # native_app_glue
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()

    # VulkanNDK
    find_library(vulkan-lib vulkan)
    target_include_directories(${PROJECT_NAME} PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    # openxr-gfxwrapper - Manually build from OpenXR for OpenGL ES
    add_library(openxr-gfxwrapper STATIC ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.c ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.h)
    target_include_directories(openxr-gfxwrapper PUBLIC ${openxr_SOURCE_DIR}/external/include)
    find_library(GLES3-lib GLESv3)
    find_library(EGL-lib EGL)
    target_link_libraries(openxr-gfxwrapper PUBLIC ${GLES3-lib})
    target_link_libraries(openxr-gfxwrapper PUBLIC ${EGL-lib})

    target_include_directories(${PROJECT_NAME} PUBLIC ${openxr_SOURCE_DIR}/src/common)
    target_include_directories(${PROJECT_NAME} PUBLIC ${openxr_SOURCE_DIR}/external/include)

    # Final link
    target_link_libraries(${PROJECT_NAME}
            android
            native_app_glue
            openxr_loader
            ${vulkan-lib}
            openxr-gfxwrapper)

else() # Windows / Linux
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    if(NOT "${XR_RUNTIME_JSON}" STREQUAL "")
        set_target_properties( ${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "XR_RUNTIME_JSON=${XR_RUNTIME_JSON}")
    endif()
    target_include_directories(${PROJECT_NAME} PUBLIC "../Common/")
    target_link_libraries(${PROJECT_NAME} openxr_loader)

    if (WIN32) # Windows 
        # D3D11
        target_link_libraries(${PROJECT_NAME} "d3d11.lib") 
        # D3D12
        target_link_libraries(${PROJECT_NAME} "d3d12.lib") 
        # D3D11 / D3D12
        target_link_libraries(${PROJECT_NAME} "dxgi.lib") 
    else() # Linux
        target_compile_definitions(${PROJECT_NAME} PUBLIC OS_LINUX_WAYLAND=1 XR_USE_PLATFORM_WAYLAND=1 XR_USE_GRAPHICS_API_OPENGL=1 LINUX=1)#XR_USE_GRAPHICS_API_VULKAN=1 
    endif()

    # VulkanSDK
    find_package(Vulkan)
    if (Vulkan_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
    endif()

    # OpenGL
    # Fix redirection to a different project's build folder
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../Chapter2/openxr/src/common)
    target_link_libraries(${PROJECT_NAME} openxr-gfxwrapper)
endif() # EOF