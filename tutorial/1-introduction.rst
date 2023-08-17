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
	
OpenXR recognised that there is vast and ever changing array of hardware and configurations in the XR space. With new headsets and controllers coming to the market, an abstraction of the input system was needed so that same application can target difference and newer hardware. The abstraction is done via the concept of an ``XrAction``, which acts as handle to interactive elements of the application. Instead of directly querying the state of any one button, joysticks, trigger, touch pad etc., you create an ``XrAction`` for a specific action such as a "menu_click". You provide a suggested binding along with an interaction profile so that OpenXR can link that action with the available input hardware at runtime.

**************
1.3 Setting Up
**************

This section will help you set up your development environment. Here your choice of platform really makes a difference. After that, things will be much more consistent.

You can change platform at any time by clicking the tabs at the top of the page. Select the platform you want to develop for
by clicking a tab, or one of the buttons below.

.. raw:: html
	:file: platforms.html

.. container:: windows
	:name: windows-id-1

	.. rubric:: Visual Studio

	If you'll be building an OpenXR project for Microsoft Windows PC based devices, we'll assume you'll be using Microsoft Visual Studio.
	The free Community edition of Visual Studio is available to download `here <https://visualstudio.microsoft.com/vs/community/>`_.

	.. rubric:: CMake

	Install the latest `CMake <https://cmake.org/download/>`_. This tutorial uses CMake to generate the Visual Studio Solution and Project files.

	Next you'll want to choose which Graphics API you'll be using.
	
	.. container:: d3d11 d3d12
		:name: d3d11-d3d12-id-1

		Direct3D 11 and 12 are built into the Windows SDK's. If you're using Direct3D, D3D12 is recommended, because it supports `View Instancing <https://microsoft.github.io/DirectX-Specs/d3d/ViewInstancing.html>`_. Essentially, it allows you to draw both eye views in one call.

		In both cases, download and install the latest `Windows SDK <https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/>`_.
		
	.. container:: vulkan
		:name: vulkan-id-1

		If you'll be using Vulkan, download and install the latest `Vulkan SDK <https://www.lunarg.com/vulkan-sdk/>`_. Vulkan also supports rendering to both eye views with multiview. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.

	.. container:: opengl
		:name: opengl-id-1

		For this tutorial, we are using the 'gfxwrapper' for the OpenGL API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. It was originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ specification.
		If you want to use OpenGL stand-alone, you will need to use WGL to create a valid OpenGL Context for Microsoft Windows - `Creating an OpenGL Context WGL <https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)>`_. You will also need to use a function loader like GLAD to access functions for OpenGL - `GLAD <https://glad.dav1d.de/>`_. OpenGL also supports rendering to both eye views with multiview - `OpenGL Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.

.. container:: linux
	:name: linux-id-1

	.. rubric:: Visual Studio Code for Linux

	To install Visual Studio Code, go to `https://code.visualstudio.com/ <https://code.visualstudio.com/>`_ and click the "Download for Linux" button.

	Install the latest `CMake <https://cmake.org/download/>`_. This tutorial uses CMake with Visual Studio Code to build the project.

	.. container:: vulkan
		:name: vulkan-id-2

		If you'll be using Vulkan, download and install the latest `Vulkan SDK <https://www.lunarg.com/vulkan-sdk/>`_. Vulkan also supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.

	.. container:: opengl
		:name: opengl-id-2

		For this tutorial, we are using the 'gfxwrapper' for the OpenGL API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. It was originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ specification.
		If you want to use OpenGL stand-alone, you will need to use GLX to create a valid OpenGL Context for Linux - `Tutorial: OpenGL 3.0 Context Creation (GLX) <Tutorial:_OpenGL_3.0_Context_Creation_(GLX)>`_. You will also need to use a function loader like GLAD to access functions for OpenGL - `GLAD <https://glad.dav1d.de/>`_. OpenGL also supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.
	
.. container:: android
	:name: android-id-1
	
	Android Studio is available to download here: `https://developer.android.com/studio <https://developer.android.com/studio>`_.
	
	.. rubric::  OpenXR SDK
	
	In conjunction with the later ``CMakeLists.txt``, we add the OpenXR SDK to our dependencies in the ``app/build.gradle`` file. The ``openxr_loader_for_android`` provides an ``AndroidManifest.xml`` that will be merged into our own, setting some required properties for the package and application. With this though, we are still required to add to our own ``AndroidManifest.xml`` file the relevant intent filters, such as ``<category android:name="org.khronos.openxr.intent.category.IMMERSIVE_HMD" />``. It also provides the OpenXR headers and library binaries in a format that the Android Gradle Plugin will expose to CMake.

	.. literalinclude:: ../Chapter2/app/build.gradle
		:language: groovy
		:start-at: dependencies {
		:end-at: }
		:emphasize-lines: 5

	.. container:: vulkan
		:name: vulkan-id-3

		Vulkan is recommended for Android for its modern, low-level API and extension.
		Vulkan is included as part of the NDK provided Google and is supported on Android 7.0 (Nougat), API level 24 or higher. `https://developer.android.com/ndk/guides/graphics <https://developer.android.com/ndk/guides/graphics>`_. Vulkan also supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.
	
	.. container:: opengles
		:name: opengles-id-1
		
		Alternatively, OpenGL ES is also an option for Android graphics.
		For this tutorial, we are using the 'gfxwrapper' for the OpenGL ES API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. 
		If you want to use OpenGL ES stand-alone, you will need to use EGL to create a valid OpenGL ES Context for Android - `EGL Overview <https://www.khronos.org/egl>`_. You will also need to use a function loader like GLAD to access functions for OpenGL ES - `GLAD <https://glad.dav1d.de/>`_.	OpenGL ES also supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL ES Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.


.. rubric:: OpenXR SDK

In this tutorial, we will use CMake ``FetchContent_Declare()`` command in the ``CMakeLists.txt`` files to get the OpenXR SDK package from Khronos’s OpenXR-SDK-Source GitHub page, as shown below.

.. literalinclude:: ../Chapter2/CMakeLists.txt
	:language: cmake
	:start-at: # For FetchContent_Declare() and FetchContent_MakeAvailable()
	:end-before: # Files

Download :download:`CMakeLists.txt <../Chapter2/CMakeLists.txt>`

If you prefer to git clone or download the SDK yourself from `Khronos’s OpenXR-SDK-Source GitHub page <https://github.com/KhronosGroup/OpenXR-SDK>`_. You can either use your preferred command-line or GUI Git tool, or download it as a .zip file.

With the OpenXR loader and your graphics API selected, you're ready to start creating your first OpenXR project.

*****************
1.4 Project Setup
*****************

This sub-chapter explains how to setup your project ready for :ref:`Chapter 2<2.1 Creating an XrInstance / xrGetSystem>` and will make references to the ``/Chapter2`` folder. It explains including the OpenXR headers, linking the ``openxr_loader`` library, graphics API integration, other boilerplate code and finally creating a simple stub-application with the ``OpenXRTutorial`` class to test.

1.4.1 CMake and Project Files
=============================

.. container:: windows
	:name: windows-id-2

	For the Microsoft Windows OpenXR project, we'll use CMake to create the solution and project files for Visual Studio.
	First, create a directory where the code will go, we'll call this the *workspace* directory.

.. container:: linux
	:name: linux-id-2

	You are free to use any code editor and/or compiler; this tutorial will use Visual Studio Code.
	For the Linux OpenXR project, we'll use CMake alongside Visual Studio Code to build the project.
	First, create a directory where the code will go, we'll call this the *workspace* directory. Open Visual Studio Code and from the File menu, select "Open Folder..."

	.. figure:: linux-vscode-open-folder.png
		:alt: The File menu of Visual Studio Code is shown, with the command "Open Folder..." selected.
		:scale: 55%

	Select your *workspace* folder, which is currently empty.
	
	If you haven't previously done so, install the CMake extension for Visual Studio Code.
	
.. container:: windows linux
	:name: windows-linux-id-1

	Now, create a text file in the *workspace* folder called ``CMakeLists.txt`` and in it, put the following code:

	.. literalinclude:: ../CMakeLists.txt
		:language: cmake
		:emphasize-lines: 11

	Here, we specify the CMake version, project name and the configurations types, which we limit to just Debug and Release.
	You can use ``XR_RUNTIME_JSON`` to specify an optional runtime. The one provided by your hardware vendor will be automatically used, though this feature is helpful for debugging on different runtimes.
	Finally, we specify CMake to continue the build into the Chapter2 directory with ``add_subdirectory()``.

	In the workspace folder, create a folder called ``Chapter2``, and in it create another ``CMakeLists.txt`` file.
	In it, put the following code:

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: cmake_minimum_required
		:end-at: )

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: # For FetchContent_Declare() and FetchContent_MakeAvailable()
		:end-before: # Files

	After setting our CMake version, we include ``FetchContent`` and use it to get the OpenXR-SDK from Khronos's GitHub page. Now, we will add to our ``CMakeLists.txt`` to specify the source and header files by adding the following code. Here, we are including all the files needed for our project. 

	.. container:: d3d11
		:name: d3d11-id-1

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-at: # Files
			:end-before: if (ANDROID) # Android
			:emphasize-lines: 4, 5, 13, 14
	
	.. container:: d3d12
		:name: d3d12-id-1

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-at: # Files
			:end-before: if (ANDROID) # Android
			:emphasize-lines: 4, 6, 13, 15
	
	.. container:: opengl
		:name: opengl-id-3

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-at: # Files
			:end-before: if (ANDROID) # Android
			:emphasize-lines: 4, 7, 13, 16

	.. container:: vulkan
		:name: vulkan-id-4

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-at: # Files
			:end-before: if (ANDROID) # Android
			:emphasize-lines: 4, 9, 13, 18

	All the files listed above with ``../Common/*.*`` are available to download from this tutorial website. In the next section, you will find the links and discussion of their usage within this tutorial and with OpenXR. This tutorial includes all the graphics APIs header and cpp files; you only need to download the files pertaining to your graphics API choice. 
	
	You should remove the ``GraphicsAPI`` cpp and header files that do not relate to your chosen graphics API from the ``Chapter2/CMakeList.txt`` files. You must keep ``GraphicsAPI.cpp`` and ``OpenXRDebugUtils.cpp`` in the ``SOURCES`` list and also keep ``DebugOutput.h``, ``GraphicsAPI.h``, ``HelperFunctions.h``, ``OpenXRDebugUtils.h`` and ``OpenXRHelper.h`` in the ``HEADERS`` list.

	At this point, we'll also create our text file called ``main.cpp`` in the ``/Chapter2`` directory. This will be our main source file, which we will use later in this chapter.

	Now, we will continue with the ``Chapter2/CMakeList.txt``, adding the following code.

	.. container:: d3d11
		:name: d3d11-id-2
		
		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-after: else() # Windows / Linux
			:end-before: endif() # EOF
			:dedent: 4
			:emphasize-lines: 10, 12

	.. container:: d3d12
		:name: d3d12-id-2

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-after: else() # Windows / Linux
			:end-before: endif() # EOF
			:dedent: 4
			:emphasize-lines: 11-12

	.. container:: opengl
		:name: opengl-id-4

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-after: else() # Windows / Linux
			:end-before: endif() # EOF
			:dedent: 4
			:emphasize-lines: 23-25

	.. container:: vulkan
		:name: vulkan-id-5
		
		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-after: else() # Windows / Linux
			:end-before: endif() # EOF
			:dedent: 4
			:emphasize-lines: 17-21

	Now, we have set up the project by adding an executable by using ``add_executable()`` specifying the ``${SOURCES}`` and ``${HEADERS}``. We have optionally set a the ``XR_RUNTIME_JSON`` in the debugger environment. We've added the ``../Common`` folder as an include directory and linked ``openxr_loader`` from ``FetchContent``. We didn't have to add OpenXR as a include directory; as we have specified it to be located in the CMake Build directory under ``openxr/`` when using ``FetchContent``, so we've only included the OpenXR headers from the CMake Build directory.
	
	We have also added the include directories and linkage for the graphics APIs.
	For Microsoft Windows, we've linked ``d3d11.lib``, ``d3d12.lib`` and ``dxgi.lib``, so that we can use Direct3D 11, Direct3D 12 and the DirectX Graphics Infrastructure. The headers for Direct3D are automatically included as part of the Visual Studio project.
	If you have the Vulkan SDK installed, this CMake code will try to find that package, add the include directories and link the Vulkan libraries.
	As a default, this CMake has added gfxwrapper for OpenGL, we've added the include directories and linked the gfxwrapper libraries from the OpenXR-SDK.

	Note: You can remove or comment out from the above CMake code any references to graphics APIs that are not in use, if you wish to do so. Leaving them in place will not effect your project.

	That's all the CMake code that we require for this project. 

.. container:: android
	:name: android-id-2

	Here, We'll show how to hand build an Android Studio project that runs a C++ Native Activity.
	First, we will create a *workspace* folder and in that folder create that ``/Chapter2`` folder,
	Open Android Studio, select New Project and choose an Empty Activity. Set the Name to 'OpenXR Tutorial Chapter 2', the Package name to 'com.simul.OpenXRTutorialChapter2' and save location to that ``/Chapter2`` folder. The language can be ignored here as we are using C++, and we can set the Minimum SDK to API 24: Android 7.0(Nougat). Complete the set up.

	.. figure:: android-studio-newproject.png
		:alt: Android Studio - New Project - Empty Activity.
		:align: left

	.. rubric:: CMake

	With the Android Studio project now set up, we need to modify some of the files and folders so as to set up the project to support the C++ Native Activity.
	Under the ``app`` folder, you can delete the ``libs`` folder, and under the ``app/src`` you can also delete the ``androidTest`` and ``test`` folders. Finally under ``app/src/main``, delete the ``java`` folder and add a ``cpp`` folder. Under the ``app/src/main/res``, delete the ``layout``, ``values-night`` and ``xml`` folders. Under the ``values`` folder, delete ``colors.xml`` and ``themes.xml``

	Create a ``CMakeLists.txt`` in the ``/Chapter2`` directory for compatibility with other platforms. We will use this file to specific how our Native C++ code will be built. This ``CMakeLists.txt`` file will be invoked by Android Studio's Gradle build system and we will point Gradle to this CMake file. 

	.. rubric:: CMakeLists.txt

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: cmake_minimum_required
		:end-at: # Needed for Android

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: # For FetchContent_Declare() and FetchContent_MakeAvailable()
		:end-before: # Files

	After setting our CMake version to 3.22.1 and the project's name, we include ``FetchContent`` and use it to get the OpenXR-SDK from Khronos's GitHub page.

	.. container:: opengles
		:name: opengles-id-2

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-at: # Files
			:end-before: if (ANDROID) # Android
			:emphasize-lines: 4, 8, 13, 17
	
	.. container:: vulkan
		:name: vulkan-id-6

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-at: # Files
			:end-before: if (ANDROID) # Android
			:emphasize-lines: 4, 9, 13, 18

	Here, we include all the files needed for our project. First, we'll create our source file called ``main.cpp`` in the ``/Chapter2`` directory. All files with ``../Common/*.*`` are available to download from this tutorial website. Below are the links and discussion of their usage within this tutorial and with OpenXR. This tutorial includes all the graphics APIs header and cpp files; you only need to download the files pertaining to your graphics API choice.

	.. container:: opengles
		:name: opengles-id-3
	
		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-after: if (ANDROID) # Android
			:end-before: else() # Windows / Linux
			:dedent: 4
			:emphasize-lines: 13-22, 30
	
	.. container:: vulkan
		:name: vulkan-id-7

		.. literalinclude:: ../Chapter2/CMakeLists.txt
			:language: cmake
			:start-after: if (ANDROID) # Android
			:end-before: else() # Windows / Linux
			:dedent: 4
			:emphasize-lines: 9-11, 29
	
	Now, we can set things up by adding a shared library with the ``${SOURCES}`` and ``${HEADERS}``. We add the ``../Common`` folder as an include directory too. Just above that we need to set the ``CMAKE_SHARED_LINKER_FLAGS`` so that ``ANativeActivity_onCreate()`` is exported for the Java Virtual Machine to call. This is used by a static library called ``native_app_glue``, which allows us to interface between the Java Virtual Machine and our C++ code. Ultimately, it allows us to use the ``void android_main(struct android_app*)`` entry point.  We add ``native_app_glue`` to our project by including ``AndroidNdkModules`` and calling ``android_ndk_import_module_native_app_glue()``. 

	Next, we find the Vulkan library in the NDK and include the directory to the Android Vulkan headers. We also add a static library called ``openxr-gfxwrapper``, which will allow us to use OpenGL ES. We compile the library the C and header file in ``${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.*`` and add this ``${openxr_SOURCE_DIR}/external/include`` as an include directory. Next, we find the ``GLESv3`` and ``EGL`` libraries and link them to ``openxr-gfxwrapper``. We add the ``${openxr_SOURCE_DIR}/src/common`` and ``${openxr_SOURCE_DIR}/external/include`` folder as include directories to ``OpenXRTutorialChapter2`` as well.
	
	Finally we link the ``android``, ``native_app_glue``, ``openxr_loader``, ``vulkan`` and ``openxr-gfxwrapper`` libraries to our ``OpenXRTutorialChapter2`` library. Our ``libOpenXRTutorialChapter2 .so`` will packaged inside our .apk along with any shared libraries that we have linked.

	.. rubric:: AndroidManifest.xml

	.. literalinclude:: ../Chapter2/app/src/main/AndroidManifest.xml
		:language: xml
		:diff: ../thirdparty/AndroidDefaultApp/app/src/main/AndroidManifest.xml
	
	Download :download:`AndroidManifest.xml <../Chapter2/app/src/main/AndroidManifest.xml>`

	We need to modify our ``AndroidManifest.xml`` file to allow our application to use the XR hardware. Above is a diff between our ``AndroidManifest.xml`` and the default one. Thing to note are:

	* We add in a ``<uses-feature>`` to require OpenGL ES 3.2 and Vulkan 1.0.3 support.
	* Next, we add ``<uses-feature android:name="android.hardware.vr.headtracking" android:required="false" />`` to specify that the application works with 3DOF or 6DOF and on devices that are not all-in-ones. It's set to false so as to allow greater compatibility across devices.
	* We update the ``<application>`` section and add ``android:hasCode="false"`` as there is no Java or Kotlin code in our application.
	* We also need to modify the ``<activity>`` section to tell Android to run this application as a Native Activity. We set ``android:name`` to ``"android.app.NativeActivity"``.
	* Next, we add a ``<meta-data>`` section and add these values: ``android:name="android.app.lib_name"`` and ``android:value="OpenXRTutorialChapter2"``, where ``android:value`` is name of the library we created in the CMakeLists, thus pointing our NativeActivity to the correct library.
	* Finally, We need update the ``<intent-filter>`` to tell the application that it should take over rendering when active, rather than appearing in a window. Set ``<category android:name="org.khronos.openxr.intent.category.IMMERSIVE_HMD" />``. Note: not all devices yet support this ``<category>``. For example, for Meta Quest devices you will need ``<category android:name="com.oculus.intent.category.VR" />`` for the same purpose. The code shows both the 'Standard Khronos OpenXR' and 'Meta Quest-specific non-standard' ways of setting the ``<intent-filter>``.

	.. rubric:: Gradle

	.. literalinclude:: ../Chapter2/app/build.gradle
		:language: groovy
		:diff: ../thirdparty/AndroidDefaultApp/app/build.gradle
	
	Download :download:`app/build.gradle <../Chapter2/app/build.gradle>`

	Now, we can config our ``build.gradle`` file in the ``app`` folder to remove any references to Java, Kotlin and to testing. We also want to specify the the we are doing a ``externalNativeBuild``  with CMake, along with the CMake version and the location of the ``CMakeLists.txt`` that we created earlier. We also specify the ``ndkVersion``. Above is a diff between our ``build.gradle`` and the default one. Thing to note are:

	* We remove any references to Java, Kotlin and to testing in the ``plugins {}``, ``android { defaultConfig {} }`` sections and remove ``compileOptions {}`` and ``kotlinOptions {}`` sections entirely.
	* Update ``compileSdk``, ``minSdk`` and ``targetSdk`` as shown. We specify ``29`` for the version, but any value great than or equal to ``24`` will work.
	* We specify the ``ndkVersion`` and add in an ``externalNativeBuild {}`` under the ``android {}`` section.  This specifies that we want to do an external native build with CMake and further specifies the version and the path to our ``CMakeLists.txt``. We also add ``buildFeatures { prefab true }``. This allows the OpenXR loader AAR, specified in the ``dependencies {}`` section to work correctly.
	* In the ``dependencies {}`` section remove all the existing entries and add ``implementation 'org.khronos.openxr:openxr_loader_for_android:...'``. This provides an ``AndroidManifest.xml`` that will be merged into our own, setting some required properties for the package and application. With this though, we are still required to add to our own ``AndroidManifest.xml`` file the relevant intent filters, such as ``<category android:name="org.khronos.openxr.intent.category.IMMERSIVE_HMD" />``. It also provides the OpenXR headers and library binaries in a format that the Android Gradle Plugin will expose to CMake.

	.. literalinclude:: ../Chapter2/build.gradle
		:language: groovy
		:diff: ../thirdparty/AndroidDefaultApp/build.gradle

	Download :download:`build.gradle <../Chapter2/build.gradle>`

	Now, we can config our ``build.gradle`` file in the ``/Chapter2`` folder. Above is a diff between our ``build.gradle`` and the default one. Update the ``plugins {}`` section and add the ``task clean(type: Delete) {}`` section.
	
	In the ``settings.gradle``, ensure that ``include ':app'`` is included.

	With that completed, we should now be able to sync the Gradle file and build the project.

1.4.2 Common Files
==================

Now, that we have set up the project and source file. We will create a ``/Common`` folder with in the *workspace* directory. In that folder, we will add the following files. Below are all the files available to download:

DebugOutput
-----------
:download:`DebugOutput.h <../Common/DebugOutput.h>`

``DebugOutput`` is a class that redirects ``std::cout`` and ``std::cerr`` to the output window in your IDE.

.. container:: windows linux
	:name: windows-linux-id-2

	``DebugOutput`` inherites from ``vsBufferedStringStreamBuf``, which inherites from ``std::streambuf``. ``vsBufferedStringStreamBuf`` queues the data from the redirected ``std::streambuf`` and calls ``virtual void writeString(const std::string &)``, which ``DebugOutput`` inplements as a call to ``OutputDebugStringA()``.

.. container:: android
	:name: android-id-3

	``DebugOutput`` inherites from ``AndroidStreambuf``, which inherites from ``std::streambuf``. ``vsBufferedStringStreamBuf`` queues the data from the redirected ``std::streambuf`` and calls ``__android_log_write()`` to log the message to Android Logcat.

HelperFunctions
---------------
:download:`HelperFunctions.h <../Common/HelperFunctions.h>`

A simple header file for boilerplate code for the various platforms. It includes various C/C++ standard header, and the code that defines the macro ``DEBUG_BREAK``, according to which platform we're building for. This macro will stop execution of your program when an error occurs, so you can see where it happened and fix it. We use this macro in the ``OpenXRMessageCallbackFunction()`` function, which is discussed in detail in :ref:`Chapter 5.2<5.2 Using xrCreateDebugUtilsMessengerEXT>`.
``IsStringInVector()`` and ``BitwiseCheck()`` are just simple wrappers over commonly used code. ``IsStringInVector()`` checks if a ``const char *`` is in a ``std::vector<const char *>`` by using ``strcmp()``, and ``BitwiseCheck()`` checks if a bit is set in a bitfield.

OpenXRDebugUtils
----------------
:download:`OpenXRDebugUtils.h <../Common/OpenXRDebugUtils.h>`

:download:`OpenXRDebugUtils.cpp <../Common/OpenXRDebugUtils.cpp>`

A header and cpp file pair helps in setting up the DebugUtilsMessenger. ``XR_EXT_debug_utils`` in an OpenXR instance extension that can intercept call made to OpenXR and provide extra information or report warning and errors, if the usage of the API or the current state of OpenXR is not valid. As you go through this tutorial it is highly recommended to have this enable to help with debugging. This is discussed in detail in :ref:`Chapter 5.2<5.2 Using xrCreateDebugUtilsMessengerEXT>`, but in general ``CreateOpenXRDebugUtilsMessenger()`` creates and ``DestroyOpenXRDebugUtilsMessenger()`` destroys an ``XrDebugUtilsMessengerEXT``. ``OpenXRMessageCallbackFunction()`` is a called function specified at object creation, which is used to call when OpenXR raises an issue. The header declares the functions and the cpp defines them.

OpenXRHelper
------------
:download:`OpenXRHelper.h <../Common/OpenXRHelper.h>`

A header for including all the needed header files and helper functions.

.. literalinclude:: ../Common/OpenXRHelper.h
	:language: cpp
	:start-at: // Define any
	:end-at: #include "openxr/openxr_platform.h"
	:emphasize-lines: 5

Here, we include the main OpenXR header file ``openxr.h`` and the OpenXR platform header file ``openxr_platform.h``.
For the OpenXR platform header file, note the comment about using the preceding ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros. When enabled, we gain access to functionality that interact with the chosen graphics API and/or platform. These macros are automatically set by ``GraphicsAPI.h``

This header also defines the macro ``OPENXR_CHECK``. Many OpenXR functions return a ``XrResult``. This macro will check if the call has failed and logs a message to ``std::cerr``. This can be modified to suit your needs. There are two additional functions ``GetXRErrorString()`` and ``OpenXRDebugBreak()``, which are used to convert the ``XrResult`` to a string and as a breakpoint function respectively.


GraphicsAPI
-----------
:download:`GraphicsAPI.h <../Common/GraphicsAPI.h>`

:download:`GraphicsAPI.cpp <../Common/GraphicsAPI.cpp>`

.. container:: d3d11
	:name: d3d11-id-3

	.. rubric:: Direct3D 11

	:download:`GraphicsAPI_D3D11.h <../Common/GraphicsAPI_D3D11.h>`

	:download:`GraphicsAPI_D3D11.cpp <../Common/GraphicsAPI_D3D11.cpp>`

.. container:: d3d12
	:name: d3d12-id-3

	.. rubric:: DirectX 12

	:download:`GraphicsAPI_D3D12.h <../Common/GraphicsAPI_D3D12.h>`

	:download:`GraphicsAPI_D3D12.cpp <../Common/GraphicsAPI_D3D12.cpp>`

.. container:: opengl
	:name: opengl-id-5

	.. rubric:: OpenGL

	:download:`GraphicsAPI_OpenGL.h <../Common/GraphicsAPI_OpenGL.h>`

	:download:`GraphicsAPI_OpenGL.cpp <../Common/GraphicsAPI_OpenGL.cpp>`

.. container:: opengles
	:name: opengles-id-4

	.. rubric:: OpenGL ES

	:download:`GraphicsAPI_OpenGL_ES.h <../Common/GraphicsAPI_OpenGL_ES.h>`

	:download:`GraphicsAPI_OpenGL_ES.cpp <../Common/GraphicsAPI_OpenGL_ES.cpp>`

.. container:: vulkan
	:name: vulkan-id-8

	.. rubric:: Vulkan

	:download:`GraphicsAPI_Vulkan.h <../Common/GraphicsAPI_Vulkan.h>`

	:download:`GraphicsAPI_Vulkan.cpp <../Common/GraphicsAPI_Vulkan.cpp>`

This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. The derived class is based on your graphics API selection. Include both the header and cpp files for both ``GraphicsAPI`` and ``GraphicsAPI...``. ``GraphicsAPI.h`` includes the headers and macros needed to set up your platform and graphics API. Below are code snippets that show how to set up the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros for your platform along with any relevant headers. This tutorial demonstrates all five graphics APIs, you will only need to select one ``XR_USE_PLATFORM_...`` macro and one ``XR_USE_GRAPHICS_API_...`` macro.

The code below is an example of how you might implement the inclusion and definition the relevant graphics API header along with the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros. This will already be set up in the ``GraphicsAPI.h`` file.

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: #include "HelperFunctions
	:end-at: .h"

.. container:: windows
	:name: windows-id-3

	.. container:: d3d11
		:name: d3d11-id-4

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif
			:emphasize-lines: 5, 7

	.. container:: d3d12
		:name: d3d12-id-4

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif
			:emphasize-lines: 5, 8

	.. container:: opengl
		:name: opengl-id-6

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif
			:emphasize-lines: 5, 9

	.. container:: vulkan
		:name: vulkan-id-9

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif
			:emphasize-lines: 5, 10

.. container:: linux
	:name: linux-id-3

	.. container:: opengl
		:name: opengl-id-7

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__LINIX__)
			:end-at: #endif
			:emphasize-lines: 2-4, 6

	.. container:: vulkan
		:name: vulkan-id-10

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__LINIX__)
			:end-at: #endif
			:emphasize-lines: 2-4, 7

.. container:: android
	:name: android-id-4

	.. container:: opengles
		:name: opengles-id-5

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__ANDROID__)
			:end-at: #endif
			:emphasize-lines: 3, 5

	.. container:: vulkan
		:name: vulkan-id-11

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__ANDROID__)
			:end-at: #endif
			:emphasize-lines: 3, 6

.. container:: d3d11
	:name: d3d11-id-5

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_D3D11)
		:end-at: #endif

.. container:: d3d12
	:name: d3d12-id-5

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_D3D12)
		:end-at: #endif

.. container:: opengl
	:name: opengl-id-8

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_OPENGL)
		:end-at: #endif

.. container:: opengles
	:name: opengles-id-6

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
		:end-at: #endif

.. container:: vulkan
	:name: vulkan-id-12

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_VULKAN)
		:end-at: #endif

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: // OpenXR
	:end-at: #include "OpenXRHelper.h"

1.4.3 OpenXRTutorial and Main
=============================

We can now start adding code to our ``main.cpp`` file that we have created in the ``Chapter2/`` folder. Open ``main.cpp`` text file with Visual Studio. We will just use Visual Studio to edit the file and we will later set up the project files with CMake. 

First, we add ``DebugOutput.h``:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: #include "DebugOutput
	:end-at: .h"

Next, we add the ``GraphicsAPI_....h`` header to include the Graphics API code of your chosen graphics API. This will in turn include ``GraphicsAPI.h``, ``HelperFunctions.h``  and ``OpenXRHelper.h``. In this tutorial, we include all of them, though you would only be picking one.

.. container:: d3d11
	:name: d3d11-id-6

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 1

.. container:: d3d12
	:name: d3d12-id-6
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 2

.. container:: opengl
	:name: opengl-id-9

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 3

.. container:: opengles
	:name: opengles-id-7
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 4

.. container:: vulkan
	:name: vulkan-id-13
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 5

You can also include ``OpenXRDebugUtils.h`` to help with set up of ``XrDebugUtilsMessengerEXT``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: #include "OpenXRDebugUtils
	:end-at: .h"

Now we will define the main class ``OpenXRTutorial`` of the application. It's just a stub for now, with an empty ``Run()`` method:

.. code-block:: cpp

	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type apiType) = default;
		~OpenXRTutorial() = default;

		void Run()
		{
		}

	private:
		 bool m_applicationRunning = true;
		 bool m_sessionRunning = false;
	};

Finally, let's add the main function for the application. It will look slightly different, depending on your
chosen platform. We first create a 'pseudo-main function' called ``OpenXRTutorial_Main()``, in which we create an instance of our ``OpenXRTutorial`` class, taking a ``GraphicsAPI_Type`` parameter, and call the ``Run()`` method. ``GraphicsAPI_Type`` can be changed to suit the graphics API that you have chosen.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: void OpenXRTutorial_Main(GraphicsAPI_Type api) {
	:end-at: }

Then, we create the actual platform specific main function (our entry point to the application), which will call ``OpenXRTutorial_Main()`` with our ``GraphicsAPI_Type`` parameter. By default, this tutorial uses ``OPENGL`` as the parameter to ``OpenXRTutorial_Main()``. This must be changed to match on your chosen graphics API.

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: enum GraphicsAPI_Type : uint8_t {
	:end-at: };

.. container:: windows linux
	:name: windows-linux-id-3

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_main_WIN32___linux__
		:end-before: XR_DOCS_TAG_END_main_WIN32___linux__

.. container:: android
	:name: android-id-5
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_android_main___ANDROID__
		:end-before: XR_DOCS_TAG_END_android_main___ANDROID__

	Android requires a few extra additions to the class. Namely, ``android_app *``, ``AndroidAppState`` and ``AndroidAppHandleCmd``, which are used in getting updates from the Android Operating System and keep our application functioning.

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_Android_System_Functionality
		:end-before: XR_DOCS_TAG_END_Android_System_Functionality

	Before we can use OpenXR for Android, we need to initialise the loader based the application's context and virtual machine. We retrieve the function pointer to ``xrInitializeLoaderKHR``, and with the ``XrLoaderInitInfoAndroidKHR`` filled out call that function to initialise OpenXR for our use. At this point, we also attach the current thread to the Java Virtual Machine. We assign our ``AndroidAppState`` static member and our ``AndroidAppHandleCmd()`` static method to the ``android_app *`` and save it to a static member in the class.

1.4.4 Build and Run
===================

.. container:: windows
	:name: windows-id-4

	In the *workspace* directory, create a ``build/`` folder, which will contain our project, solution and output binary files. Now launch the CMake GUI, and point the "Where is the source code" box to the *workspace* directory, where your original ``CMakeLists.txt`` is located. Point the "Where to build the binaries" box to a subdirectory called ``build``, that we have just created. Click "Configure" and "OK" to accept the default Generator.

	You can now build and run your program. It should compile and link with no errors or warnings.

.. container:: linux
	:name: linux-id-4

	You now have the files and folders, laid out as follows:

	.. figure:: linux-vscode-initial-files.png
		:alt: Initial files in VS Code 

	Having installed the CMake extension for Visual Studio Code, you can now right-click on the main ``CMakeLists.txt`` file (the one in the root *workspace* folder). We can select “Configure and Build All” from the right-click menu of the main ``CMakeLists.txt`` file.

	.. figure:: linux-vscode-cmake-configure.png
		:alt: Select

	To enable debugging, select the Run/Debug panel in Visual Studio Code. You will now need to create a debugging configuration.
	Click the "Gear" icon to edit the file launch.json, and enter the following:

	.. code-block:: json

		{
			"version": "0.2.0",
			"configurations": [
				{
					"type": "cppdbg",
					"request": "launch",
					"name": "Chapter2",
					"program": "${workspaceFolder}/build/Chapter2/Chapter2",
					"cwd":"${workspaceFolder}/Chapter2",
					"externalConsole": true,
				}
			]
		}

.. container:: android
	:name: android-id-6

	With all the source and build systems set up, we can now build the Android project. In upper right of Android Studio, you should find the toolbar below. Click the green hammer icon to build the project, if all is successful you should see "BUILD SUCCESSFUL in [...]s" in the Build Output window.
	It is also recommended to sync the gradle files too.

	Next to the green hammer icon is the Run/Debug configuration dropdown menu. If that isn't populated, create a configuration called app.

	Turn on and connect your Android device. Set up any requirements for USB debugging and adb. You device should appear in the dropdown. Here, I am using a Oculus Quest 2.

	.. figure:: android-studio-build-run-toolbar.png
		:alt: Build/Run Toolbar
	
	To debug/run the application click the green bug icon.

Now that we have a basic application up and running with the OpenXR header files and libraries, we can start to set up the core aspects of OpenXR. As a modern Khronos API, the OpenXR is heavily influcencd by the Vulkan API. So those who are familiar with the style of the Vulkan API will find OpenXR easy to follow.

