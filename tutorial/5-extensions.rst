############
5 Extensions
############

OpenXR is designed to be an extensible API. As we've seen above, the call to xrCreateInstance() can include one or more extension names, and we can query xrEnumerateInstanceExtensionProperties() in advance to find out which extensions are supported by the runtime. In this section, we'll see how extensions add to the core API and look at a specific case, the hand tracking extension.

*****************
5.1 Hand Tracking
*****************

Many XR devices now support hand-tracking. Instead of a motion-tracked controller, one or more cameras take images of the surrounding area. If your hands are visible to the cameras, algorithms in or accessible to the runtime try to calculate the positions of your hands and fingers. We'll now enable hand tracking in your project.

.. container:: android

	We'll edit your app/src/main/AndroidManifest.xml to enable the hand tracking feature. Add these lines to the <manifest> block:
	
	.. code-block:: xml

		<uses-permission android:name="com.oculus.permission.HAND_TRACKING" />
		<uses-feature android:name="oculus.software.handtracking" android:required="false" />

	Add this line to the <application> block:
	
	.. code-block:: xml

		<meta-data android:name="com.oculus.handtracking.frequency" android:value="HIGH"/>
	
We saw in :ref:`Chapter 2 <instanceextensions>`  how to create a list of instance extensions before starting up the OpenXR instance. At the top of CreateInstance(), where we're listing the extensions to request, we'll add the name of the one that enables hand tracking:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_instanceExtensions
	:end-before: XR_DOCS_TAG_END_instanceExtensions
	:dedent: 12

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_handTrackingExtensions
	:end-before: XR_DOCS_TAG_END_handTrackingExtensions
	:dedent: 12

It happens that XR_EXT_HAND_TRACKING is an official extension, so its name and function prototypes are provided in openxr.h. This won't be the case for every extension!

At the start of your main.cpp, underneath where you included "OpenXRDebugUtils.h", add these declarations:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DeclareExtensionFunctions
	:end-before: XR_DOCS_TAG_END_DeclareExtensionFunctions
	:dedent: 0

openxr.h has prototyped these functions for us, but we'll need to get the function pointers at runtime, so at the end of CreateInstance(), add this:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_ExtensionFunctions
	:end-before: XR_DOCS_TAG_END_ExtensionFunctions
	:dedent: 8

These calls require an XrInstance, so we must initialize these after XrCreateInstance() has successfully returned. If hand tracking is not supported (or not enabled), we'll get a warning, and these functions will be null. You can run your app now to check this.

At the end of your OpenXRTutorial application class, declare the following:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_HandTracking
	:end-before: XR_DOCS_TAG_END_HandTracking
	:dedent: 4

`Hand` is a simple struct to encapsulate the XrHandTrackerEXT object for each hand, and the joint location structures that we'll update to track hand motion. Now, in Run(), after the call to AttachActionSet(), add:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallCreateHandTracker
	:end-before: XR_DOCS_TAG_END_CallCreateHandTracker
	:dedent: 1

Add this function after the definition of AttachActionSet(). For each of two hands, we'll call xrCreateHandTrackerEXT() and fill in the m_handTracker object.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateHandTracker
	:end-before: XR_DOCS_TAG_END_CreateHandTracker
	:dedent: 4

The XrHandTrackerEXT object is a session-lifetime object, so in DestroySession(), at the top of the function we'll add:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroyHandTracker
	:end-before: XR_DOCS_TAG_END_DestroyHandTracker
	:dedent: 8

Hands should be polled once per frame. At the end of PollActions(), we'll poll each hand. We'll assume to begin with that the user isn't holding the controller, so we'll use the XR_HAND_JOINTS_MOTION_RANGE_UNOBSTRUCTED_EXT motion range.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollHands
	:end-before: XR_DOCS_TAG_END_PollHands
	:dedent: 8

Finally, we'll render the hands simply by drawing a cuboid at each of the 26 joints of each hand. Where `numberOfCuboids` is defined, add this to make sure we have enough space in the constant buffers:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_AddHandCuboids
	:end-before: XR_DOCS_TAG_END_AddHandCuboids
	:dedent: 8

Now in RenderLayer(), just before the call to `m_graphicsAPI->EndRendering()`, add this so we render both hands, with all their joints:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderHands
	:end-before: XR_DOCS_TAG_END_RenderHands
	:dedent: 12

Run the app: you'll now see both hands, rendered as blocks.

***************************
5.2 Composition Layer Depth
***************************

Composition Layer Depth is truly important for AR use cases as it allows applications to submit to the runtime a depth image that can be used in the accurate compostion of rendered graphics with in the real world. Without a submitted depth image, runtimes would be unable to composite rendered objects that are occluded by real world objects e.g. a rendering a cube partially occluded by a door frame.

This functionality is provided to OpenXR via the use of the XR_KHR_composition_layer_depth extension (`OpenXR Specification 12.8. XR_KHR_composition_layer_depth <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_KHR_composition_layer_depth>`_). The extension allows depth images from a swapchain to be submitted alongside the color images. This extension works in conjunction with projection layer type. 

The ``XrCompositionLayerProjection`` structure contains a pointer an array of ``XrCompositionLayerProjectionView`` structures. Each of these structures refer to a single view in the XR system and a single image subresource from a swapchain. To submit a depth image, we employ the use of a ``XrCompositionLayerDepthInfoKHR`` structure. Like with ``XrCompositionLayerProjectionView``, ``XrCompositionLayerDepthInfoKHR`` refers to a single view in the XR system and a single image subresource from a swapchain. These structures are 'chained' together via the use of the ``const void* next`` member in ``XrCompositionLayerProjectionView``. We assigned the memory address of  a ``XrCompositionLayerDepthInfoKHR`` structure that we want to chain together. The runtime time will read the ``next`` pointer and associate the structure and ultimately the color and depth images togehter for compositing. This is same style of extensiblity used in the Vulkan API.

One thing is now very clear to the programmer - we need a depth swapchain! Seldom used in windowed graphics, but required here to allow smooth rendering of the depth image and not lock either runtime or application waiting on a single depth image to be passed back and forth between the two. In most windowed graphics application, the depth image is discarded at the end of the frame and doesn't interact with the windowing system at all.

When creating a depth swapchain, we must check that the system supports a depth format for swapchain creation. You can check this with ``xrEnumerateSwapchainImages()``. Unfortunately, there are no guarantees with in the OpenXR 1.0 core specification or the XR_KHR_composition_layer_depth extension revision 6 that runtimes must support depth format for swapchains.

To implement this in the Chapter 5 code, we first add the following memeber to the ``RenderLayerInfo`` structure:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderLayer_LayerDepthInfos
	:end-before: XR_DOCS_TAG_END_RenderLayer_LayerDepthInfos
	:dedent: 8

Now, in the ``CreateInstance()`` under the extensions from Chapter 2:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_instanceExtensions
	:end-before: XR_DOCS_TAG_END_instanceExtensions
	:dedent: 12

We add this code:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CompositionLayerDepthExtensions
	:end-before: XR_DOCS_TAG_END_CompositionLayerDepthExtensions
	:dedent: 12

This enables the XR_KHR_composition_layer_depth extension for us.

In ``RenderLayer()`` after we've resized the ``std::vector<XrCompositionLayerProjectionView>``, we also resize the ``std::vector<XrCompositionLayerDepthInfoKHR>``, both of which are found in the ``RenderLayerInfo`` parameter.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_ResizeLeyerDepthInfos
	:end-before: XR_DOCS_TAG_END_ResizeLeyerDepthInfos
	:dedent: 8

After we have filled out the ``XrCompositionLayerProjectionView`` structure, we fill out the ``XrCompositionLayerDepthInfoKHR`` structure and using the ``XrCompositionLayerProjectionView::next`` pointer we chain the two structures together. This submits the depth and the color image together for the XR compositor.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SetupLeyerDepthInfos
	:end-before: XR_DOCS_TAG_END_SetupLeyerDepthInfos
	:dedent: 8

*****************
5.3 XR_API_LAYERS
*****************

The OpenXR loader has a layer system that allows the OpenXR API calls to pass through a number of optional layers, that add some functionality for the application. These are exceptionally useful for debugging.

The OpenXR SDK provides us two API layers for us to use:
There's the layer name and its associated library and json file.

+------------------------------------+-----------------------------------------------+-------------------------------------+
| XR_APILAYER_LUNARG_api_dump        | ``XrApiLayer_api_dump.dll`` or ``.so``        | ``XrApiLayer_api_dump.json``        |
+------------------------------------+-----------------------------------------------+-------------------------------------+
| XR_APILAYER_LUNARG_core_validation | ``XrApiLayer_core_validation.dll`` or ``.so`` | ``XrApiLayer_core_validation.json`` |
+------------------------------------+-----------------------------------------------+-------------------------------------+

XR_APILAYER_LUNARG_api_dump simply logs extra/verbose information to ``std::cout`` descibing in more detail what has happened during that API call. XR_APILAYER_LUNARG_core_validation acts similarly to VK_LAYER_KHRONOS_validation in Vulkan, where the layer intercepts the API call and performance validation to ensure conformance with the specification.

Other runtimes and hardware vendor may provide layers that are useful for debugging your XR system.

To enable api layers, add the ``XR_API_LAYER_PATH=<path>`` environment variable to your project or your system. Something like this: ``XR_API_LAYER_PATH=<openxr_base>/<build_folder>/src/api_layers/;<another_path>``. 

The path must point a folder containing a ``.json`` file similar to the one for XR_APILAYER_LUNARG_core_validation, shown below:

.. code-block::json
	{
		"file_format_version": "1.0.0",
		"api_layer": {
			"name": "XR_APILAYER_LUNARG_core_validation",
			"library_path": "./XrApiLayer_core_validation.dll",
			"api_version": "1.0",
			"implementation_version": "1",
			"description": "API Layer to perform validation of api calls and parameters as they occur"
		}
	}

To select which API Layers we want to use, there are two ways to do this:
 1. Add the ``XR_ENABLE_API_LAYERS=<layer_name>`` environment variable to your project or your system. Something like this: ``XR_ENABLE_API_LAYERS=XR_APILAYER_LUNARG_test1;XR_APILAYER_LUNARG_test2``.
 2. When creating the ``XrInstance``, specify the requested API layers in the ``XrInstanceCreateInfo`` struct.

Calls to ``xrEnumerateApiLayerProperties()`` should now return the count and data of all API layers available to the application.

For more details, please see `API Layers README <https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/main/src/api_layers/README.md>`_ and see `OpenXR API Layers <https://registry.khronos.org/OpenXR/specs/1.0/loader.html#openxr-api-layers>`_.