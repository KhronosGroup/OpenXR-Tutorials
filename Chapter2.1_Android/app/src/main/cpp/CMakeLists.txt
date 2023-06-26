# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)
project("openxrtutorialch2_1")

include(FetchContent) # For FetchContent_Declare()

# native_app_glue
add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate") # export ANativeActivity_onCreate for java to call.
add_library(openxrtutorialch2_1 SHARED ../../../../../Chapter2.1/main.cpp)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_ALL_EXTENSIONS ON CACHE INTERNAL "Build loader and layers with all extensions")
set(BUILD_TESTS OFF CACHE INTERNAL "Build tests")
set(BUILD_CONFORMANCE_TESTS OFF CACHE INTERNAL "Build conformance tests")
set(BUILD_API_LAYERS ON CACHE INTERNAL "Use OpenXR layers")
FetchContent_Declare(
        OpenXR
        URL_HASH MD5=390455e9395a92d6b730afb8f23fac8e
        URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.27.zip)
FetchContent_MakeAvailable(OpenXR)

# vulkan - Found in the NDK
find_library(vulkan-lib vulkan)
target_include_directories(openxrtutorialch2_1 PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

# log - Found in the NDK
find_library(log-lib log)

target_link_libraries(openxrtutorialch2_1
        android
        native_app_glue
        openxr_loader
        ${vulkan-lib}
        ${log-lib})

