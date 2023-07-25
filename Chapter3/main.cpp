// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

#include "DebugOutput.h"
#include "GraphicsAPIs.h"
#include "OpenXRDebugUtils.h"

#define XR_DOCS_CHAPTER_VERSION XR_DOCS_CHAPTER_3_2

#ifdef _MSC_VER
#define strncpy(dst, src, count) strcpy_s(dst, count, src);
#endif

class OpenXRTutorial {
public:
    OpenXRTutorial(GraphicsAPI_Type api)
        : apiType(api) {
        if (!CheckGraphicsAPI_TypeIsValidForPlatform(apiType)) {
            std::cout << "ERROR: The provided Graphics API is not valid for this platform." << std::endl;
            DEBUG_BREAK;
        }
    }
    ~OpenXRTutorial() = default;

    void Run() {
        CreateInstance();
        CreateDebugMessenger();

        GetInstanceProperties();
        GetSystemID();

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1
        GetViewConfigurationViews();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
        GetEnvironmentBlendModes();
#endif

        CreateSession();
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
        CreateReferenceSpace();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1
        CreateSwapchain();
#endif

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_3
        while (applicationRunning) {
            PollSystemEvents();
            PollEvents();
            if (sessionRunning) {
                RenderFrame();
            }
        }
#endif

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1
        DestroySwapchain();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
        DestroyReferenceSpace();
#endif
        DestroySession();

        DestroyDebugMessenger();
        DestroyInstance();
    }

private:
    void CreateInstance() {
        XrApplicationInfo AI;
        strncpy(AI.applicationName, "OpenXR Tutorial Chapter 3", XR_MAX_APPLICATION_NAME_SIZE);
        AI.applicationVersion = 1;
        strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
        AI.engineVersion = 1;
        AI.apiVersion = XR_CURRENT_API_VERSION;

        // Add additional instance layers/extensions
        {
            instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
            instanceExtensions.push_back(GetGraphicsAPIInstanceExtensionString(apiType));
        }

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

        XrInstanceCreateInfo instanceCI{XR_TYPE_INSTANCE_CREATE_INFO};
        instanceCI.createFlags = 0;
        instanceCI.applicationInfo = AI;
        instanceCI.enabledApiLayerCount = static_cast<uint32_t>(activeAPILayers.size());
        instanceCI.enabledApiLayerNames = activeAPILayers.data();
        instanceCI.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
        instanceCI.enabledExtensionNames = activeInstanceExtensions.data();
        OPENXR_CHECK(xrCreateInstance(&instanceCI, &xrInstance), "Failed to create Instance.");
    }

    void DestroyInstance() {
        OPENXR_CHECK(xrDestroyInstance(xrInstance), "Failed to destroy Instance.");
    }

    void CreateDebugMessenger() {
        if (IsStringInVector(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            debugUtilsMessenger = CreateOpenXRDebugUtilsMessenger(xrInstance);
        }
    }
    void DestroyDebugMessenger() {
        if (IsStringInVector(activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            DestroyOpenXRDebugUtilsMessenger(xrInstance, debugUtilsMessenger);
        }
    }

    void GetInstanceProperties() {
        XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
        OPENXR_CHECK(xrGetInstanceProperties(xrInstance, &instanceProperties), "Failed to get InstanceProperties.");

        std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - ";
        std::cout << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << ".";
        std::cout << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << ".";
        std::cout << XR_VERSION_PATCH(instanceProperties.runtimeVersion) << std::endl;
    }

    void GetSystemID() {
        XrSystemGetInfo systemGI{XR_TYPE_SYSTEM_GET_INFO};
        systemGI.formFactor = formFactor;
        OPENXR_CHECK(xrGetSystem(xrInstance, &systemGI, &systemID), "Failed to get SystemID.");

        XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
        OPENXR_CHECK(xrGetSystemProperties(xrInstance, systemID, &systemProperties), "Failed to get SystemProperties.");
    }

    void GetEnvironmentBlendModes() {
        uint32_t environmentBlendModeSize = 0;
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(xrInstance, systemID, viewConfiguration, 0, &environmentBlendModeSize, nullptr), "Failed to enumerate ViewConfigurationViews.");
        environmentBlendModes.resize(environmentBlendModeSize);
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(xrInstance, systemID, viewConfiguration, environmentBlendModeSize, &environmentBlendModeSize, environmentBlendModes.data()), "Failed to enumerate ViewConfigurationViews.");
    }

    // XR_DOCS_TAG_BEGIN_GetViewConfigurationViews
    void GetViewConfigurationViews() {
        uint32_t viewConfigurationViewSize = 0;
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(xrInstance, systemID, viewConfiguration, 0, &viewConfigurationViewSize, nullptr), "Failed to enumerate ViewConfigurationViews.");
        viewConfigurationViews.resize(viewConfigurationViewSize, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(xrInstance, systemID, viewConfiguration, viewConfigurationViewSize, &viewConfigurationViewSize, viewConfigurationViews.data()), "Failed to enumerate ViewConfigurationViews.");
    }
    // XR_DOCS_TAG_END_GetViewConfigurationViews

    void CreateSession() {
        XrSessionCreateInfo sessionCI{XR_TYPE_SESSION_CREATE_INFO};

        if (apiType == D3D11) {
#if defined(XR_USE_GRAPHICS_API_D3D11)
            graphicsAPI = std::make_unique<GraphicsAPI_D3D11>(xrInstance, systemID);
#endif
        } else if (apiType == D3D12) {
#if defined(XR_USE_GRAPHICS_API_D3D12)
            graphicsAPI = std::make_unique<GraphicsAPI_D3D12>(xrInstance, systemID);
#endif
        } else if (apiType == OPENGL) {
#if defined(XR_USE_GRAPHICS_API_OPENGL)
            graphicsAPI = std::make_unique<GraphicsAPI_OpenGL>(xrInstance, systemID);
#endif
        } else if (apiType == OPENGL_ES) {
#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
            graphicsAPI = std::make_unique<GraphicsAPI_OpenGL_ES>(xrInstance, systemID);
#endif
        } else if (apiType == VULKAN) {
#if defined(XR_USE_GRAPHICS_API_VULKAN)
            graphicsAPI = std::make_unique<GraphicsAPI_Vulkan>(xrInstance, systemID);
#endif
        } else {
            std::cout << "ERROR: Unknown Graphics API." << std::endl;
            DEBUG_BREAK;
        }
        sessionCI.next = graphicsAPI->GetGraphicsBinding();
        sessionCI.createFlags = 0;
        sessionCI.systemId = systemID;

        OPENXR_CHECK(xrCreateSession(xrInstance, &sessionCI, &session), "Failed to create Session.");
    }

    void DestroySession() {
        OPENXR_CHECK(xrDestroySession(session), "Failed to destroy Session.");
    }

    void PollEvents() {
        XrResult result = XR_SUCCESS;
        do {
            XrEventDataBuffer eventData{XR_TYPE_EVENT_DATA_BUFFER};
            result = xrPollEvent(xrInstance, &eventData);

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
    
// XR_DOCS_TAG_BEGIN_CreateReferenceSpace
    void CreateReferenceSpace() {
        XrReferenceSpaceCreateInfo referenceSpaceCI{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        referenceSpaceCI.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
        OPENXR_CHECK(xrCreateReferenceSpace(session, &referenceSpaceCI, &localOrStageSpace), "Failed to create ReferenceSpace.");
    }
// XR_DOCS_TAG_END_CreateReferenceSpace

// XR_DOCS_TAG_BEGIN_DestroyReferenceSpace
    void DestroyReferenceSpace() {
        OPENXR_CHECK(xrDestroySpace(localOrStageSpace), "Failed to destroy Space.")
    }
// XR_DOCS_TAG_END_DestroyReferenceSpace

    void CreateSwapchain() {
// XR_DOCS_TAG_BEGIN_EnumerateSwapchainFormats
        uint32_t formatSize = 0;
        OPENXR_CHECK(xrEnumerateSwapchainFormats(session, 0, &formatSize, nullptr), "Failed to enumerate Swapchain Formats");
        std::vector<int64_t> formats(formatSize);
        OPENXR_CHECK(xrEnumerateSwapchainFormats(session, formatSize, &formatSize, formats.data()), "Failed to enumerate Swapchain Formats");
// XR_DOCS_TAG_END_EnumerateSwapchainFormats

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

        swapchainAndDepthImages.resize(viewConfigurationViews.size());
        for (SwapchainAndDepthImage &swapchainAndDepthImage : swapchainAndDepthImages) {
            // XR_DOCS_TAG_BEGIN_CreateSwapchain
            XrSwapchainCreateInfo swapchainCI{XR_TYPE_SWAPCHAIN_CREATE_INFO};
            swapchainCI.createFlags = 0;
            swapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            swapchainCI.format = graphicsAPI->SelectSwapchainFormat(formats);
            swapchainCI.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount;
            swapchainCI.width = viewConfigurationView.recommendedImageRectWidth;
            swapchainCI.height = viewConfigurationView.recommendedImageRectHeight;
            swapchainCI.faceCount = 1;
            swapchainCI.arraySize = 1;
            swapchainCI.mipCount = 1;
            OPENXR_CHECK(xrCreateSwapchain(session, &swapchainCI, &swapchainAndDepthImage.swapchain), "Failed to create Swapchain");
            swapchainAndDepthImage.swapchainFormat = swapchainCI.format;
            // XR_DOCS_TAG_END_CreateSwapchain

            // XR_DOCS_TAG_BEGIN_EnumerateSwapchainImages
            uint32_t swapchainImageCount = 0;
            OPENXR_CHECK(xrEnumerateSwapchainImages(swapchainAndDepthImage.swapchain, 0, &swapchainImageCount, nullptr), "Failed to enumerate Swapchain Images.");
            XrSwapchainImageBaseHeader *swapchainImages = graphicsAPI->AllocateSwapchainImageData(swapchainImageCount);
            OPENXR_CHECK(xrEnumerateSwapchainImages(swapchainAndDepthImage.swapchain, swapchainImageCount, &swapchainImageCount, swapchainImages), "Failed to enumerate Swapchain Images.");
            // XR_DOCS_TAG_END_EnumerateSwapchainImages

            // XR_DOCS_TAG_BEGIN_CreateDepthImage
            GraphicsAPI::ImageCreateInfo depthImageCI;
            depthImageCI.dimension = 2;
            depthImageCI.width = viewConfigurationView.recommendedImageRectWidth;
            depthImageCI.height = viewConfigurationView.recommendedImageRectHeight;
            depthImageCI.depth = 1;
            depthImageCI.mipLevels = 1;
            depthImageCI.arrayLayers = 1;
            depthImageCI.sampleCount = 1;
            depthImageCI.format = graphicsAPI->GetDepthFormat();
            depthImageCI.cubemap = false;
            depthImageCI.colorAttachment = false;
            depthImageCI.depthAttachment = true;
            depthImageCI.sampled = false;
            swapchainAndDepthImage.depthImage = graphicsAPI->CreateImage(depthImageCI);
            // XR_DOCS_TAG_END_CreateDepthImage

            // XR_DOCS_TAG_BEGIN_CreateImageViews
            for (uint32_t i = 0; i < swapchainImageCount; i++) {
                GraphicsAPI::ImageViewCreateInfo imageViewCI;
                imageViewCI.image = graphicsAPI->GetSwapchainImage(i);
                imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::RTV;
                imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
                imageViewCI.format = swapchainAndDepthImage.swapchainFormat;
                imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::COLOR_BIT;
                imageViewCI.baseMipLevel = 0;
                imageViewCI.levelCount = 1;
                imageViewCI.baseArrayLayer = 0;
                imageViewCI.layerCount = 1;
                swapchainAndDepthImage.colorImageViews.push_back(graphicsAPI->CreateImageView(imageViewCI));
            }

            GraphicsAPI::ImageViewCreateInfo imageViewCI;
            imageViewCI.image = swapchainAndDepthImage.depthImage;
            imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::DSV;
            imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
            imageViewCI.format = graphicsAPI->GetDepthFormat();
            imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::DEPTH_BIT;
            imageViewCI.baseMipLevel = 0;
            imageViewCI.levelCount = 1;
            imageViewCI.baseArrayLayer = 0;
            imageViewCI.layerCount = 1;
            swapchainAndDepthImage.depthImageView = graphicsAPI->CreateImageView(imageViewCI);
            // XR_DOCS_TAG_END_CreateImageViews
        }
    }

    // XR_DOCS_TAG_BEGIN_DestroySwapchain
    void DestroySwapchain() {
        for (SwapchainAndDepthImage &swapchainAndDepthImage : swapchainAndDepthImages) {
            graphicsAPI->DestroyImageView(swapchainAndDepthImage.depthImageView);
            for (void *&colorImageView : swapchainAndDepthImage.colorImageViews) {
                graphicsAPI->DestroyImageView(colorImageView);
            }

            graphicsAPI->DestroyImage(swapchainAndDepthImage.depthImage);

            OPENXR_CHECK(xrDestroySwapchain(swapchainAndDepthImage.swapchain), "Failed to destroy Swapchain");
        }
    }
    // XR_DOCS_TAG_END_DestroySwapchain

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
        viewLocateInfo.space = localOrStageSpace;
        uint32_t viewCount = 0;
        XrResult result = xrLocateViews(session, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());
        if (result != XR_SUCCESS) {
            std::cout << "Failed to locate Views." << std::endl;
            return false;
        }

        layerProjectionViews.resize(viewCount, {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW});
        for (uint32_t i = 0; i < viewCount; i++) {
            uint32_t imageIndex = 0;
            XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
            OPENXR_CHECK(xrAcquireSwapchainImage(swapchainAndDepthImages[i].swapchain, &acquireInfo, &imageIndex), "Failed to acquire Image from the Swapchian");

            XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
            waitInfo.timeout = XR_INFINITE_DURATION;
            OPENXR_CHECK(xrWaitSwapchainImage(swapchainAndDepthImages[i].swapchain, &waitInfo), "Failed to wait for Image from the Swapchain");

            const uint32_t &width = viewConfigurationViews[i].recommendedImageRectWidth;
            const uint32_t &height = viewConfigurationViews[i].recommendedImageRectHeight;
            GraphicsAPI::Viewport viewport = {0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
            GraphicsAPI::Rect2D scissor = {{(int32_t)0, (int32_t)0}, {width, height}};

            layerProjectionViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
            layerProjectionViews[i].pose = views[i].pose;
            layerProjectionViews[i].fov = views[i].fov;
            layerProjectionViews[i].subImage.swapchain = swapchainAndDepthImages[i].swapchain;
            layerProjectionViews[i].subImage.imageRect.offset.x = 0;
            layerProjectionViews[i].subImage.imageRect.offset.y = 0;
            layerProjectionViews[i].subImage.imageRect.extent.width = static_cast<int32_t>(width);
            layerProjectionViews[i].subImage.imageRect.extent.height = static_cast<int32_t>(height);
            layerProjectionViews[i].subImage.imageArrayIndex = 0;

            graphicsAPI->BeginRendering();

            graphicsAPI->ClearColor(swapchainAndDepthImages[i].colorImageViews[imageIndex], 0.17f, 0.17f, 0.17f, 1.0f);
            graphicsAPI->ClearDepth(swapchainAndDepthImages[i].depthImageView, 1.0f);

            graphicsAPI->EndRendering();

            XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
            OPENXR_CHECK(xrReleaseSwapchainImage(swapchainAndDepthImages[i].swapchain, &releaseInfo), "Failed to release Image back to the Swapchian");
        };
        layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
        layerProjection.space = localOrStageSpace;
        layerProjection.viewCount = static_cast<uint32_t>(layerProjectionViews.size());
        layerProjection.views = layerProjectionViews.data();

        return true;
    }

#if defined(__ANDROID__)
public:
    static android_app *androidApp;

    // Modified from https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/d6b6d7a10bdcf8d4fe806b4f415fde3dd5726878/src/tests/hello_xr/main.cpp#L133C1-L189C2
    struct AndroidAppState {
        ANativeWindow *nativeWindow = nullptr;
        bool resumed = false;
    };
    static AndroidAppState androidAppState;

    // Process the next main command.
    static void AndroidAppHandleCmd(struct android_app *app, int32_t cmd) {
        AndroidAppState *appState = (AndroidAppState *)app->userData;

        switch (cmd) {
        // There is no APP_CMD_CREATE. The ANativeActivity creates the application thread from onCreate().
        // The application thread then calls android_main().
        case APP_CMD_START: {
            break;
        }
        case APP_CMD_RESUME: {
            appState->resumed = true;
            break;
        }
        case APP_CMD_PAUSE: {
            appState->resumed = false;
            break;
        }
        case APP_CMD_STOP: {
            break;
        }
        case APP_CMD_DESTROY: {
            appState->nativeWindow = nullptr;
            break;
        }
        case APP_CMD_INIT_WINDOW: {
            appState->nativeWindow = app->window;
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            appState->nativeWindow = nullptr;
            break;
        }
        }
    }

private:
    void PollSystemEvents() {
        if (androidApp->destroyRequested != 0) {
            applicationRunning = false;
            return;
        }
        while (true) {
            struct android_poll_source *source = nullptr;
            int events = 0;
            const int timeoutMilliseconds = (!androidAppState.resumed && !sessionRunning && androidApp->destroyRequested == 0) ? -1 : 0;
            if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void **)&source) >= 0) {
                if (source != nullptr) {
                    source->process(androidApp, source);
                }
            } else {
                break;
            }
        }
    }
#else
    void PollSystemEvents() {
        return;
    }
#endif

private:
    XrInstance xrInstance = {};
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

    struct SwapchainAndDepthImage {
        XrSwapchain swapchain{};
        int64_t swapchainFormat = 0;
        void *depthImage = nullptr;

        std::vector<void *> colorImageViews;
        void *depthImageView = nullptr;
    };
    std::vector<SwapchainAndDepthImage> swapchainAndDepthImages;

    std::vector<XrEnvironmentBlendMode> environmentBlendModes{};

    XrSpace localOrStageSpace{};
};

void OpenXRTutorial_Main() {
    DebugOutput debugOutput;
    std::cout << "OpenXR Tutorial Chapter 3." << std::endl;
    OpenXRTutorial app(VULKAN);
    app.Run();
}

#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
// XR_DOCS_TAG_BEGIN_main_WIN32___linux__
int main(int argc, char **argv) {
    OpenXRTutorial_Main();
}
// XR_DOCS_TAG_END_main_WIN32___linux__
#elif (__ANDROID__)
android_app *OpenXRTutorial::androidApp = nullptr;
OpenXRTutorial::AndroidAppState OpenXRTutorial::androidAppState = {};

// XR_DOCS_TAG_BEGIN_android_main___ANDROID__
void android_main(struct android_app *app) {
    // Allow interaction with JNI and the JVM on this thread.
    // https://developer.android.com/training/articles/perf-jni#threads
    JNIEnv *env;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    XrInstance xrInstance = {};  // Dummy XrInstance variable for OPENXR_CHECK macro.
    PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR;
    OPENXR_CHECK(xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction *)&xrInitializeLoaderKHR), "Failed to get InstanceProcAddr.");
    if (!xrInitializeLoaderKHR) {
        return;
    }

    XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid{XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR};
    loaderInitializeInfoAndroid.applicationVM = app->activity->vm;
    loaderInitializeInfoAndroid.applicationContext = app->activity->clazz;
    OPENXR_CHECK(xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR *)&loaderInitializeInfoAndroid), "Failed to initialise Loader for Android.");

    app->userData = &OpenXRTutorial::androidAppState;
    app->onAppCmd = OpenXRTutorial::AndroidAppHandleCmd;

    OpenXRTutorial::androidApp = app;
    OpenXRTutorial_Main();
}
// XR_DOCS_TAG_END_android_main___ANDROID__
#endif