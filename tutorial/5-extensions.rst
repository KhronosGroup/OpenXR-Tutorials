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
	:dedent: 3

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_handTrackingExtensions
	:end-before: XR_DOCS_TAG_END_handTrackingExtensions
	:dedent: 3

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
	:dedent: 2

These calls require an XrInstance, so we must initialize these after XrCreateInstance() has successfully returned. If hand tracking is not supported (or not enabled), we'll get a warning, and these functions will be null. You can run your app now to check this.

At the end of your OpenXRTutorial application class, declare the following:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_HandTracking
	:end-before: XR_DOCS_TAG_END_HandTracking
	:dedent: 1

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
	:dedent: 1

The XrHandTrackerEXT object is a session-lifetime object, so in DestroySession(), at the top of the function we'll add:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroyHandTracker
	:end-before: XR_DOCS_TAG_END_DestroyHandTracker
	:dedent: 2

Hands should be polled once per frame. At the end of PollActions(), we'll poll each hand. We'll assume to begin with that the user isn't holding the controller, so we'll use the XR_HAND_JOINTS_MOTION_RANGE_UNOBSTRUCTED_EXT motion range.

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollHands
	:end-before: XR_DOCS_TAG_END_PollHands
	:dedent: 2

Finally, we'll render the hands simply by drawing a cuboid at each of the 26 joints of each hand. Where `numberOfCuboids` is defined, add this to make sure we have enough space in the constant buffers:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_AddHandCuboids
	:end-before: XR_DOCS_TAG_END_AddHandCuboids
	:dedent: 2

Now in RenderLayer(), just before the call to `m_graphicsAPI->EndRendering()`, add this so we render both hands, with all their joints:

.. literalinclude:: ../Chapter5/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderHands
	:end-before: XR_DOCS_TAG_END_RenderHands
	:dedent: 3

Run the app: you'll now see both hands, rendered as blocks.

***************************
5.2 Composition Layer Depth
***************************