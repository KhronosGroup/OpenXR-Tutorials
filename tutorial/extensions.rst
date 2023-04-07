Extensions
==========

5. EXTENSIONS (extensions debug and hand tracking)

Using Extensions and Layers
---------------------------

5.1. Using Extensions and Layers xrEnumerateInstanceExtensionProperties,
xrEnumerateApiLayerProperties,

Using the Debug Utils Messenger Extension
-----------------------------------------

5.2. Using xrCreateDebugUtilsMessengerEXT (might consider doing this earlier to
show how to get debug messages)

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

.. code-block:: cpp

	bool IsStringInVector(std::vector<const char*> list, const char* name)
	{
		bool found = false;
		for (auto& item : list)
		{
			if (strcmp(name, item) == 0)
			{
				found = true;
				break;
			}
		}
		return found;
	}

	void CreateDebugMessenger()
	{
		if (IsStringInVector(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME))
		{
			XrDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI;
			debugUtilsMessengerCI.type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCI.next = nullptr;
			debugUtilsMessengerCI.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerCI.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
			debugUtilsMessengerCI.userCallback = (PFN_xrDebugUtilsMessengerCallbackEXT)OpenXRMessageCallbackFunction;
			debugUtilsMessengerCI.userData = nullptr;

			PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT;
			OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&xrCreateDebugUtilsMessengerEXT), "Failed to get InstanceProcAddr.");
			OPENXR_CHECK(xrCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCI, &debugUtilsMessenger), "Failed to create DebugUtilsMessenger.");
		}
	}
	void DestroyDebugMessenger()
	{
		if (IsStringInVector(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME))
		{
			PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT;
			OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&xrDestroyDebugUtilsMessengerEXT), "Failed to get InstanceProcAddr.");
			OPENXR_CHECK(xrDestroyDebugUtilsMessengerEXT(debugUtilsMessenger), "Failed to destroy DebugUtilsMessenger.");
		}
	}

Below is an example of a OpenXR DebugUtilsMessenger Callback function. This function can be completely customised to your liking, but here we simply convert the message's severity and type to strings, and create a string to log to stdout. We also add a DEBUG_BREAK if the severity is an error. Just one thing to note: Applications should always return XR_FALSE from this function.

.. code-block:: cpp

	template<typename T>
	bool BitwiseCheck(const T& value, const T& checkValue)
	{
		return ((value & checkValue) == checkValue);
	}

	XrBool32 OpenXRMessageCallbackFunction(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageType, const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		auto GetMessageSeverityString = [](XrDebugUtilsMessageSeverityFlagsEXT messageSeverity)->std::string
		{
			bool separator = false;

			std::string msg_flags;
			if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT))
			{
				msg_flags += "VERBOSE";
				separator = true;
			}
			if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT))
			{
				if (separator)
					msg_flags += ",";
				msg_flags += "INFO";
				separator = true;
			}
			if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
			{
				if (separator)
					msg_flags += ",";
				msg_flags += "WARN";
				separator = true;
			}
			if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT))
			{
				if (separator)
					msg_flags += ",";
				msg_flags += "ERROR";
			}
			return msg_flags;
		};
		auto GetMessageTypeString = [](XrDebugUtilsMessageTypeFlagsEXT messageType)->std::string
		{
			bool separator = false;

			std::string msg_flags;
			if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT))
			{
				msg_flags += "GEN";
				separator = true;
			}
			if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT))
			{
				if (separator)
					msg_flags += ",";
				msg_flags += "SPEC";
				separator = true;
			}
			if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
			{
				if (separator)
					msg_flags += ",";
				msg_flags += "PERF";
			}
			return msg_flags;
		};

		std::string functionName = (pCallbackData->functionName) ? pCallbackData->functionName : "";
		std::string messageSeverityStr = GetMessageSeverityString(messageSeverity);
		std::string messageTypeStr = GetMessageTypeString(messageType);
		std::string messageId = (pCallbackData->messageId) ? pCallbackData->messageId : "";
		std::string message = (pCallbackData->message) ? pCallbackData->message : "";

		std::stringstream errorMessage;
		errorMessage << functionName << "(" << messageSeverityStr << " / " << messageTypeStr << "): msgNum: " << messageId << " - " << message;

		std::cerr << errorMessage.str();
		if (messageSeverity == XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			DEBUG_BREAK;
		}
		return XrBool32();
	}

Extension Examples
------------------

5.3. Extension examples including XR_EXT_HAND_TRACKING cubes at joints.
