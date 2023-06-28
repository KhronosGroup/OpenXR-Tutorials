cmake_minimum_required(VERSION 3.22.1)
project("openxrtutorialch2_1") # Needed for Android

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
    add_library(openxrtutorialch2_1 SHARED ${SOURCES})

    # native_app_glue
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()

    # vulkan - Found in the NDK
    find_library(vulkan-lib vulkan)
    target_include_directories(openxrtutorialch2_1 PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

    target_link_libraries(openxrtutorialch2_1
            android
            native_app_glue
            openxr_loader
            ${vulkan-lib})

else()
    add_executable(Chapter_2_1 ${SOURCES})
    target_link_libraries(Chapter_2_1 openxr_loader)

endif()
