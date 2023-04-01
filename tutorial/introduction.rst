############
Introduction
############

1. INTRO (understanding & dev env)

**************
Goal of OpenXR
**************

What is the goal of OpenXR?

********
Overview
********

1.2. High Level overview of the components of OpenXR (Application - Loader - API
layers - Runtime - Graphics - Input System)


**********
Setting Up
**********

This section will help you set up a development environment.
( Visual Studio & Android Studio & Monado for people without HMDs )

PCVR: Windows
~~~~~~~~~~~~~
.. rubric:: Visual Studio
If you'll be building an OpenXR project for PC-based devices, we'll assume you'll be using Microsoft Visual Studio.
The free Community edition of Visual Studio is available `here <https://visualstudio.microsoft.com/vs/community/>`_.

.. rubric:: Direct3D
Next you'll want to choose which Graphics API you'll be using. Direct3D 11 and 12 are built into the Windows SDK's.
If you're using Direct3D, I recommend D3D12, because it supports indexed views: essentially, you can draw both eye views in one call. Vulkan also
supports this.

Whether D3D11 or 12, download and install the latest `Windows SDK <https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/>`_.

.. rubric:: Vulkan
If you'll be using Vulkan, download and install the latest `Vulkan SDK <https://www.lunarg.com/vulkan-sdk/>`_. Take a note of the installation location,
as this will be needed later.

.. rubric:: OpenXR SDK
You'll need a copy of the OpenXR SDK, which is distributed as a Git repository. If you're familiar with Git, you can use your preferred command-line or GUI Git tool to get
the SDK from *git@github.com:KhronosGroup/OpenXR-SDK.git*.
Alternatively, you can download it as a .zip file from `github.com/KhronosGroup/OpenXR-SDK <https://github.com/KhronosGroup/OpenXR-SDK>`_.

.. rubric:: CMake
The OpenXR SDK is distributed as source code, and you'll need to build it, using CMake.
CMake is a program that generates project files, such as the .vcxproj and .sln files
that Visual Studio needs.
Install the latest `CMake <https://cmake.org/download/>`_.

Once CMake is installed, click the Windows button, and type "cmake" to find the application Cmake GUI.

.. image:: find_cmake.png
   :alt: Find CMake by clicking the Windows icon and typing "cmake".
   :align: right

In the CMake Gui, enter the location of the OpenXR SDK in the top text box, marked "Where is the source code". Below, in the box marked "Where to
build the binaries", enter a subdirectory of that folder, called "build".

.. image:: cmake-openxrsdk-1.png
   :alt: an image of the CMake GUI, showing that the location of the OpenXR SDK has been entered as the source directory, and that a subdirectory of this, called "build" has been entered as the binary directory.
   :align: right

Click the button below marked "Configure". A popup will ask if CMake should create the "build" directory. Click "Yes".
Now a box will appear where you can select a "Generator". This should default to the version of
Visual Studio you've just installed. Click Finish.
Cmake will now configure the project, and a list of variables will appear in red.

.. image:: cmake-openxrsdk-2.png
   :alt: alternate text
   :align: right

You shouldn't have to change any of these variables, but you can if you want. CMake should have
automatically found the installation of the Vulkan SDK that you created previously, and used it to fill in the variables
Vulkan_INCLUDE_DIR, Vulkan_LIBRARY and so on. If not, you can modify these variables
by clicking on the "Value" entries. The values should look like this:


.. image:: cmake-vulkan-vars.png
   :alt: The Vulkan variables in CMake GUI should read:    Vulkan_GLSLANG_VALIDATOR_EXECUTABLE C:/VulkanSDK/1.3.239.0/Bin/glslangValidator.exe    Vulkan_GLSLC_EXECUTABLE C:/VulkanSDK/1.3.239.0/Bin/glslc.exe    Vulkan_INCLUDE_DIR C:/VulkanSDK/1.3.239.0/Include    Vulkan_LIBRARY    C:/VulkanSDK/1.3.239.0/Lib/vulkan-1.lib
   :align: right

Obviously, the precise directory will depend on which version of the Vulkan SDK you installed.

Now, we're going to "Generate" the project files for the OpenXR SDK. Click "Generate", and CMake GUI should
now read "Generating done" in the output log at the bottom.

.. image:: cmake-openxrsdk-generate.png
   :alt: The Vulkan variables in CMake GUI should read:    Vulkan_GLSLANG_VALIDATOR_EXECUTABLE C:/VulkanSDK/1.3.239.0/Bin/glslangValidator.exe    Vulkan_GLSLC_EXECUTABLE C:/VulkanSDK/1.3.239.0/Bin/glslc.exe    Vulkan_INCLUDE_DIR C:/VulkanSDK/1.3.239.0/Include    Vulkan_LIBRARY    C:/VulkanSDK/1.3.239.0/Lib/vulkan-1.lib
   :align: right

Now we can build the SDK. Click "Open Project" in CMake GUI, or navigate to your "build" folder and double-click on
the file "OPENXR.sln", and Visual Studio will open the solution.

Select "Debug" from the Visual Studio "Solution Configurations" dropdown.

.. image:: visual-studio-openxr-debug.png
   :alt: In Visual Studio, the Solution Configuration dropdown menu is shown, with "Debug" selected.
   :align: right

Go to the Build menu and select "Build Solution". The projects will be compiled, and the output
from openxr_loader.vcxproj should be a library called openxr_loaderd.lib.

.. image:: visual-studio-openxr-build.png
   :alt: In Visual Studio, the "Build" menu is shown, with the "Build Solution" option selected.
   :align: right

Now repeat the process for the Release build. Select "Release" from the Configurations dropdown,
amd build the solution. This time, the library openxr_loader.lib will be built.

You can now close this solution, you're ready to start creating your first OpenXR project.


PCVR: Linux
~~~~~~~~~~~
.. rubric:: Visual Studio Code

...

.. rubric::  OpenXR SDK
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

This builds libopenxr_loader.so, in Debug and Release flavours, in the directories:
OpenXR-SDK/build/linux_debug/src/loader and OpenXR-SDK/build/linux_release/src/loader.
Both files have the same name on Linux - the debug one is substantially larger however.
And both are *dynamic* libraries by default, which will be loaded at runtime.

Now you're ready to start creating your first OpenXR project.
