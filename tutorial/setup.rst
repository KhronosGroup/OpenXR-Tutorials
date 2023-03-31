#####
Setup
#####

.. rubric:: Windows
For your Windows OpenXR project, we'll use CMake to create some Windows project files.
Create a directory where your code will go, and create a text file called CMakeLists.txt.

In it, put the following:

.. highlight:: cmake
.. code-block:: cmake

	cmake_minimum_required(VERSION 3.15)
	project(openxr-tutorial)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
	set(OPENXR_DIR "" CACHE PATH "Location of OpenXR-SDK repository.")
	add_subdirectory(Chapter2)

Now let's create a folder called Chapter2, and in it put another CMakeLists.txt file,
this one containing:

.. code-block:: cmake

	file(GLOB SOURCES "main.cpp" )
	add_executable(Chapter2 ${SOURCES})
	target_include_directories(Chapter_2 PUBLIC ${OPENXR_DIR})
	target_link_directories(Chapter_2 PUBLIC ${OPENXR_DIR}/build/src/loader/Debug ${OPENXR_DIR}/build/src/loader/Release)
	target_link_libraries(Chapter_2 openxr_loader$<$<CONFIG:Debug>:d>)

That's all we need for CMake. Now we'll create our source file. Create a new text file called "main.cpp"
and put it in the Chapter2 directory. In this file, place the following code:

.. highlight:: cpp
.. code-block:: cpp

	#if defined(_WIN32)
	#define NO_MINMAX
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#elif(__ANDROID__)
	#include "android_native_app_glue.h"
	#endif

	// C++ Headers
	#include <iostream>

	// OpenXR Headers
	#include "openxr/openxr.h"

	//#define XR_USE_GRAPHICS_API_D3D11
	//#define XR_USE_GRAPHICS_API_D3D12
	//#define XR_USE_GRAPHICS_API_OPENGL
	//#define XR_USE_GRAPHICS_API_OPENGL_ES
	//#define XR_USE_GRAPHICS_API_VULKAN
	#include "openxr/openxr_platform.h"

	//Debugbreak
	#if defined(_MSC_VER)
	#define DEBUG_BREAK __debugbreak()
	#else
	#include <signal.h>
	#define DEBUG_BREAK raise(SIGTRAP)
	#endif

	class OpenXRTutorial_Ch2_1
	{
	public:
		OpenXRTutorial_Ch2_1() = default;
		~OpenXRTutorial_Ch2_1() = default;

		void Run()
		{
		}
	};

	void OpenXRTutorial_Main()
	{
		std::cout << "OpenXR Tutorial Chapter 2.1." << std::endl;

		OpenXRTutorial_Ch2_1 app;
		app.Run();
	}

	#if defined(_WIN32)
	int WINAPI wWinMain(_In_ HINSTANCE hInstance,
						 _In_opt_ HINSTANCE hPrevInstance,
						 _In_ LPWSTR    lpCmdLine,
						 _In_ int       nCmdShow)
	{
		OpenXRTutorial_Main();
		return 0;
	}
	#elif(__ANDROID__)
	void android_main(struct android_app* app)
	{
		OpenXRTutorial_Main();
	}
	#endif

Now launch Cmake GUI, and in the 

2. SETUP (Basic structure ready)

Creating an XrInstance
----------------------
2.1. Creating an XrInstance / xrGetSystem (xrCreateInstance)

Creating an XrSession
---------------------

2.2. Creating an XrSession (xrCreateSession, OpenGL based for code brevity)

Polling the Event Loop
----------------------

2.3. 2.3. Polling the Event Loop (xrPollEvent and Session States)