//Simul Software Ltd 2023
//OpenXR Tutorial for Khronos Group

//C Headers
#include <iostream>
#include <sstream>
#include <vector>

//OpenXR Headers
#include "openxr/openxr.h"

//#define XR_USE_GRAPHICS_API_D3D11
//#define XR_USE_GRAPHICS_API_D3D12
//#define XR_USE_GRAPHICS_API_OPENGL
//#define XR_USE_GRAPHICS_API_OPENGL_ES
//#define XR_USE_GRAPHICS_API_VULKAN
#include "openxr/openxr_platform.h"

//Debugbreak
#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#endif

const char *GetXRErrorString(XrInstance	xr_instance,XrResult res)
{
	static char str[XR_MAX_RESULT_STRING_SIZE];
	xrResultToString(xr_instance, res, str);
	return str;
}

//Helper Functions
#define OPENXR_CHECK(x, y) { if (!XR_SUCCEEDED(x)) { std::cout << "ERROR: OPENXR: " << y << std::endl; } }

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

	std::cerr << errorMessage.str() << std::endl;
	if (messageSeverity == XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		DEBUG_BREAK;
	}
	return XrBool32();
}

class OpenXRTutorial_Ch2_1
{
public:
	OpenXRTutorial_Ch2_1() = default;
	~OpenXRTutorial_Ch2_1() = default;

	void Run()
	{
		CreateInstance();
		CreateDebugMessenger();

		GetInstanceProperties();
		GetSystemID();

		DestroyDebugMessenger();
		DestroyInstance();
	}

private:
	
	void CreateInstance()
	{
		XrApplicationInfo AI;
		strcpy(AI.applicationName, "OpenXR Tutorial Chapter 2.1");
		AI.applicationVersion = 1;
		strcpy(AI.engineName, "OpenXR Engine");
		AI.engineVersion = 1;
		AI.apiVersion = XR_CURRENT_API_VERSION;

		//Add additional instance layers/extensions
		{
			instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);

			#if defined(XR_USE_GRAPHICS_API_D3D11)
				instanceExtensions.push_back(XR_KHR_D3D11_ENABLE_EXTENSION_NAME);
			#elif defined(XR_USE_GRAPHICS_API_D3D12)
				instanceExtensions.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
			#elif defined(XR_USE_GRAPHICS_API_OPENGL)
				instanceExtensions.push_back(XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);
			#elif defined(XR_USE_GRAPHICS_API_OPENGL_ES)
				instanceExtensions.push_back(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);
			#elif defined(XR_USE_GRAPHICS_API_VULKAN)
				instanceExtensions.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);
			#endif
		}

		uint32_t apiLayerCount = 0;
		std::vector<XrApiLayerProperties> apiLayerProperties;
		OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
		apiLayerProperties.resize(apiLayerCount);
		for (auto& apiLayerProperty : apiLayerProperties)
			apiLayerProperty.type = XR_TYPE_API_LAYER_PROPERTIES;
		OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");
		for (auto& requestLayer : apiLayers)
		{
			for (auto& layerProperty : apiLayerProperties)
			{
				if (strcmp(requestLayer.c_str(), layerProperty.layerName))
					continue;
				else
					activeAPILayers.push_back(requestLayer.c_str()); break;
			}
		}
		uint32_t extensionCount = 0;
		std::vector<XrExtensionProperties> extensionProperties;
		OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");
		extensionProperties.resize(extensionCount);
		for (auto& extensionProperty : extensionProperties)
			extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;
		OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");
		for (auto& requestExtension : instanceExtensions)
		{
			for (auto& extensionProperty : extensionProperties)
			{
				if (strcmp(requestExtension.c_str(), extensionProperty.extensionName))
					continue;
				else
					activeInstanceExtensions.push_back(requestExtension.c_str()); break;
			}
		}

		XrInstanceCreateInfo instanceCI;
		instanceCI.type = XR_TYPE_INSTANCE_CREATE_INFO;
		instanceCI.next = nullptr;
		instanceCI.createFlags = 0;
		instanceCI.applicationInfo = AI;
		instanceCI.enabledApiLayerCount = static_cast<uint32_t>(activeAPILayers.size());
		instanceCI.enabledApiLayerNames = activeAPILayers.data();
		instanceCI.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
		instanceCI.enabledExtensionNames = activeInstanceExtensions.data();
		OPENXR_CHECK(xrCreateInstance(&instanceCI, &instance), "Failed to create Instance.");
	}

	void DestroyInstance()
	{
		OPENXR_CHECK(xrDestroyInstance(instance), "Failed to destroy Instance.");
	}

	void GetInstanceProperties()
	{
		XrInstanceProperties instanceProperties;
		instanceProperties.type = XR_TYPE_INSTANCE_PROPERTIES;
		instanceProperties.next = nullptr;
		OPENXR_CHECK(xrGetInstanceProperties(instance, &instanceProperties), "Failed to get InstanceProperties.");

		std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - ";
		std::cout << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_PATCH(instanceProperties.runtimeVersion) << std::endl;
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

	void GetSystemID()
	{
		XrSystemGetInfo systemGI;
		systemGI.type = XR_TYPE_SYSTEM_GET_INFO;
		systemGI.next = nullptr;
		systemGI.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		OPENXR_CHECK(xrGetSystem(instance, &systemGI, &systemID), "Failed to get SystemID.");

		XrSystemProperties systemProperties;
		systemProperties.type = XR_TYPE_SYSTEM_PROPERTIES;
		systemProperties.next = nullptr;
		OPENXR_CHECK(xrGetSystemProperties(instance, systemID, &systemProperties), "Failed to get SystemProperties.");
	}

private:
	XrInstance instance = {};
	std::vector<const char*> activeAPILayers = {};
	std::vector<const char*> activeInstanceExtensions = {};
	std::vector<std::string> apiLayers = {};
	std::vector<std::string> instanceExtensions = {};
	XrDebugUtilsMessengerEXT debugUtilsMessenger = {};
	XrSystemId systemID = {};
};

void OpenXRTutorial_Main()
{
	std::cout << "OpenXR Tutorial Chapter 2.1." << std::endl;

	OpenXRTutorial_Ch2_1 app;
	app.Run();
}

#if defined(_WIN32)
int main(int argc, char** argv)
{
	OpenXRTutorial_Main();
}
#elif(__ANDROID__)
#include "android_native_app_glue.h"

void android_main(struct android_app* app)
{
	OpenXRTutorial_Main();
}
#endif