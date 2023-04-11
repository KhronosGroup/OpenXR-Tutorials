OpenXR Tutorial
===============

This tutorial will teach you how to use the OpenXR API.
OpenXR is an API managed by the Khronos Group which provides a cross-platform
way for applications to interact with immersive devices. This includes virtual reality (VR)
headsets, augmented reality (AR) devices, motion controllers and more.

An API is an application programming interface, a set of rules for how a software system
can be interfaced-with by other systems. There is no specific library or code that is OpenXR,
rather, there are multiple implementations (or runtimes), which might be provided by hardware
manufacturers, by software companies. There are even open source runtimes. But whichever
runtime you use, if it's compliant with the OpenXR standard, your OpenXR-compliant code will
work with it.

This tutorial introduces programming with OpenXR. If you use a game engine such as Unreal Engine, Unity or Godot,
you may not need to interact directly with OpenXR: the engine abstracts this away. It may be benificial
to learn the underlying concepts however.

OpenXR is designed to work with various different graphics API's: Vulkan, Direct3D 11 and 12,
OpenGL and OpenGL ES are all supported at present. You will need some familiarity with your graphics
API of choice to get the most out of this tutorial.

Other prerequisites are:

* A Windows or Linux PC to compile the code
* An IDE (integrated development environment) that supports your target device: either Visual Studio 2017 or later; or Android Studio.
* Some XR hardware to run on: either an XR device that connects to your PC, or an Android-based device like a Meta Quest or Pico Neo.

In the latter case, it is possible to use Monado to emulate XR hardware - see Section x.x.

Tutorial Structure
------------------

The tutorial is divided into several main sections. Each section walks you through
part of the process of creating an XR program, and assumes that the previous secions have
been completed.

At the end of each section is a link to download the full example source code up to that point.

This code can also be downloaded from the following repository: https://github.com/simul/openxr-tutorial-src

In the Introduction, we describe the basic concepts underpinning OpenXR, and show you how to
set up a PC or Android development environment to build the code.

Then in Setup, we show how to initialize OpenXR and connect your program to your XR
device.


.. toctree::
	:numbered:
	:maxdepth: 4
	:caption: Contents:

	introduction
	setup
	graphics
	actions
	extensions
