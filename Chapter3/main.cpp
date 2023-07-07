// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

#include "GraphicsAPIs.h"
#include "OpenXRDebugUtils.h"

#define XR_DOCS_CHAPTER_VERSION XR_DOCS_CHAPTER_3_2

class OpenXRTutorialChapter3 {
public:
    OpenXRTutorialChapter3(GraphicsAPI_Type api)
        : apiType(api) {
        if (!CheckGraphicsAPI_TypeIsValidForPlatform(apiType)) {
            std::cout << "ERROR: The provided Graphics API is not valid for this platform." << std::endl;
            DEBUG_BREAK;
        }
    }
    ~OpenXRTutorialChapter3() = default;

    void Run() {
        CreateInstance();
        CreateDebugMessenger();

        GetInstanceProperties();
        GetSystemID();

        if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1) {
            GetViewConfigurationViews();
            if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2) {
                GetEnvirmentBlendModes();
            }
        }

        if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_2) {
            CreateSession();
            if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2) {
                CreateReferenceSpace();
            }
            if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1) {
                CreateSwapchain();
            }
        }

        if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_3) {
            while (applicationRunning) {
#if defined(__ANDROID__)
                PollAndroidEvents();
#endif

                PollEvents();
                if (sessionRunning) {
                    RenderFrame();
                }
            }
        }

        if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_2) {
            if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1) {
                DestroySwapchain();
            }
            if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2) {
                DestroyReferenceSpace();
            }
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
        systemGI.formFactor = formFactor;
        OPENXR_CHECK(xrGetSystem(instance, &systemGI, &systemID), "Failed to get SystemID.");

        XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
        OPENXR_CHECK(xrGetSystemProperties(instance, systemID, &systemProperties), "Failed to get SystemProperties.");
    }

    void GetEnvirmentBlendModes() {
        uint32_t environmentBlendModeSize = 0;
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(instance, systemID, viewConfiguration, 0, &environmentBlendModeSize, nullptr), "Failed to enumerate ViewConfigurationViews.");
        environmentBlendModes.resize(environmentBlendModeSize);
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(instance, systemID, viewConfiguration, environmentBlendModeSize, &environmentBlendModeSize, environmentBlendModes.data()), "Failed to enumerate ViewConfigurationViews.");
    }

    void GetViewConfigurationViews() {
        uint32_t viewConfigurationViewSize = 0;
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(instance, systemID, viewConfiguration, 0, &viewConfigurationViewSize, nullptr), "Failed to enumerate ViewConfigurationViews.");
        viewConfigurationViews.resize(viewConfigurationViewSize, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(instance, systemID, viewConfiguration, viewConfigurationViewSize, &viewConfigurationViewSize, viewConfigurationViews.data()), "Failed to enumerate ViewConfigurationViews.");
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

    void PollEvents() {
        XrResult result = XR_SUCCESS;
        do {
            XrEventDataBuffer eventData{XR_TYPE_EVENT_DATA_BUFFER};
            result = xrPollEvent(instance, &eventData);

            switch (eventData.type) {
            case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
                XrEventDataEventsLost *eventsLost = reinterpret_cast<XrEventDataEventsLost *>(&eventData);
                std::cout << "WARN: OPENXR: Events Lost: " << eventsLost->lostEventCount << std::endl;
                break;
            }
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                XrEventDataInstanceLossPending *instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending *>(&eventData);
                std::cout << "WARN: OPENXR: Instance Loss Pending at: " << instanceLossPending->lossTime << std::endl;
                sessionRunning = false;
                applicationRunning = false;
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
                XrEventDataInteractionProfileChanged *interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged *>(&eventData);
                std::cout << "WARN: OPENXR: Interaction Profile changed for Session: " << interactionProfileChanged->session << std::endl;
                break;
            }
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                XrEventDataReferenceSpaceChangePending *referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending *>(&eventData);
                std::cout << "WARN: OPENXR: Reference Space Change pending for Session: " << referenceSpaceChangePending->session << std::endl;
                break;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                XrEventDataSessionStateChanged *sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged *>(&eventData);

                if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
                    XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
                    sessionBeginInfo.primaryViewConfigurationType = viewConfiguration;
                    OPENXR_CHECK(xrBeginSession(session, &sessionBeginInfo), "Failed to begin Session.");
                    sessionRunning = true;
                }
                if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
                    OPENXR_CHECK(xrEndSession(session), "Failed to end Session.");
                    sessionRunning = false;
                }
                if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
                    sessionRunning = false;
                    applicationRunning = false;
                }
                sessionState = sessionStateChanged->state;
                break;
            }
            default: {
                break;
            }
            }

        } while (result == XR_SUCCESS);
    }

    void CreateReferenceSpace() {
        XrReferenceSpaceCreateInfo referenceSpaceCI{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        referenceSpaceCI.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
        OPENXR_CHECK(xrCreateReferenceSpace(session, &referenceSpaceCI, &space), "Failed to create ReferenceSpace.");
    }

    void DestroyReferenceSpace() {
        OPENXR_CHECK(xrDestroySpace(space), "Failed to destroy Space.")
    }

    void CreateSwapchain() {
        uint32_t formatSize = 0;
        OPENXR_CHECK(xrEnumerateSwapchainFormats(session, 0, &formatSize, nullptr), "Failed to enumerate Swapchain Formats");
        std::vector<int64_t> formats(formatSize);
        OPENXR_CHECK(xrEnumerateSwapchainFormats(session, formatSize, &formatSize, formats.data()), "Failed to enumerate Swapchain Formats");

        // Check the two views for stereo are the same
        if (viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO && viewConfigurationViews.size() == 2) {
            bool viewWidthsSame = viewConfigurationViews[0].recommendedImageRectWidth == viewConfigurationViews[1].recommendedImageRectWidth;
            bool viewHeightsSame = viewConfigurationViews[0].recommendedImageRectHeight == viewConfigurationViews[1].recommendedImageRectHeight;
            if (!viewWidthsSame || !viewHeightsSame) {
                std::cout << "ERROR: The two views for stereo are not the same." << std::endl;
                DEBUG_BREAK;
            }
        }
        const XrViewConfigurationView &viewConfigurationView = viewConfigurationViews[0];

        XrSwapchainCreateInfo swapchainCI{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchainCI.createFlags = 0;
        swapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCI.format = graphicsAPI->SelectSwapchainFormat(formats);
        swapchainCI.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount;
        swapchainCI.width = viewConfigurationView.recommendedImageRectWidth;
        swapchainCI.height = viewConfigurationView.recommendedImageRectHeight;
        swapchainCI.faceCount = 1;
        swapchainCI.arraySize = static_cast<uint32_t>(viewConfigurationViews.size());
        swapchainCI.mipCount = 1;
        OPENXR_CHECK(xrCreateSwapchain(session, &swapchainCI, &swapchain), "Failed to create Swapchain");
        swapchainFormat = swapchainCI.format;

        uint32_t swapchainImageCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainImages(swapchain, 0, &swapchainImageCount, nullptr), "Failed to enumerate Swapchain Images.");
        XrSwapchainImageBaseHeader *swapchainImages = graphicsAPI->AllocateSwapchainImageData(swapchainImageCount);
        OPENXR_CHECK(xrEnumerateSwapchainImages(swapchain, swapchainImageCount, &swapchainImageCount, swapchainImages), "Failed to enumerate Swapchain Images.");

        GraphicsAPI::ImageCreateInfo depthImageCI;
        depthImageCI.dimension = 2;
        depthImageCI.width = viewConfigurationView.recommendedImageRectWidth;
        depthImageCI.height = viewConfigurationView.recommendedImageRectHeight;
        depthImageCI.depth = 1;
        depthImageCI.mipLevels = 1;
        depthImageCI.arrayLayers = static_cast<uint32_t>(viewConfigurationViews.size());
        depthImageCI.sampleCount = 1;
        depthImageCI.format = graphicsAPI->GetDepthFormat();
        depthImageCI.cubemap = false;
        depthImageCI.colorAttachment = false;
        depthImageCI.depthAttachment = true;
        depthImageCI.sampled = false;
        depthImage = graphicsAPI->CreateImage(depthImageCI);

        for (uint32_t i = 0; i < swapchainImageCount; i++) {
            GraphicsAPI::ImageViewCreateInfo imageViewCI;
            imageViewCI.image = graphicsAPI->GetSwapchainImage(i);
            imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::RTV;
            imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D_ARRAY;
            imageViewCI.format = swapchainFormat;
            imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::COLOR_BIT;
            imageViewCI.baseMipLevel = 0;
            imageViewCI.levelCount = 1;
            imageViewCI.baseArrayLayer = 0;
            imageViewCI.layerCount = 2;
            colorImageViews.push_back(graphicsAPI->CreateImageView(imageViewCI));
        }

        GraphicsAPI::ImageViewCreateInfo imageViewCI;
        imageViewCI.image = depthImage;
        imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::DSV;
        imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D_ARRAY;
        imageViewCI.format = graphicsAPI->GetDepthFormat();
        imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::DEPTH_BIT;
        imageViewCI.baseMipLevel = 0;
        imageViewCI.levelCount = 1;
        imageViewCI.baseArrayLayer = 0;
        imageViewCI.layerCount = 2;
        depthImageView = graphicsAPI->CreateImageView(imageViewCI);
    }

    void DestroySwapchain() {
        graphicsAPI->DestroyImageView(depthImageView);
        for (void *&colorImageView : colorImageViews) {
            graphicsAPI->DestroyImage(colorImageView);
        }
        graphicsAPI->DestroyImage(depthImage);
        OPENXR_CHECK(xrDestroySwapchain(swapchain), "Failed to destroy Swapchain");
    }

    void RenderFrame() {
        if (XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2) {
            XrFrameState frameState{XR_TYPE_FRAME_STATE};
            XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
            OPENXR_CHECK(xrWaitFrame(session, &frameWaitInfo, &frameState), "Failed to wait for XR Frame.");

            XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
            OPENXR_CHECK(xrBeginFrame(session, &frameBeginInfo), "Failed to begin the XR Frame.");

            bool rendered = false;
            XrCompositionLayerBaseHeader *layers[1] = {nullptr};
            XrCompositionLayerProjection layerProjection{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
            std::vector<XrCompositionLayerProjectionView> layerProjectionViews;

            bool sessionActive = (sessionState == XR_SESSION_STATE_SYNCHRONIZED || sessionState == XR_SESSION_STATE_VISIBLE || sessionState == XR_SESSION_STATE_FOCUSED);
            if (sessionActive && frameState.shouldRender) {
                rendered = RenderLayer(frameState.predictedDisplayTime, layerProjection, layerProjectionViews);
                if (rendered) {
                    layers[0] = reinterpret_cast<XrCompositionLayerBaseHeader *>(&layerProjection);
                }
            }

            XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
            frameEndInfo.displayTime = frameState.predictedDisplayTime;
            frameEndInfo.environmentBlendMode = environmentBlendModes[0];
            if (rendered) {
                frameEndInfo.layerCount = 1;
                frameEndInfo.layers = reinterpret_cast<XrCompositionLayerBaseHeader **>(&layers);
            } else {
                frameEndInfo.layerCount = 0;
                frameEndInfo.layers = nullptr;
            }
            OPENXR_CHECK(xrEndFrame(session, &frameEndInfo), "Failed to end the XR Frame.");
        }
    }

    bool RenderLayer(const XrTime &predictedDisplayTime, XrCompositionLayerProjection &layerProjection, std::vector<XrCompositionLayerProjectionView> &layerProjectionViews) {
        std::vector<XrView> views(viewConfigurationViews.size(), {XR_TYPE_VIEW});

        XrViewState viewState = {XR_TYPE_VIEW_STATE};
        XrViewLocateInfo viewLocateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
        viewLocateInfo.viewConfigurationType = viewConfiguration;
        viewLocateInfo.displayTime = predictedDisplayTime;
        viewLocateInfo.space = space;
        uint32_t viewCount = 0;
        XrResult result = xrLocateViews(session, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());
        if (result != XR_SUCCESS) {
            std::cout << "Failed to locate Views." << std::endl;
            return false;
        }

        uint32_t imageIndex = 0;
        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        OPENXR_CHECK(xrAcquireSwapchainImage(swapchain, &acquireInfo, &imageIndex), "Failed to acquire Image from the Swapchian");

        XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        OPENXR_CHECK(xrWaitSwapchainImage(swapchain, &waitInfo), "Failed to wait for Image from the Swapchian");

        {
            const XrViewConfigurationView &viewConfigurationView = viewConfigurationViews[0];
            layerProjectionViews.resize(viewCount, {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW});
            for (uint32_t i = 0; i < viewCount; i++) {
                layerProjectionViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
                layerProjectionViews[i].pose = views[i].pose;
                layerProjectionViews[i].fov = views[i].fov;
                layerProjectionViews[i].subImage.swapchain = swapchain;
                layerProjectionViews[i].subImage.imageRect.offset.x = 0;
                layerProjectionViews[i].subImage.imageRect.offset.y = 0;
                layerProjectionViews[i].subImage.imageRect.extent.width = static_cast<int32_t>(viewConfigurationView.recommendedImageRectWidth);
                layerProjectionViews[i].subImage.imageRect.extent.height = static_cast<int32_t>(viewConfigurationView.recommendedImageRectHeight);
                layerProjectionViews[i].subImage.imageArrayIndex = i;
            };
            layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
            layerProjection.space = space;
            layerProjection.viewCount = static_cast<uint32_t>(layerProjectionViews.size());
            layerProjection.views = layerProjectionViews.data();

            graphicsAPI->BeginRendering();

            graphicsAPI->ClearColor(colorImageViews[imageIndex], 0.47f, 0.17f, 0.56f, 1.0f);
            graphicsAPI->ClearDepth(depthImageView, 1.0f);

            graphicsAPI->EndRendering();
        }

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        OPENXR_CHECK(xrReleaseSwapchainImage(swapchain, &releaseInfo), "Failed to release Image back to the Swapchian");

        return true;
    }

#if defined(__ANDROID__)
public:
    static android_app *app;

private:
    void PollAndroidEvents() {
        /*if (app->destroyRequested == 0) {
            applicationRunning = false;
            return;
        }*/
        while (true) {
            struct android_poll_source *source;
            int events;
            const int timeoutMilliseconds = (!sessionRunning && app->destroyRequested == 0) ? -1 : 0;
            if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void **)&source) >= 0) {
                if (source != nullptr) {
                    source->process(app, source);
                }
            } else {
                break;
            }
        }
    }
#endif

private:
    XrInstance instance = {};
    std::vector<const char *> activeAPILayers = {};
    std::vector<const char *> activeInstanceExtensions = {};
    std::vector<std::string> apiLayers = {};
    std::vector<std::string> instanceExtensions = {};

    XrDebugUtilsMessengerEXT debugUtilsMessenger = {};

    XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId systemID = {};

    GraphicsAPI_Type apiType = UNKNOWN;
    std::unique_ptr<GraphicsAPI> graphicsAPI = nullptr;

    XrViewConfigurationType viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    std::vector<XrViewConfigurationView> viewConfigurationViews;

    XrSession session = {};
    XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
    bool applicationRunning = true;
    bool sessionRunning = false;

    XrSwapchain swapchain{};
    int64_t swapchainFormat = 0;
    void *depthImage = nullptr;

    std::vector<void *> colorImageViews;
    void * depthImageView;

    std::vector<XrEnvironmentBlendMode> environmentBlendModes{};

    XrSpace space{};
};

void OpenXRTutorial_Main() {
    std::cout << "OpenXR Tutorial Chapter 3." << std::endl;

    OpenXRTutorialChapter3 app(OPENGL);
    app.Run();
}

#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
// XR_DOCS_TAG_BEGIN_main_WIN32___linux__
int main(int argc, char **argv) {
    OpenXRTutorial_Main();
}
// XR_DOCS_TAG_END_main_WIN32___linux__
#elif (__ANDROID__)
android_app *OpenXRTutorialChapter3::app = nullptr;
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

    OpenXRTutorialChapter3::app = app;
    OpenXRTutorial_Main();
}
// XR_DOCS_TAG_END_android_main___ANDROID__
#endif