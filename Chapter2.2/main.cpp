// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

// C Headers
#include <iostream>
#include <sstream>
#include <vector>

#include "GraphicsAPIs.h"

// Debugbreak
#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#endif

// XR_DOCS_TAG_BEGIN_Helper_Functions1
bool IsStringInVector(std::vector<const char *> list, const char *name) {
    bool found = false;
    for (auto &item : list) {
        if (strcmp(name, item) == 0) {
            found = true;
            break;
        }
    }
    return found;
}

template <typename T>
bool BitwiseCheck(const T &value, const T &checkValue) {
    return ((value & checkValue) == checkValue);
}
// XR_DOCS_TAG_END_Helper_Functions1

// XR_DOCS_TAG_BEGIN_OpenXRMessageCallbackFunction
XrBool32 OpenXRMessageCallbackFunction(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageType, const XrDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    auto GetMessageSeverityString = [](XrDebugUtilsMessageSeverityFlagsEXT messageSeverity) -> std::string {
        bool separator = false;

        std::string msg_flags;
        if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
            msg_flags += "VERBOSE";
            separator = true;
        }
        if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
            if (separator)
                msg_flags += ",";
            msg_flags += "INFO";
            separator = true;
        }
        if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
            if (separator)
                msg_flags += ",";
            msg_flags += "WARN";
            separator = true;
        }
        if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
            if (separator)
                msg_flags += ",";
            msg_flags += "ERROR";
        }
        return msg_flags;
    };
    auto GetMessageTypeString = [](XrDebugUtilsMessageTypeFlagsEXT messageType) -> std::string {
        bool separator = false;

        std::string msg_flags;
        if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)) {
            msg_flags += "GEN";
            separator = true;
        }
        if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)) {
            if (separator)
                msg_flags += ",";
            msg_flags += "SPEC";
            separator = true;
        }
        if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)) {
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
    if (messageSeverity == XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        DEBUG_BREAK;
    }
    return XrBool32();
}
// XR_DOCS_TAG_END_OpenXRMessageCallbackFunction

class OpenXRTutorial_Ch2_2 {
public:
    OpenXRTutorial_Ch2_2() = default;
    ~OpenXRTutorial_Ch2_2() = default;

    void Run() {
        CreateInstance();
        CreateDebugMessenger();

        GetInstanceProperties();
        GetSystemID();

        CreateSession();
        DestroySession();

        DestroyDebugMessenger();
        DestroyInstance();
    }

private:
    void CreateInstance() {
        // XR_DOCS_TAG_BEGIN_XrApplicationInfo
        XrApplicationInfo AI;
        strcpy(AI.applicationName, "OpenXR Tutorial Chapter 2.2");
        AI.applicationVersion = 1;
        strcpy(AI.engineName, "OpenXR Engine");
        AI.engineVersion = 1;
        AI.apiVersion = XR_CURRENT_API_VERSION;
        // XR_DOCS_TAG_END_XrApplicationInfo

        // Add additional instance layers/extensions
        {
            // XR_DOCS_TAG_BEGIN_instanceExtensions
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
            // XR_DOCS_TAG_END_instanceExtensions
        }

        // XR_DOCS_TAG_BEGIN_find_apiLayer_extension
        uint32_t apiLayerCount = 0;
        std::vector<XrApiLayerProperties> apiLayerProperties;
        OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
        apiLayerProperties.resize(apiLayerCount);
        for (auto &apiLayerProperty : apiLayerProperties)
            apiLayerProperty.type = XR_TYPE_API_LAYER_PROPERTIES;

        OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");
        for (auto &requestLayer : apiLayers) {
            for (auto &layerProperty : apiLayerProperties) {
                if (strcmp(requestLayer.c_str(), layerProperty.layerName)) {
                    continue;
                } else {
                    activeAPILayers.push_back(requestLayer.c_str());
                    break;
                }
            }
        }

        uint32_t extensionCount = 0;
        std::vector<XrExtensionProperties> extensionProperties;
        OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");
        extensionProperties.resize(extensionCount);
        for (auto &extensionProperty : extensionProperties)
            extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;

        OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");
        for (auto &requestExtension : instanceExtensions) {
            for (auto &extensionProperty : extensionProperties) {
                if (strcmp(requestExtension.c_str(), extensionProperty.extensionName)) {
                    continue;
                } else {
                    activeInstanceExtensions.push_back(requestExtension.c_str());
                    break;
                }
            }
        }
        // XR_DOCS_TAG_END_find_apiLayer_extension

        // XR_DOCS_TAG_BEGIN_XrInstanceCreateInfo
        XrInstanceCreateInfo instanceCI{XR_TYPE_INSTANCE_CREATE_INFO};
        instanceCI.createFlags = 0;
        instanceCI.applicationInfo = AI;
        instanceCI.enabledApiLayerCount = static_cast<uint32_t>(activeAPILayers.size());
        instanceCI.enabledApiLayerNames = activeAPILayers.data();
        instanceCI.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
        instanceCI.enabledExtensionNames = activeInstanceExtensions.data();
        OPENXR_CHECK(xrCreateInstance(&instanceCI, &instance), "Failed to create Instance.");
        // XR_DOCS_TAG_END_XrInstanceCreateInfo
    }

    void DestroyInstance() {
        OPENXR_CHECK(xrDestroyInstance(instance), "Failed to destroy Instance.");
    }

    // XR_DOCS_TAG_BEGIN_Create_DestroyDebugMessenger
    void CreateDebugMessenger() {
        if (IsStringInVector(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            XrDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
            debugUtilsMessengerCI.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugUtilsMessengerCI.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
            debugUtilsMessengerCI.userCallback = (PFN_xrDebugUtilsMessengerCallbackEXT)OpenXRMessageCallbackFunction;
            debugUtilsMessengerCI.userData = nullptr;

            PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT;
            OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction *)&xrCreateDebugUtilsMessengerEXT), "Failed to get InstanceProcAddr.");
            OPENXR_CHECK(xrCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCI, &debugUtilsMessenger), "Failed to create DebugUtilsMessenger.");
        }
    }
    void DestroyDebugMessenger() {
        if (IsStringInVector(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT;
            OPENXR_CHECK(xrGetInstanceProcAddr(instance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction *)&xrDestroyDebugUtilsMessengerEXT), "Failed to get InstanceProcAddr.");
            OPENXR_CHECK(xrDestroyDebugUtilsMessengerEXT(debugUtilsMessenger), "Failed to destroy DebugUtilsMessenger.");
        }
    }
    // XR_DOCS_TAG_END_Create_DestroyDebugMessenger

    void GetInstanceProperties() {
        XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
        OPENXR_CHECK(xrGetInstanceProperties(instance, &instanceProperties), "Failed to get InstanceProperties.");

        std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - ";
        std::cout << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << ".";
        std::cout << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << ".";
        std::cout << XR_VERSION_PATCH(instanceProperties.runtimeVersion) << std::endl;
    }

    void GetSystemID() {
        XrSystemGetInfo systemGI{XR_TYPE_SYSTEM_GET_INFO};
        systemGI.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        OPENXR_CHECK(xrGetSystem(instance, &systemGI, &systemID), "Failed to get SystemID.");

        XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
        OPENXR_CHECK(xrGetSystemProperties(instance, systemID, &systemProperties), "Failed to get SystemProperties.");
    }

    void CreateSession() {
        XrSessionCreateInfo sessionCI{XR_TYPE_SESSION_CREATE_INFO};

#if defined(XR_USE_GRAPHICS_API_D3D11)
        XrGraphicsBindingD3D11KHR graphicsBindingD3D11{XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
        GraphicsAPI_D3D11 d3d11(instance, systemID);
        graphicsBindingD3D11.device = d3d11.device;
        sessionCI.next = &graphicsBindingD3D11;
#elif defined(XR_USE_GRAPHICS_API_D3D12)
        XrGraphicsBindingD3D12KHR graphicsBindingD3D12{XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};
        GraphicsAPI_D3D12 d3d12(instance, systemID);
        graphicsBindingD3D12.device = d3d12.device;
        graphicsBindingD3D12.queue = d3d12.queue;
        sessionCI.next = &graphicsBindingD3D12;
#elif defined(XR_USE_GRAPHICS_API_OPENGL)
        GraphicsAPI_OpenGL opengl(instance, systemID);
#if defined(XR_USE_PLATFORM_WIN32)
        XrGraphicsBindingOpenGLWin32KHR graphicsBindingOpenGLWin32{XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR};
        graphicsBindingOpenGLWin32.hDC = opengl.window.context.hDC;
        graphicsBindingOpenGLWin32.hGLRC = opengl.window.context.hGLRC;
        sessionCI.next = &graphicsBindingOpenGLWin32;
#elif defined(XR_USE_PLATFORM_XLIB)
        XrGraphicsBindingOpenGLXlibKHR graphicsBindingOpenGLXlib;
        sessionCI.next = &graphicsBindingOpenGLXlib;
#elif defined(XR_USE_PLATFORM_XCB)
        XrGraphicsBindingOpenGLXcbKHR graphicsBindingOpenGLXcb;
        sessionCI.next = &graphicsBindingOpenGLXcb;
#elif defined(XR_USE_PLATFORM_WAYLAND)
        XrGraphicsBindingOpenGLWaylandKHR graphicsBindingOpenGLWayland;
        sessionCI.next = &graphicsBindingOpenGLWayland;
#endif
#elif defined(XR_USE_GRAPHICS_API_OPENGL_ES)
#if defined(XR_USE_PLATFORM_ANDROID)
        XrGraphicsBindingOpenGLESAndroidKHR graphicsBindingOpenGLESAndroid{XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
        GraphicsAPI_OpenGLES opengles(instance, systemID);
        graphicsBindingOpenGLESAndroid.display = opengles.window.display;
        graphicsBindingOpenGLESAndroid.config = opengles.window.context.config;
        graphicsBindingOpenGLESAndroid.context = opengles.window.context.context;
        sessionCI.next = &graphicsBindingOpenGLESAndroid;
#endif
#elif defined(XR_USE_GRAPHICS_API_VULKAN)
        XrGraphicsBindingVulkanKHR graphicsBindingVulkan{XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR};
        GraphicsAPI_Vulkan vulkan(instance, systemID);
        graphicsBindingVulkan.instance = vulkan.instance;
        graphicsBindingVulkan.physicalDevice = vulkan.physicalDevice;
        graphicsBindingVulkan.device = vulkan.device;
        graphicsBindingVulkan.queueFamilyIndex = vulkan.queueFamilyIndex;
        graphicsBindingVulkan.queueIndex = vulkan.queueIndex;
        sessionCI.next = &graphicsBindingVulkan;
#endif
        sessionCI.createFlags = 0;
        sessionCI.systemId = systemID;

        OPENXR_CHECK(xrCreateSession(instance, &sessionCI, &session), "Failed to create Session.");
        OPENXR_CHECK(xrDestroySession(session), "Failed to destroy Session.");
    }
    void DestroySession() {
    }

private:
    XrInstance instance = {};
    std::vector<const char *> activeAPILayers = {};
    std::vector<const char *> activeInstanceExtensions = {};
    std::vector<std::string> apiLayers = {};
    std::vector<std::string> instanceExtensions = {};
    XrDebugUtilsMessengerEXT debugUtilsMessenger = {};
    XrSystemId systemID = {};
    XrSession session = {};
};

void OpenXRTutorial_Main() {
    std::cout << "OpenXR Tutorial Chapter 2.2." << std::endl;

    OpenXRTutorial_Ch2_2 app;
    app.Run();
}

#if defined(_WIN32)
// XR_DOCS_TAG_BEGIN_main_WIN32___linux__
int main(int argc, char **argv) {
    OpenXRTutorial_Main();
}
// XR_DOCS_TAG_END_main_WIN32___linux__
#elif (__ANDROID__)
// XR_DOCS_TAG_BEGIN_android_main___ANDROID__
void android_main(struct android_app *app) {
    PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR;
    OPENXR_CHECK(xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction *)&xrInitializeLoaderKHR), "Failed to get InstanceProcAddr.");
    if (!xrInitializeLoaderKHR) {
        return;
    }

    XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid{XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR};
    loaderInitializeInfoAndroid.applicationVM = app->activity->vm;
    loaderInitializeInfoAndroid.applicationContext = app->activity->clazz;
    OPENXR_CHECK(xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR *)&loaderInitializeInfoAndroid), "Failed to initialise Loader for Android.");

    OpenXRTutorial_Main();
}
// XR_DOCS_TAG_END_android_main___ANDROID__
#endif