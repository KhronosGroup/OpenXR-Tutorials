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
Separate tabs for Windows GL, WIndows D3D12, Windows Vulkan etc.
Where/how to get the SDK: FetchContent, say what it does.
What dependencies are required, and where to get them. Again, highly platform- and API-dependent. Refer to FetchContent where appropriate, not too much detail.
Introduce CMake and briefly explain its use.
For Android, discuss gradle files, their structure and links to CMake.

## Chapter 1.4 Project Setup
Boilerplate code around including openxr headers, creating an example app class object "OpenXRTutorial". We will describe at the top the outline of what will be done in this section.
Details on creating CMake projects and building on PC/Linux or building for Android?
Possibly separate Ch2 into two chapters, one for setting up the project, one for initiating OpenXR? // Resolve by moving the head of Chapter 2 to Chapter 1.4.

## Chapter 2
The goal is to create an XrInstance and an XrSession and setup the event loop. Discuss the purpose of this setup OpenXR code.

## Chapter 2.1 Creating an XrInstance
We will briefly introduce XrInstance, its purpose and scope/lifetime. Discuss XrApplicationInfo and API versioning. Some mention of extensions, and description of what instance extensions we will be using (some platform/API-specific extensions here).
Describe extension enumeration, and show code to do this.
Ensure safe shutdown/fallout when necessary extensions are not present.
Describe XrInstanceCreateInfo and its members. Show code to set this struct up and call xrCreateInstance.
(Briefly describe instance destruction and show code for this.)
Show how to get instance properties.
Discuss XrSystemId and its uses. Show code to obtain the system id.

## Chapter 2.2 Creating an XrSession
Introduce XrSession and explain its purpose.
Code sample showing XrSessionCreateInfo and xrCreateSession().
Need to present all different types the Graphics API structures the interface with XrSessionCreateInfo. Explain the use of next pointer.
Describe briefly Destroying an XrSession.

## Chapter 2.3 Polling the Event Loop
Introduce xrPollEvent() and explain that the function should be called each frame.
Describe lifetime of an application and what XrSessionStates are and how to react to the them.
Describe briefly Beginning and Ending an XrSession.
Explain that events are queued, so you'd drain the queue and that the .type will be change to XR_TYPE_EVENT_DATA_...
Code sample showing the function in use and the casting of structures depending on the type.
Describe the XrEventDataEventsLost, XrEventDataInstanceLossPending, XrEventDataInteractionProfileChanged, XrEventDataReferenceSpaceChangePending, XrEventDataSessionStateChanged.
Getting the new XrSessionState from XrEventDataSessionStateChanged - code.

## Chapter 3
The goal is to create and clear colour and depth buffers within the scope of OpenXR render loop and its interaction with all the Graphics APIs.

## Chapter 3.1 Creating Swapchains
Introduce XrViewConfigurationView and explain various types of XR systems and their possible view configurations and how to use xrEnumerateViewConfigurationViews() to get the number of view in the system - there's not always 2 views.
Use xrEnumerateSwapchainFormats() to find a compatible format that is suitable for our rendering. Discuss the Graphics APIs's preference for a specific format and the subtetlies of usage, and the general usage of sRGB and linear formats.
Describe the XrSwapchainCreateInfo struct and the usage flags. Use xrCreateSwapchain() to create images that are layered for easier handling of resources. Use xrEnumerateSwapchainImages() to get the resources for rendering and create layered depth textures (one per eye/view). The layered images are easier to integrate with Multiview and allow binding 2D Array images in the colour blend/output merger stage.

## Chapter 3.2 Building a RenderLoop
Describe the idea of an OpenXR frame and show the usage of xrWaitFrame(), xrBeginFrame() and xrEndFrame().
Discuss the use of multiple layer and XR compositor and the compositing modes, which can be queried through xrEnumerateEnvironmentBlendModes().
Describe the usage of xrAcquireSwapchainImage(), xrWaitSwapchainImage() and xrReleaseSwapchainImage() in rendering to one of the images in swapchain.
Explain and show code examples for Multiview/ViewInstancing rendering in all APIs (except D3D11), otherwise use normal instanced rendering (This might be in Chapter 4).
Show calling the Graphics APIs's 'Clear Image/ImageView' function and providing the image back to the swapchain, ending the OpenXR frame for the compositor and runtime to display the image to the screen(s).

# Chapter 4: ACTIONS

NOTE: we've swapped part 4.2 and 4.3, as it makes sense to introduce actions
before we can speak of binding them.

General description of Actions as a concept in OpenXR.

## Chapter 4.1 Spaces
Introduce the concept of Spaces, their purpose and their lifetime.
Describe the types of space: view, local, (important) Stage Space.
Example code to create a local space for hands, xrLocateSpace. tracking.

## Chapter 4.2 Interactions
Introduce the concept of Interactions, interaction profiles and bindings.
Example code creates the actions/interactions, particular attention to where in the lifecycle of the application this is done.
Possibly refer to external resources on interaction bindings, or include our table (https://docs.google.com/spreadsheets/d/1w4Me9_yG_TNho4Gmelrc9ILYtG1TELoxCXc622xUiVM/edit#gid=1366774089) as a link or appendix.
## Chapter 4.3 Creating actions/actionSets.
Describe types of action: floats, vectors, booleans, poses. Example code for at least poses and booleans. link hand pose to cube position.
## Chapter 4.4 Checking if a controller is connected.
Example code of controller detection and fallback behaviour.

# Chapter 5: EXTENSIONS

## Chapter 5.1 Using Extensions and Layers
Discuss the reasoning of having an extensible and layered API, i.e. more portability/platform options, future extensibilities, vendor hardware/software support, API debugging and captures files etc. 
Show the usage of xrEnumerateApiLayerProperties() and xrEnumerateInstanceExtensionProperties(). Show loading in function pointers with xrGetInstanceProcAddr() for functions that are not automatically loaded by the Loader - some function don't require an XrInstance.
Link to OpenXR specification extension section for research and reference documentation on extension and layers.

## Chapter 5.2 Using xrCreateDebugUtilsMessengerEXT
Introduce the issue that Debug Utils Messenger addresses and describe the extension.
Show code checks for the extension in the submitted extension list when we created the XrInstance.
Discuss loading the function pointers from the instance with xrGetInstanceProcAddr() (a quick recap from Chapter 5.1) in order to create and destroy the XrDebugUtilsMessenger.
Use XrDebugUtilsMessengerCreateInfoEXT to create the XrDebugUtilsMessenger. Discuss the PFN_xrDebugUtilsMessengerCallbackEXT callback function. Show a suitable function and discuss the requirements of the return value and the usage of the parameters.