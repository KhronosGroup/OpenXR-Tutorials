OpenXR Tutorial
===============

This tutorial will teach you how to use the OpenXR API. OpenXR is an API managed by the Khronos Group which provides a cross-platform way for applications to interact with immersive devices. This includes virtual reality (VR) headsets, augmented reality (AR) devices, motion controllers and more.

An API is an application programming interface, a set of rules for how a software system can be interfaced-with by other systems. There is no specific library or code that is OpenXR, but rather there are multiple implementations (or runtimes), which might be provided by hardware manufacturers or by software companies; there are even open source runtimes for OpenXR. Whichever runtime you use, if it's compliant with the OpenXR specification, your OpenXR-compliant code will work with it.

This tutorial introduces programming with OpenXR. If you use a game engine such as Unreal Engine, Unity or Godot, you may not need to interact directly with OpenXR: the engine abstracts this away. However, it may be beneficial to learn the underlying concepts.

OpenXR is designed to work with various different graphics APIs: Vulkan, Direct3D 11, Direct3D 12, OpenGL and OpenGL ES are all supported at present. You will need some familiarity with your graphics API of choice to get the most out of this tutorial.

Other prerequisites are:

* A Windows or Linux PC to compile the code.
* An IDE (integrated development environment) that supports your target device: either Visual Studio 2017 or later; or Android Studio.
* Some XR hardware to run on: either an XR device that connects to your PC, or an Android-based device like a Meta Quest or Pico Neo. It is also possible to use `Monado <https://monado.freedesktop.org/>`_ to emulate XR hardware.
* CMake 3.22.1 is required.
* Python 3.6+ is required to build the OpenXR-SDK-Source repository. See `OpenXR-SDK-Source/BUILDING.md <https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/main/BUILDING.md>`_.

Tutorial Structure
------------------

The tutorial is divided into several main chapters. Each chapter walks you through a part of the process of creating an XR application and assumes that the previous chapters have been completed.

Explantions of key concepts are given throughout the chapters to provide deeper insight into the workings of OpenXR. The code snippets can be copied directly into your project as you follow the tutorial, and explantions for the code are generally given after the code snippets.

At the end of each chapter there is a link to download the full example source code up to that point.

The full source code for this tutorial can also be downloaded from `https://github.com/KhronosGroup/OpenXR-Tutorials <https://github.com/KhronosGroup/OpenXR-Tutorials>`_.

* In the Introduction, we describe the basic concepts underpinning OpenXR, and show you how to set up a PC or Android development environment to build the code.
* In Setup, we show how to initialize OpenXR and connect your program to your XR device.
* In Graphics, we setup the swapchains and render graphics to the views.
* In Actions, we use OpenXR's action system to bind input to application specific actions allowing the user to interactivity.
* In Extensions, we demonstrate various extension that you may find helpful.

.. toctree::
	:maxdepth: 5
	:caption: Contents:

	1-introduction
	2-setup
	3-graphics
	4-actions
	5-extensions
	6-next-steps