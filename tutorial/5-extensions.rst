############
5 Extensions
############

OpenXR is designed to be an extensible API. As we've seen before, the call to ``xrCreateInstance()`` can include one or more extension names, and we can query them by using ``xrEnumerateInstanceExtensionProperties()`` to find out which extensions are supported by the runtime. 

All functions, types and constants for an extension are found in OpenXR headers. There are two ways to access the function(s) relating to an extension: functions pointers and function prototypes. 

* The first one is more portable, as at runtime the application queries and loads the function pointers with ``xrGetInstanceProcAddr()``.

* The second requires build time linking to resolve the symbols created by the declared function prototypes. 

In this chapter, we will load the functions pointers. We'll see how extensions add to the core OpenXR API and look at specific cases: hand tracking and compostion layer depth.

*****************
5.1 Hand Tracking
*****************

Many XR devices now support hand-tracking. Instead of a motion-tracked controller, one or more cameras take images of the surrounding area. If your hands are visible to the cameras, algorithms in or accessible to the runtime try to calculate the positions of your hands and digits. We'll now enable hand tracking in your project.

**Remember to enable hand tracking in your XR runtime and/or XR system settings.**

.. container:: android

	We'll edit your ``app/src/main/AndroidManifest.xml`` to enable the hand tracking feature. Add these lines to the ``<manifest>`` block:
	
	.. code-block:: xml

		<uses-permission android:name="com.oculus.permission.HAND_TRACKING" />
		<uses-feature android:name="oculus.software.handtracking" android:required="false" />

	Add this line to the ``<application>`` block:
	
	.. code-block:: xml

		<meta-data android:name="com.oculus.handtracking.frequency" android:value="HIGH"/>
	
We saw in :ref:`Chapter 2 <instanceextensions>` how to create a list of instance extensions before creating the OpenXR instance. At the top of ``CreateInstance()``, where we're listing the extensions to request, we'll add the name of the one that enables hand tracking:

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

XR_EXT_hand_tracking and XR_EXT_hand_interaction are official multivendor extensions. Being multivendor extensions means that they are likely well supported across vendors, but they are not currently Khronos-approved extensions. All functions, types and constants in this extension will have the `EXT`` label somewhere in their name.

`OpenXR Specification 12.31. XR_EXT_hand_tracking <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_EXT_hand_tracking>`_.

`OpenXR Specification 12.29. XR_EXT_hand_interaction <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_EXT_hand_interaction>`_.

At the start of your ``main.cpp`` file, underneath where you included ``"OpenXRDebugUtils.h"``, add these declarations:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DeclareExtensionFunctions
	:end-before: XR_DOCS_TAG_END_DeclareExtensionFunctions
	:dedent: 0

Next, we'll need to get the function pointers at runtime, so at the end of ``CreateInstance()``, add this:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_ExtensionFunctions
	:end-before: XR_DOCS_TAG_END_ExtensionFunctions
	:dedent: 8

These calls require an ``XrInstance``, so we must initialize these after ``XrCreateInstance()`` has successfully returned. If hand tracking is not supported (or not enabled), we'll get a warning, and these functions will be ``nullptr``. You can run your application now to check this.

At the end of your ``OpenXRTutorial`` application class, declare the following:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_HandTracking
	:end-before: XR_DOCS_TAG_END_HandTracking
	:dedent: 4

``Hand`` is a simple struct to encapsulate the ``XrHandTrackerEXT`` object for each hand, and the joint location structures that we'll update to track hand motion. Now, in ``Run()``, after the call to ``AttachActionSet()``, add:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallCreateHandTracker
	:end-before: XR_DOCS_TAG_END_CallCreateHandTracker
	:dedent: 1

Add this method after the definition of ``AttachActionSet()``. For each of two hands, we'll call ``xrCreateHandTrackerEXT()`` and fill in the ``m_handTracker`` object.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateHandTracker
	:end-before: XR_DOCS_TAG_END_CreateHandTracker
	:dedent: 4

The ``XrHandTrackerEXT`` object is a session-lifetime object, so in ``DestroySession()``, at the top of the method we'll add:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroyHandTracker
	:end-before: XR_DOCS_TAG_END_DestroyHandTracker
	:dedent: 8

Hands should be polled once per frame. At the end of ``PollActions()``, we'll poll each hand. We'll assume to begin with that the user isn't holding the controller, so we'll use the ``XR_HAND_JOINTS_MOTION_RANGE_UNOBSTRUCTED_EXT`` motion range.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollHands
	:end-before: XR_DOCS_TAG_END_PollHands
	:dedent: 8

Finally, we'll render the hands simply by drawing a cuboid at each of the 26 joints of each hand. Where ``numberOfCuboids`` is defined, add this to make sure we have enough space in the constant buffers for our new cuboids:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_AddHandCuboids
	:end-before: XR_DOCS_TAG_END_AddHandCuboids
	:dedent: 8

Now in ``RenderLayer()``, just before the call to ``m_graphicsAPI->EndRendering()``, add the following code so that we render both hands, with all their joints:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderHands
	:end-before: XR_DOCS_TAG_END_RenderHands
	:dedent: 12

Now run the app. You'll now see both hands rendered as blocks.

***************************
5.2 Composition Layer Depth
***************************

Composition Layer Depth is truly important for AR use cases as it allows applications to submit to the runtime a depth image that can be used for the accurate reprojection of rendered graphics in the real world. Without a submitted depth image, certain AR applications would experience lag and 'sloshing' of the rendered graphics over top of the real world. This extension *may in the future* also include the ability for runtimes to have real world objects occlude rendered objects correctly. e.g. a rendering a cube partially occluded by a door frame.

This functionality is provided to OpenXR via the use of the XR_KHR_composition_layer_depth extension (`OpenXR Specification 12.8. XR_KHR_composition_layer_depth <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_KHR_composition_layer_depth>`_). This is Khronos-approved extension and as such it's aligned with the standards used in the core specification. The extension allows depth images from a swapchain to be submitted alongside the color images. This extension works in conjunction with the projection layer type. 

The ``XrCompositionLayerProjection`` structure contains a pointer to an array of ``XrCompositionLayerProjectionView`` structures. Each of these structures refers to a single view in the XR system and a single image subresource from a swapchain. To submit a depth image, we employ the use of a ``XrCompositionLayerDepthInfoKHR`` structure. Like with ``XrCompositionLayerProjectionView``, ``XrCompositionLayerDepthInfoKHR`` refers to a single view in the XR system and a single image subresource from a swapchain. These structures are 'chained' together via the use of the ``const void* next`` member in ``XrCompositionLayerProjectionView``. We assign the memory address of a ``XrCompositionLayerDepthInfoKHR`` structure that we want to chain together. The runtime time will read the ``next`` pointer and associate the structures and ultimately the color and depth images togehter for compositing. This is the same style of extensiblity used in the Vulkan API.

One thing is now very clear to the programmer - we need a depth swapchain! Seldom used in windowed graphics, but required here to allow smooth rendering of the depth image and to not lock either the runtime or the application by waiting on a single depth image to be passed back and forth between them. In most windowed graphics applications, the depth image is 'discarded' at the end of the frame and it doesn't interact with the windowing system at all.

When creating a depth swapchain, we must check that the system supports a depth format for swapchain creation. You can check this with ``xrEnumerateSwapchainFormats()``. Unfortunately, there are no guarantees with in the OpenXR 1.0 core specification or the XR_KHR_composition_layer_depth extension revision 6 that states runtimes must support depth format for swapchains.

To implement this extension in the Chapter 5 code, we first add the following member to the ``RenderLayerInfo`` structure:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderLayer_LayerDepthInfos
	:end-before: XR_DOCS_TAG_END_RenderLayer_LayerDepthInfos
	:dedent: 8

Now, in the ``CreateInstance()`` method under the extensions from Chapter 2:

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

In the ``RenderLayer()`` method after we've resized the ``std::vector<XrCompositionLayerProjectionView>``, we also resize the ``std::vector<XrCompositionLayerDepthInfoKHR>``, both of which are found in the ``RenderLayerInfo`` parameter.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_ResizeLeyerDepthInfos
	:end-before: XR_DOCS_TAG_END_ResizeLeyerDepthInfos
	:dedent: 8

After we have filled out the ``XrCompositionLayerProjectionView`` structure, we fill out the ``XrCompositionLayerDepthInfoKHR`` structure and by using the ``XrCompositionLayerProjectionView::next`` pointer we chain the two structures together. This submits the depth and the color image together for use by the XR compositor.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SetupLeyerDepthInfos
	:end-before: XR_DOCS_TAG_END_SetupLeyerDepthInfos
	:dedent: 8

***********
5.3 Summary
***********

In this chapter, we have discussed a couple of pertinent extension that offers possiblites and accessiblity options.

Below is a download link to a zip archive for this chapter containing all the C++ and CMake code for all platform and graphics APIs.

:download:`Chapter5.zip <../build/eoc_archs/Chapter5.zip>`