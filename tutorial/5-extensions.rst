5 Extensions
==========

5. EXTENSIONS (extensions debug and hand tracking)

Using Extensions and Layers
---------------------------

5.1. Using Extensions and Layers xrEnumerateInstanceExtensionProperties,
xrEnumerateApiLayerProperties,

Using xrCreateDebugUtilsMessengerEXT
------------------------------------

XR_EXT_debug_utils is an instance extension for OpenXR, which allows the application to get more information on any errors or warnings etc. You can specify which message severities and types will checked. If a debug message raised, it is passed to the callback function, which can optionally use the user data pointer provided in the ``XrDebugUtilsMessengerCreateInfoEXT`` structure.

In the code below, we first check that ``"XR_EXT_debug_utils"`` is in ``activeInstanceExtensions`` which we used to create the ``XrInstance`` with the use of this little helper function ``IsStringInVector()``. Next, we set up the ``XrDebugUtilsMessengerCreateInfoEXT`` struct; specifying all the message severities and types.

Message Severities: 
 * Verbose: Output all diagnostic messages.
 * Info: Output information messages helpful in debugging.
 * Warning: Output messages that could suggest an application bug and that need reviewing.
 * Error: Output messages from errors that may cause undefined behavior and/or crashes.
 
Message Types:
 * General: An event type for general information.
 * Validation: An event type that may indicate invalid usage of OpenXR.
 * Performance: An event type that may indicate non-optimal usage of OpenXR.
 * Conformance: An event type that indicating a non-conformant OpenXR result from the runtime.

`OpenXR Specification 12.26.3. Debug Message Categorization <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#debug-message-categorization>`_. 

Next, we set the callback function that we want to use, and it must match the ``PFN_xrDebugUtilsMessengerCallbackEXT`` signature. Optionally, you can set a userData pointer, perhaps to a class, but we have set it to ``nullptr`` in this example.

XR_EXT_debug_utils is an extension and as such its functions are not loaded by default by the OpenXR loader. Therefore, we need to get the address of the function through the use of ``xrGetInstanceProcAddr()``. We pass the ``XrInstance`` and a string of the function we want to get, along a pointer to a function pointer variable. We need to cast that pointer to a function pointer variable to a ``PFN_xrVoidFunction*`` type. Once we have the ``xrCreateDebugUtilsMessengerEXT()`` function, we call it by passing the ``XrInstance``, a pointer to our ``XrDebugUtilsMessengerCreateInfoEXT`` structure and a pointer to our ``XrDebugUtilsMessengerEXT``. If all is successful, we have set up the DebugUtilsMessenger callback.

At the end of the program, we should destroy the ``XrDebugUtilsMessengerEXT``. Again, the ``xrDestroyDebugUtilsMessengerEXT()`` function needs to be loaded through the use of ``xrGetInstanceProcAddr()`` (See example below). Once loaded, we can call it by passing the ``XrDebugUtilsMessengerEXT`` and thus destroying it.

.. literalinclude:: ../Common/HelperFunctions.h
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Helper_Functions1
	:end-before: XR_DOCS_TAG_END_Helper_Functions1

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Create_DestroyDebugMessenger
	:end-before: XR_DOCS_TAG_END_Create_DestroyDebugMessenger
	:dedent: 4

.. literalinclude:: ../Common/OpenXRDebugUtils.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Create_DestroyDebugMessenger
	:end-before: XR_DOCS_TAG_END_Create_DestroyDebugMessenger

Below is an example of a OpenXR DebugUtilsMessenger Callback function. This function can be completely customised to your liking, but here we simply convert the message's severity and type to strings, and create a string to log to stdout. We also add a ``DEBUG_BREAK`` if the severity is an error. Just one thing to note: Applications should always return ``XR_FALSE`` from this function.

.. literalinclude:: ../Common/OpenXRDebugUtils.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_OpenXRMessageCallbackFunction
	:end-before: XR_DOCS_TAG_END_OpenXRMessageCallbackFunction


Extension Examples
------------------

5.3. Extension examples including XR_EXT_HAND_TRACKING cubes at joints.
OpenXR Specification 12.31 XR_EXT_hand_tracking <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_EXT_hand_tracking>`_. 
