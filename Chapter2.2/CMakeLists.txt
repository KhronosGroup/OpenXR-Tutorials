cmake_minimum_required(VERSION 3.22.1)
project("openxrtutorialch2_2") # Needed for Android

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_ALL_EXTENSIONS ON CACHE INTERNAL "Build loader and layers with all extensions")
set(BUILD_TESTS OFF CACHE INTERNAL "Build tests")
set(BUILD_CONFORMANCE_TESTS OFF CACHE INTERNAL "Build conformance tests")
set(BUILD_API_LAYERS ON CACHE INTERNAL "Use OpenXR layers")
FetchContent_Declare(
        OpenXR
        URL_HASH MD5=390455e9395a92d6b730afb8f23fac8e
        URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.27.zip
        SOURCE_DIR openxr)
FetchContent_MakeAvailable(OpenXR)

# Files
file(GLOB SOURCES "*.cpp" )
file(GLOB HEADERS "*.h" )

if (ANDROID)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate") # export ANativeActivity_onCreate for java to call.
    add_library(openxrtutorialch2_2 SHARED ${SOURCES} ${HEADERS})

    # native_app_glue
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()

    # vulkan - Found in the NDK
    find_library(vulkan-lib vulkan)
    target_include_directories(openxrtutorialch2_2 PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    # openxr-gfxwrapper - Manually build from OpenXR for OpenGL ES
    add_library(openxr-gfxwrapper STATIC ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.c ${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.h)
    target_include_directories(openxr-gfxwrapper PUBLIC ${openxr_SOURCE_DIR}/external/include)
    find_library(GLES3-lib GLESv3)
    find_library(EGL-lib EGL)
    target_link_libraries(openxr-gfxwrapper PUBLIC ${GLES3-lib})
    target_link_libraries(openxr-gfxwrapper PUBLIC ${EGL-lib})

    target_include_directories(openxrtutorialch2_2 PUBLIC ${openxr_SOURCE_DIR}/src/common)
    target_include_directories(openxrtutorialch2_2 PUBLIC ${openxr_SOURCE_DIR}/external/include)

    # Final link
    target_link_libraries(openxrtutorialch2_2
            android
            native_app_glue
            openxr_loader
            ${vulkan-lib}
            openxr-gfxwrapper)

else()
    add_executable(Chapter_2_2 ${SOURCES} ${HEADERS})
    target_link_libraries(Chapter_2_2 openxr_loader)

    if (WIN32)
        target_link_libraries(Chapter_2_2 "d3d11.lib") 
        target_link_libraries(Chapter_2_2 "d3d12.lib") 
        target_link_libraries(Chapter_2_2 "dxgi.lib") 
    endif()

    find_package(Vulkan REQUIRED)
    target_include_directories(Chapter_2_2 PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(Chapter_2_2 ${Vulkan_LIBRARIES})

    # Fix redirection to a different project's build folder
    target_include_directories(Chapter_2_2 PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../Chapter2.1/openxr/src/common)
    target_link_libraries(Chapter_2_2 openxr-gfxwrapper)

endif()
