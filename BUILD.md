# Build

## Prerequisites
* [CMake 3.22.1](https://cmake.org/download/)
* [Python 3.6+](https://www.python.org/downloads/)

## Windows

Download [Visual Studio](https://visualstudio.microsoft.com/downloads/) 2017 or later. See additional [prerequisites](#Prerequisites).

Create a `build` folder in the `OpenXR-Tutorial` folder and from within that folder, configure and generate the project with CMake using following commands:

```
mkdir build
cd build
cmake -G "<your_visual_studio_version>" ../
```

You can also use the CMake GUI. 

Open `openxr-tutorial.sln` and build the `ALL_BUILD` project. Right click on an `OpenXRTutorialChapter` project and select "`Set as Startup Project`", hit `F5` to run and debug.

## Linux

Download and install your IDE of your choice. We have used [Visual Studio Code](https://code.visualstudio.com/download) in the tutorial.

See additional [prerequisites](#Prerequisites).

Create a `build` folder in the `OpenXR-Tutorial` folder and from within that folder, configure and generate the project with CMake using following commands:

```
mkdir build
cd build
cmake -G "<your_generator>" ../
```

You can also use the CMake GUI. 

Open the `openxr-tutorial` solution file and build the `ALL_BUILD` project. Select an `OpenXRTutorialChapter` project to run and debug.

## Android

Download [Android Studio](https://developer.android.com/studio) 2022.1.1 or later. 

Android Prequisites:
* SDK: 29
* NDK: 23.1.7779620
* Android Studio/Gradle Plugin Version: 7.4.2

See additional [prerequisites](#Prerequisites).

Open Android Studio and open one of the `Chapter` folders from within the reposity. Click the green hammer icon to build the project. Connect your Android device and set up any USB debugging and `adb` requirements. Click the plain green bug icon to run and debug.
