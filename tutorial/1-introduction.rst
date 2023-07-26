#################
👋 1 Introduction
#################

******************
1.1 Goal of OpenXR
******************

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


************
1.2 Overview
************

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
		or filter information between the app and the Runtime. API layers are enabled when the OpenXR Instance
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

OpenXR's lexicon and API style are based on the Vulkan API, and it provides a clear and precise common language for developers and hardware vendors to use. It was a decision by the OpenXR working group to have APIs similar.

An OpenXR Runtime implements the OpenXR API. There maybe multiple runtime installed on a system, but an OpenXR application can only choose one. The runtime acts to translate the OpenXR function calls into something that the vendor's software/hardware can understand. There is a fully open source OpenXR runtime for Linux in development called `Monado <https://monado.dev/>`_.

The OpenXR Loader finds and loads a suitable OpenXR runtime that is present on the system. The Loader will load in all of the OpenXR function pointers stated in the core specification for the application to use. If you are using an extension, such as ``XR_EXT_debug_utils``, any functions associated with that extension will need to be loaded in with ``xrGetInstanceProcAddr()``. Some platforms like Android require extra work and information to initialise the loader.

API Layers are additional code layers that are inserted between the application and the runtime. Each of these API layers intercepts the OpenXR function calls from the layer above, does something with that function, and then calls the next layer down. Some simple examples of API Layers would be logging the OpenXR functions to the output or a file, or creating trace file of the OpenXR calls for later replay. A validation layer could be used to check that the function calls made to OpenXR are compatible with the specification and with the current state of OpenXR, which would be very similar the Vulkan Validation layer.

OpenXR supports multiple graphics APIs via its extension functionality. Like in Vulkan, OpenXR can extend its functionality to include debugging layers, vendor hardware and software support and graphics APIs. This idea of absolving the core specification of the graphics API functionality, as bold as it might seem, provides us with the flexibility in choosing the graphics APIs now and in the future. Firstly, OpenXR is targeted at developing XR experiences and isn't concerned with the specifics of any graphics APIs. Secondly, the extensive nature of OpenXR allows revisions of and any new graphics APIs to be integrated with ease. Already, there are two mutually exclusive extensions in OpenXR for interacting with Vulkan.
	
OpenXR recognised that there is vast and ever changing array of hardware and configurations in the XR space. With new headsets and contollers coming to the market, an abstraction of the input system was needed so that same application can target difference and newer hardware. The abstraction is done via the concept of an ``XrAction``, which acts as handle to interactive elements of the application. Instead of directly querying the state of any one button, joysticks, trigger, touch pad etc., you create an ``XrAction`` for a specific action such as a "menu_click". You provide a suggested binding along with an interaction profile so that OpenXR can link that action with the available input hardware at runtime.

**************
1.3 Setting Up
**************

This section will help you set up your development environment. Here your choice of platform really makes a difference. After that, things will be much more consistent.

You can change platform at any time by clicking the tabs at the top of the page. Select the platform you want to develop for
by clicking a tab, or one of the buttons below.

.. raw:: html
	:file: platforms.html

.. container:: windows

	.. rubric:: Visual Studio

	If you'll be building an OpenXR project for Microsoft Windows PC based devices, we'll assume you'll be using Microsoft Visual Studio.
	The free Community edition of Visual Studio is available to download `here <https://visualstudio.microsoft.com/vs/community/>`_.

	.. rubric:: CMake

	Install the latest `CMake <https://cmake.org/download/>`_. This tutorial uses CMake to generate the Visual Studio Solution and Project files.

	Next you'll want to choose which Graphics API you'll be using.
	
	.. rubric:: Direct3D 11 or 12

	Direct3D 11 and 12 are built into the Windows SDK's. If you're using Direct3D, D3D12 is recommended, because it supports `View Instancing <https://microsoft.github.io/DirectX-Specs/d3d/ViewInstancing.html>`_. Essentially, it allows you to draw both eye views in one call.

	In both cases, download and install the latest `Windows SDK <https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/>`_.

	.. rubric:: Vulkan

	If you'll be using Vulkan, download and install the latest `Vulkan SDK <https://www.lunarg.com/vulkan-sdk/>`_. Vulkan also supports rendering to both eye views with multiview. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.

	.. rubric:: OpenGL

	For this tutorial, we are using the 'gfxwrapper' for the OpenGL API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. It was originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ specification.
	If you want to use OpenGL stand-alone, you will need to use WGL to create a valid OpenGL Context for Microsoft Windows - `Creating an OpenGL Context WGL <https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)>`_. You will also need to use a function loader like GLAD to access functions for OpenGL - `GLAD <https://glad.dav1d.de/>`_. OpenGL also supports rendering to both eye views with multiview - `OpenGL Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.

.. container:: linux

	.. rubric:: Visual Studio Code for Linux

	To install Visual Studio Code, go to `https://code.visualstudio.com/ <https://code.visualstudio.com/>`_ and click the "Download for Linux" button.

	.. rubric:: CMake
		
	Install the latest `CMake <https://cmake.org/download/>`_. This tutorial uses CMake with Visual Studio Code to build the project.

	.. rubric:: Vulkan

	If you'll be using Vulkan, download and install the latest `Vulkan SDK <https://www.lunarg.com/vulkan-sdk/>`_. Vulkan also supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.

	.. rubric:: OpenGL

	For this tutorial, we are using the 'gfxwrapper' for the OpenGL API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. It was originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ specification.
	If you want to use OpenGL stand-alone, you will need to use GLX to create a valid OpenGL Context for Linux - `Tutorial: OpenGL 3.0 Context Creation (GLX) <Tutorial:_OpenGL_3.0_Context_Creation_(GLX)>`_. You will also need to use a function loader like GLAD to access functions for OpenGL - `GLAD <https://glad.dav1d.de/>`_. OpenGL also supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.
	
.. container:: android
	
	.. rubric:: Android Studio
	
	Android Studio is available to download here: `https://developer.android.com/studio <https://developer.android.com/studio>`_.
	
	.. rubric::  OpenXR SDK
	
	In conjunction with the later ``CMakeLists.txt``, we add this to our dependencies in the ``app/build.gradle`` file. This ``openxr_loader_for_android`` also contains an ``AndroidManifest.xml`` files that includes the required uses-permissions, authorising the OpenXR runtime broker and states the intent to use the ``OpenXRRuntimeService`` and the ``OpenXRApiLayerService``.

	.. literalinclude:: ../Chapter2/app/build.gradle
		:language: groovy
		:start-at: dependencies {
		:end-at: }
		:emphasize-lines: 5

	.. rubric:: Vulkan

	Vulkan is recommended for Android for its modern, low-level API and extension.
	Vulkan is included as part of the NDK provided Google and is supported on Android 7.0 (Nougat), API level 24 or higher. `https://developer.android.com/ndk/guides/graphics <https://developer.android.com/ndk/guides/graphics>`_. Vulkan also supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.
	
	.. rubric:: OpenGL ES
	Alternatively, OpenGL ES is also an option for Android graphics.
	For this tutorial, we are using the 'gfxwrapper' for the OpenGL ES API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. 
	If you want to use OpenGL ES stand-alone, you will need to use EGL to create a valid OpenGL ES Context for Android - `EGL Overview <https://www.khronos.org/egl>`_. You will also need to use a function loader like GLAD to access functions for OpenGL ES - `GLAD <https://glad.dav1d.de/>`_.	OpenGL ES also supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL ES Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.


.. rubric:: OpenXR SDK

In this tutorial, we will use CMake ``FetchContent_Declare()`` command in the ``CMakeLists.txt`` files to get the OpenXR SDK package from Khronos’s OpenXR-SDK-Source GitHub page, as shown below.

.. literalinclude:: ../Chapter2/CMakeLists.txt
	:language: cmake
	:start-at: # For FetchContent_Declare() and FetchContent_MakeAvailable()
	:end-before: # Files

If you prefer to git clone or download the SDK yourself from Khronos’s OpenXR-SDK-Source GitHub page `here <https://github.com/KhronosGroupOpenXR-SDK>`_. You can either use your preferred command-line or GUI Git tool, or download it as a .zip file.

With the OpenXR loader and your graphics API selected, you're ready to start creating your first OpenXR project.
