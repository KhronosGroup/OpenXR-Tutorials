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

## Chapter 1.3 Setting Up
Rename 1.3 to "Setting up a Development Environment"? Move to Chapter 2?
Highly platform-dependent. Also API-dependent.
Perhaps: separate tabs for Windows GL, WIndows D3D12, Windows Vulkan etc.
Where to get the SDK.
What dependencies are required, and where to get them. Again, highly platform- and API-dependent.
Introduce CMake and briefly explain its use.
For Android, discuss gradle files.

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

## Chapter 2.2 Creating an XrSession
Introduce XrSession and explain its purpose.
Code sample showing XrSessionCreateInfo and xrCreateSession().
Need to present all different types the Graphics API structures the interface with XrSessionCreateInfo. Explain the use of next pointer.
Describe lifetime of an application and what XrSessionStates are and how to react to the them.
Describe briefly Beginning, Ending and Destroying an XrSession.

## Chapter 2.3 Polling the Event Loop
Introduce xrPollEvent() and explain that the function should be called each frame.
Explain that events are queued, so you'd drain the queue and that the .type will be change to XR_TYPE_EVENT_DATA_...
Code sample showing the function in use and the casting of structures depending on the type.
Describe the XrEventDataEventsLost, XrEventDataInstanceLossPending, XrEventDataInteractionProfileChanged, XrEventDataReferenceSpaceChangePending, XrEventDataSessionStateChanged.
Getting the new SxSession from XrEventDataSessionStateChanged - code.