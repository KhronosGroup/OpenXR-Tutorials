Outline
=======

## Chapter 1.1 What is the goal of OpenXR

Introduce OpenXR to a lay technical audience.
OpenXR is a royalty-free, open standard developed and maintained by the Khronos Group to enable the easy creation of XR applications across a variety of vendor-specific hardware and XR runtimes. XR, Virtual (V) and Augmented (A) reality, encompasses the two major formats of interactive and immersive graphics applications that incorporate visual input and/or actions from/within the physical world.
Reference the OpenXR 1.0 specification, its history and goal of creating a unified programming approach for XR developers, discuss how it addresses the issue of software fragmentation.
Describe how the unification of the XR application interface removed the need for developers to support multiple vendor-specific APIs, which at the time increased costs, complexity and the time to test applications and thus restricted the ability of developers to port applications to other XR systems. Show how the unified API can target multiple XR runtimes and a multitude of XR systems and Graphics APIs.
Some use of diagrams here will help.

## Chapter 1.2 High Level overview of the components of OpenXR (Application - Loader - API layers - Runtime - Graphics - Input System)
We will list the OpenXR components that combine to create the foundation of an XR experience. Some mention of the heritage of OpenXR, as OpenXR takes a some of its lexicon and API style from Vulkan; description of its use in providing a clear and precise common language for developers and hardware vendors to use.
Introduce the Application in terms of how OpenXR uses the term.
Introduce concept of the Loader and describe some aspects and implications.
Discuss API layers, their use in extensibility.
Describe the concept of the Runtime, mention different runtimes and where they are found.
Discuss how graphics are enabled by Instance extensions, how OpenXR supports various Graphics APIs with support for future ones too.
Introduce the Input System, show how it enables extensibility, cross-device applications.

Again, appropriate diagrams, matching site colour scheme and design.

## 1.3

Rename 1.3 to "Setting up a Development Environment"? Move to Chapter 2?

## Chapter 2

There is substantial "Setup" that we expect to be needed before "Creating an XrInstance". Perhaps a whole subsection 2.1 on creating CMake projects and building on PC/Linux or building for Android?

Possibly separate Ch2 into two chapters, one for setting up the project, one for initiating OpenXR?

## Chapter 2.1 Creating an XRInstance

Boilerplate code around including openxr headers, creating an example app class object "OpenXRTutorial". We will describe at the top the outline of what will be done in this section.
We will briefly introduce XrInstance, its purpose and scope/lifetime. Discuss XrApplicationInfo and API versioning. Some mention of extensions, and description of what instance extensions we will be using (some platform/API-specific extensions here).
Desribe extension enumeration, and show code to do this.
Ensure safe shutdown/fallout when necessary extensions are not present.
Describe XrInstanceCreateInfo and its members. Show code to set this struct up and call xrCreateInstance.
(Briefly describe instance destruction and show code for this.)
Show how to get instance properties.
Discuss XrSystemId and its uses. Show code to obtain the system id.