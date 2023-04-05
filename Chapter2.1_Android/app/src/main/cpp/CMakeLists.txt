# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("openxrtutorialch2_1")

add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library( # Sets the name of the library.
        openxrtutorialch2_1

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ../../../../../Chapter2.1/main.cpp)

target_include_directories(openxrtutorialch2_1 PUBLIC ../../../../../thirdparty/openxr-sdk/include)
target_link_directories(openxrtutorialch2_1 PUBLIC ../../../../../thirdparty/openxr-sdk/android/libs/android.arm64-v8a)
#target_link_directories(openxrtutorialch2_1 PUBLIC ../../../../../thirdparty/openxr-sdk/android/libs/android.armeabi-v7a)
#target_link_directories(openxrtutorialch2_1 PUBLIC ../../../../../thirdparty/openxr-sdk/android/libs/android.x86)
#target_link_directories(openxrtutorialch2_1 PUBLIC ../../../../../thirdparty/openxr-sdk/android/libs/android.x86_64)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        openxrtutorialch2_1
        android
        native_app_glue
        openxr_loader
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})