# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)
project("openxrtutorialch2_1")

# native_app_glue
add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate") # export ANativeActivity_onCreate for java to call.
add_library(openxrtutorialch2_1 SHARED ../../../../../Chapter2.1/main.cpp)

# import openxr_loader
add_library(openxr_loader SHARED IMPORTED)
set_target_properties(openxr_loader PROPERTIES IMPORTED_LOCATION "../../../../../../thirdparty/openxr-sdk/android/libs/android.arm64-v8a/libopenxr_loader.so")
target_include_directories(openxrtutorialch2_1 PUBLIC ../../../../../thirdparty/openxr-sdk/include)

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

