############
👋 Introduction
############

**************
Goal of OpenXR
**************

OpenXR aims to help solve the fragmentation of the XR ecosystem. Before the advent of OpenXR, software developers working with multiple
hardware platforms had to write different code for each platform to address the different hardware.
Each platform had its own, often proprietary, API, and deploying an existing app to a new platform required a lot of
adaptation. Developing a new app for a new platform was even more challenging.

In spite of their unique features, the platforms had a great deal in common. For example, most headsets had a main view seen from two
slightly different perspectives. Most had a way to track the user's head and hands or hand-controllers. Most had buttons,
many had analogue controls like triggers or joysticks; many had haptic feedback.

.. figure:: OpenXRBeforeAfter.png
	:alt: XR Fragmentation 
	:align: left
	:width: 99%

	OpenXR provides a common interface to reduce XR fragmentation.

OpenXR aims to solve this problem by providing a common API to address XR hardware, in reading its inputs
and outputting to its displays and haptic systems. Just as OpenGL and Vulkan provide a common API to access graphics hardware, so OpenXR
allows you to write code that works with multiple XR platforms, with minimal adaptation.


********
Overview
********

We'll start with the main concepts you'll need to be familiar with around OpenXR.


.. list-table:: OpenXR Concepts
	:widths: 1 5
	:class: longtable
	:header-rows: 1

	* - Concept
	  - Description
	* - API
	  - The OpenXR API is the set of commands, functions and structures that an OpenXR-compliant runtime is required to offer.
	* - Application
	  - The Application is your program, called an "app" for short.
	* - Runtime
	  - A Runtime is a specific implementation of the OpenXR functionality. It might be provided by a
	    hardware vendor, as part of a device's operating system; it might be supplied by a software vendor
	    to enable OpenXR support with a specific range of hardware. The Loader finds the appropriate Runtime
	    and loads it when OpenXR is initialized.
	* - Loader
	  - The OpenXR loader is a special library that connects your app to whichever OpenXR runtime
	    you're using. The loader's job is to find the Runtime and initialize it, then allow your app to access
	    the Runtime's version of the API. Some devices can have multiple Runtimes available, but only one can
	    be active at any given time.
	* - Layers
	  - API layers are optional components that augment an OpenXR system. A Layer might help with debugging,
	    or filter infromation between the app and the Runtime. API layers are enabled when the OpenXR Instance
	    is created.
	* - Instance
	  - The Instance is an object that allows your app to communicate with a Runtime. You'll ask OpenXR to create an Instance
	    when initializing XR support in your app. If the Runtime supports it, you might have more than one Instance
	    at a time, if more than one XR device is in use.
	* - Graphics
	  - OpenXR usually needs to connect to a graphics API, in order to permit rendering of headset views for example.
	    Which Graphics API's are supported depends on the Runtime.
	* - Input
	  - The OpenXR Input System allows apps to query what inputs are available. These can then be bound
	    to Actions or Poses, so the app knows what the user is doing.

	OpenXR's lexicon and API style are based on the Vulkan API, which provides a clear and precise common language for developers and hardware vendors to use.

	API Layers are additional code layers that are inserted between the application and the runtime. Each of these API layers intercepts the OpenXR function calls from the layer above, does something with that function and then calls the next layer down. Some simple examples of API Layers would either log the OpenXR functions to the output or a file, or create trace file of the OpenXR calls for later replay. A validation layer could be used to check that the function calls made to OpenXR are compatible with the specification and the current state of OpenXR, which would be very similar the Vulkan Validation layer.

	OpenXR supports multiple graphics APIs via its extension functionality. Like in Vulkan, OpenXR can extend its functionality to include debugging layers, vendor hardware and software support and graphics API. This idea of absolving the graphics APIs functionality from the main specification, as bold as it might seem, provides us with flexibility choosing the graphics APIs now and into the future. Firstly, OpenXR is targeted at developing XR experiences and isn't concerned with the specifics of the graphics APIs. Secondly, the extensive nature of OpenXR allows revisions of and new graphics APIs to be integrated with ease. Already, There are two mutually exclusive extensions in OpenXR for interacting with Vulkan.

**********
Setting Up
**********

This section will help you set up your development environment. This is the one place where your choice of platform really makes a difference!
After that, things will be much more consistent.

The exact procedure will depend on your target platform. So first, let's choose this:

.. raw:: html
   :file: platforms.html

Select which platform you want to develop for, and click the button to show the instructions for that platform.

.. container:: windows
    :name: windows-intro-1

	.. rubric:: Windows

	You'll see instructions to build on, and for Windows.

.. container:: linux
    :name: linux-intro-1

	.. rubric:: Linux

	You'll see instructions to build on, and for Linux.

.. container:: android
    :name: android-intro-1

	.. rubric:: Android

	You'll see instructions to build for Android, using either a Linux or Windows PC.

.. container:: windows

	.. rubric:: Visual Studio

	If you'll be building an OpenXR project for PC-based devices, we'll assume you'll be using Microsoft Visual Studio.
	The free Community edition of Visual Studio is available `here <https://visualstudio.microsoft.com/vs/community/>`_.

	.. rubric:: Direct3D 11 or 12

	Next you'll want to choose which Graphics API you'll be using. Direct3D 11 and 12 are built into the Windows SDK's.
	If you're using Direct3D, I recommend D3D12, because it supports `View Instancing <https://microsoft.github.io/DirectX-Specs/d3d/ViewInstancing.html>`_. Essentially, it allows you to draw both eye views in one call.

	Whether D3D11 or 12, download and install the latest `Windows SDK <https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/>`_.

	.. rubric:: Vulkan Windows SDK

	If you'll be using Vulkan, download and install the latest `Vulkan SDK <https://www.lunarg.com/vulkan-sdk/>`_. Take a note of the installation location,
	as this will be needed later.
	Vulkan, OpenGL and OpenGL ES also supports rendering to bith eye views with multiview.
	 * `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.
	 * `OpenGL/OpenGL ES Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.


	.. rubric:: OpenXR SDK for Windows

	You'll need a copy of the OpenXR SDK, which is distributed as a Git repository. If you're familiar with Git, you can use your preferred command-line or GUI Git tool to get
	the SDK from *git@github.com:KhronosGroup/OpenXR-SDK.git*.
	Alternatively, you can download it as a .zip file from `github.com/KhronosGroup/OpenXR-SDK <https://github.com/KhronosGroup/OpenXR-SDK>`_.

	.. rubric:: CMake

	The OpenXR SDK is distributed as source code, and you'll need to build it, using CMake.
	CMake is a program that generates project files, such as the .vcxproj and .sln files
	that Visual Studio needs.
	Install the latest `CMake <https://cmake.org/download/>`_.

	Once CMake is installed, click the Windows button, and type "cmake" to find the application Cmake GUI.

	.. figure:: find_cmake.png
	   :alt: Find CMake by clicking the Windows icon and typing "cmake".
	   

	In the CMake Gui, enter the location of the OpenXR SDK in the top text box, marked "Where is the source code". Below, in the box marked "Where to
	build the binaries", enter a subdirectory of that folder, called "build".

	.. figure:: cmake-openxrsdk-1.png
	   :alt: CMake GUI: location of the OpenXR SDK has been entered as the source directory, and that a subdirectory "build" has been entered as the binary directory.
	   

	Click the button below marked "Configure". A popup will ask if CMake should create the "build" directory. Click "Yes".
	Now a box will appear where you can select a "Generator". This should default to the version of
	Visual Studio you've just installed. Click Finish.
	Cmake will now configure the project, and a list of variables will appear in red.

	.. figure:: cmake-openxrsdk-2.png
	   :alt: alternate text
	   

	You shouldn't have to change any of these variables, but you can if you want. CMake should have
	automatically found the installation of the Vulkan SDK that you created previously, and used it to fill in the variables
	``Vulkan_INCLUDE_DIR``, ``Vulkan_LIBRARY`` and so on. If not, you can modify these variables
	by clicking on the "Value" entries. The values should look like this:


	.. figure:: cmake-vulkan-vars.png
	   :alt: Vulkan variable highlighted in the CMake GUI.
	   

	The Vulkan variables in CMake GUI should read:
	 +-----------------------------------------+-----------------------------------------------------+
	 | ``Vulkan_GLSLANG_VALIDATOR_EXECUTABLE`` | ``C:/VulkanSDK/1.3.239.0/Bin/glslangValidator.exe`` |
	 +-----------------------------------------+-----------------------------------------------------+
	 | ``Vulkan_GLSLC_EXECUTABLE``             | ``C:/VulkanSDK/1.3.239.0/Bin/glslc.exe``            |
	 +-----------------------------------------+-----------------------------------------------------+
	 | ``Vulkan_INCLUDE_DIR``                  | ``C:/VulkanSDK/1.3.239.0/Include``                  |
	 +-----------------------------------------+-----------------------------------------------------+
	 | ``Vulkan_LIBRARY``                      | ``C:/VulkanSDK/1.3.239.0/Lib/vulkan-1.lib``         |
	 +-----------------------------------------+-----------------------------------------------------+
	Obviously, the precise directory will depend on which version of the Vulkan SDK you installed.

	Now, we're going to "Generate" the project files for the OpenXR SDK. Click "Generate", and CMake GUI should
	now read "Generating done" in the output log at the bottom.

	.. figure:: cmake-openxrsdk-generate.png
	   :alt: CMake GUI should read "Generating done" in the output log at the bottom.
	   

	Now we can build the SDK. Click "Open Project" in CMake GUI, or navigate to your "build" folder and double-click on
	the file "OPENXR.sln", and Visual Studio will open the solution.

	Select "Debug" from the Visual Studio "Solution Configurations" dropdown.

	.. figure:: visual-studio-openxr-debug.png
	   :alt: In Visual Studio, the Solution Configuration dropdown menu is shown, with "Debug" selected.
	   

	Go to the Build menu and select "Build Solution". The projects will be compiled, and the output
	from ``openxr_loader.vcxproj`` should be a library called ``openxr_loaderd.lib``.

	.. figure:: visual-studio-openxr-build.png
	   :alt: In Visual Studio, the "Build" menu is shown, with the "Build Solution" option selected.
	   

	Now repeat the process for the Release build. Select "Release" from the Configurations dropdown,
	amd build the solution. This time, the library ``openxr_loader.lib`` will be built.

	You can now close this solution, you're ready to start creating your first OpenXR project.

	
.. container:: linux

	.. rubric:: Visual Studio Code for Linux

	To install Visual Studio Code, go to `code.visualstudio.com <https://code.visualstudio.com/>`_ and click the "Download for Linux" button.

	.. rubric::  OpenXR SDK for Linux

	You'll need a copy of the OpenXR SDK, which is distributed as a Git repository. If you're familiar with Git, you can use your preferred command-line or GUI Git tool to get
	the SDK from *git@github.com:KhronosGroup/OpenXR-SDK.git*.

	In the downloaded repo, you'll find a file called README.md, which contains up-to-date instructions
	for building the libraries. So follow these instructions in the *Linux* section. In
	general, you will first want to install the required packages, which at the time of writing were:

	.. code-block:: bash

		sudo apt install build-essential
		sudo apt install cmake
		sudo apt install libgl1-mesa-dev
		sudo apt install libvulkan-dev
		sudo apt install libx11-xcb-dev
		sudo apt install libxcb-dri2-0-dev
		sudo apt install libxcb-glx0-dev
		sudo apt install libxcb-icccm4-dev
		sudo apt install libxcb-keysyms1-dev
		sudo apt install libxcb-randr0-dev
		sudo apt install libxrandr-dev
		sudo apt install libxxf86vm-dev
		sudo apt install mesa-common-dev

	From the OpenXR-SDK directory,

	.. code-block:: bash

		mkdir -p build/linux_debug
		cd build/linux_debug
		cmake -DCMAKE_BUILD_TYPE=Debug ../..
		make

	Now return to the OpenXR-SDK directory, and build the release library:

	.. code-block:: bash

		mkdir -p build/linux_release
		cd build/linux_release
		cmake -DCMAKE_BUILD_TYPE=Release ../..
		make

	This builds ``libopenxr_loader.so``, in Debug and Release flavours, in the directories:
	``OpenXR-SDK/build/linux_debug/src/loader`` and ``OpenXR-SDK/build/linux_release/src/loader``.
	Both files have the same name on Linux - the debug one is substantially larger however.
	And both are *dynamic* libraries by default, which will be loaded at runtime.

	Now you're ready to start creating your first OpenXR project.

.. container:: android
	
	.. rubric:: Android Studio
	
	Android Studio is available to download here: `https://developer.android.com/studio <https://developer.android.com/studio>`_.
	
	.. rubric::  OpenXR SDK
	
	For Android, we will use the CMake ``FetchContent_Declare()`` command in the ``CMakeLists.txt`` to get the OpenXR package from Khronos's OpenXR-SDK-Source GitHub page.


	For Android, you can download the OpenXR loader libraries from here: `https://github.com/KhronosGroup/OpenXR-SDK-Source/releases/release-1.0.27 <https://github.com/KhronosGroup/OpenXR-SDK-Source/releases/release-1.0.27>`_.
	Using the .aar file and a program like 7-Zip, you can extract the header files and libraries. Under ``prefab/modules/openxr_loader/include/openxr``, you'll find the header files, and under ``prefab/modules/openxr_loader/libs/``, you'll find the folders for the arm64-v8a, armeabi-v7a, x86 and x86_64 libraries.
	
	.. figure:: android-7Zip-include.png
	   :alt: 7-Zip internal file structure showing the OpenXR headers. prefab/modules/openxr_loader/include/openxr
	   
	
	.. figure:: android-7Zip-libs.png
	   :alt: 7-Zip internal file structure showing the OpenXR libraries. prefab/modules/openxr_loader/libs
	   
	
	.. rubric:: Vulkan
	
	

	Vulkan is recommended for Android for its modern, low-level API and extension.
	Vulkan is included as part of the NDK provided Google and is supported on Android 7.0 (Nougat), API level 24 or higher. `https://developer.android.com/ndk/guides/graphics <https://developer.android.com/ndk/guides/graphics>`_.
	Alternatively, OpenGL ES is also an option for Android graphics.

	With the OpenXR loader and your graphics API selected, you're ready to start creating your first OpenXR project.