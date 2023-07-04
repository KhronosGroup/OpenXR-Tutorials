// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

#include "GraphicsAPIs.h"
#include "OpenXRDebugUtils.h"

#define XR_DOCS_CHAPTER_VERSION XR_DOCS_CHAPTER_2_2

class OpenXRTutorialChapter2 {
public:
    OpenXRTutorialChapter2(GraphicsAPI_Type api)
        : apiType(api) {
        if (!CheckGraphicsAPI_TypeIsValidForPlatform(apiType)) {
            std::cout << "ERROR: The provided Graphics API is not valid for this platform." << std::endl;
            DEBUG_BREAK;
        }
    }
    ~OpenXRTutorialChapter2() = default;

    void Run() {
        CreateInstance();
        CreateDebugMessenger();

        GetInstanceProperties();
        GetSystemID();

        if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_2) {
            CreateSession();
            DestroySession();
        }

        DestroyDebugMessenger();
        DestroyInstance();
    }

private:
    void CreateInstance() {
        // XR_DOCS_TAG_BEGIN_XrApplicationInfo
        XrApplicationInfo AI;
        strcpy(AI.applicationName, "OpenXR Tutorial Chapter 2");
        AI.applicationVersion = 1;
        strcpy(AI.engineName, "OpenXR Engine");
        AI.engineVersion = 1;
        AI.apiVersion = XR_CURRENT_API_VERSION;
        // XR_DOCS_TAG_END_XrApplicationInfo

        // Add additional instance layers/extensions
        {
            // XR_DOCS_TAG_BEGIN_instanceExtensions
            instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
            instanceExtensions.push_back(GetGraphicsAPIInstanceExtensionString(apiType));
            // XR_DOCS_TAG_END_instanceExtensions
        }

        // XR_DOCS_TAG_BEGIN_find_apiLayer_extension
        uint32_t apiLayerCount = 0;
        std::vector<XrApiLayerProperties> apiLayerProperties;
        OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
        apiLayerProperties.resize(apiLayerCount, {XR_TYPE_API_LAYER_PROPERTIES});

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
        extensionProperties.resize(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});

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
            debugUtilsMessenger = CreateOpenXRDebugUtilsMessenger(instance);
        }
    }
    void DestroyDebugMessenger() {
        if (IsStringInVector(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            DestroyOpenXRDebugUtilsMessenger(instance, debugUtilsMessenger);
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

        if (apiType == D3D11) {
#if defined(XR_USE_GRAPHICS_API_D3D11)
            graphicsAPI = std::make_unique<GraphicsAPI_D3D11>(instance, systemID);
#endif
        } else if (apiType == D3D12) {
#if defined(XR_USE_GRAPHICS_API_D3D12)
            graphicsAPI = std::make_unique<GraphicsAPI_D3D12>(instance, systemID);
#endif
        } else if (apiType == OPENGL) {
#if defined(XR_USE_GRAPHICS_API_OPENGL)
            graphicsAPI = std::make_unique<GraphicsAPI_OpenGL>(instance, systemID);
#endif
        } else if (apiType == OPENGL_ES) {
#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
            graphicsAPI = std::make_unique<GraphicsAPI_OpenGL_ES>(instance, systemID);
#endif
        } else if (apiType == VULKAN) {
#if defined(XR_USE_GRAPHICS_API_VULKAN)
            graphicsAPI = std::make_unique<GraphicsAPI_Vulkan>(instance, systemID);
#endif
        } else {
            std::cout << "ERROR: Unknown Graphics API." << std::endl;
            DEBUG_BREAK;
        }
        sessionCI.next = graphicsAPI->GetGraphicsBinding();
        sessionCI.createFlags = 0;
        sessionCI.systemId = systemID;

        OPENXR_CHECK(xrCreateSession(instance, &sessionCI, &session), "Failed to create Session.");
    }

    void DestroySession() {
        OPENXR_CHECK(xrDestroySession(session), "Failed to destroy Session.");
    }

private:
    GraphicsAPI_Type apiType = UNKNOWN;

    XrInstance instance = {};
    std::vector<const char *> activeAPILayers = {};
    std::vector<const char *> activeInstanceExtensions = {};
    std::vector<std::string> apiLayers = {};
    std::vector<std::string> instanceExtensions = {};
    XrDebugUtilsMessengerEXT debugUtilsMessenger = {};
    XrSystemId systemID = {};

    std::unique_ptr<GraphicsAPI> graphicsAPI = nullptr;
    XrSession session = {};
};

void OpenXRTutorial_Main() {
    std::cout << "OpenXR Tutorial Chapter 2." << std::endl;

    OpenXRTutorialChapter2 app(VULKAN);
    app.Run();
}

#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
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