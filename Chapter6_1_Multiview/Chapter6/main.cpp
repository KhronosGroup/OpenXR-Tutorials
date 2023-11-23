// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#include <DebugOutput.h>
// XR_DOCS_TAG_BEGIN_include_GraphicsAPI_D3D11
#include <GraphicsAPI_D3D11.h>
// XR_DOCS_TAG_END_include_GraphicsAPI_D3D11
// XR_DOCS_TAG_BEGIN_include_GraphicsAPI_D3D12
#include <GraphicsAPI_D3D12.h>
// XR_DOCS_TAG_END_include_GraphicsAPI_D3D12
// XR_DOCS_TAG_BEGIN_include_GraphicsAPI_OpenGL
#include <GraphicsAPI_OpenGL.h>
// XR_DOCS_TAG_END_include_GraphicsAPI_OpenGL
// XR_DOCS_TAG_BEGIN_include_GraphicsAPI_OpenGL_ES
#include <GraphicsAPI_OpenGL_ES.h>
// XR_DOCS_TAG_END_include_GraphicsAPI_OpenGL_ES
// XR_DOCS_TAG_BEGIN_include_GraphicsAPI_Vulkan
#include <GraphicsAPI_Vulkan.h>
// XR_DOCS_TAG_END_include_GraphicsAPI_Vulkan
// XR_DOCS_TAG_BEGIN_include_OpenXRDebugUtils
#include <OpenXRDebugUtils.h>
// XR_DOCS_TAG_END_include_OpenXRDebugUtils

// XR_DOCS_TAG_BEGIN_include_linear_algebra
// include xr linear algebra for XrVector and XrMatrix classes.
#include <xr_linear_algebra.h>
// Declare some useful operators for vectors:
XrVector3f operator-(XrVector3f a, XrVector3f b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
XrVector3f operator*(XrVector3f a, float b) {
    return {a.x * b, a.y * b, a.z * b};
}
// XR_DOCS_TAG_END_include_linear_algebra
// XR_DOCS_TAG_BEGIN_include_algorithm_random
// Include <algorithm> for std::min and max
#include <algorithm>
// Random numbers for colorful blocks
#include <random>
static std::uniform_real_distribution<float> pseudorandom_distribution(0, 1.f);
static std::mt19937 pseudo_random_generator;
// XR_DOCS_TAG_END_include_algorithm_random

#define XR_DOCS_CHAPTER_VERSION XR_DOCS_CHAPTER_4_5

class OpenXRTutorial {
private:
    struct RenderLayerInfo;

public:
    OpenXRTutorial(GraphicsAPI_Type apiType)
        : m_apiType(apiType) {
        // Check API compatibility with Platform.
        if (!CheckGraphicsAPI_TypeIsValidForPlatform(m_apiType)) {
            XR_TUT_LOG_ERROR("ERROR: The provided Graphics API is not valid for this platform.");
            DEBUG_BREAK;
        }
    }
    ~OpenXRTutorial() = default;

    void Run() {
        CreateInstance();
        CreateDebugMessenger();

        GetInstanceProperties();
        GetSystemID();
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_4_1
        // XR_DOCS_TAG_BEGIN_CallCreateActionSet
        CreateActionSet();
        // XR_DOCS_TAG_END_CallCreateActionSet
        // XR_DOCS_TAG_BEGIN_CallSuggestBindings
        SuggestBindings();
        // XR_DOCS_TAG_END_CallSuggestBindings
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1
        GetViewConfigurationViews();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
        GetEnvironmentBlendModes();
#endif

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_2
        CreateSession();

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_4_3
        // XR_DOCS_TAG_BEGIN_CallCreateActionPoses
        CreateActionPoses();
        AttachActionSet();
        // XR_DOCS_TAG_END_CallCreateActionPoses
#endif

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
        CreateReferenceSpace();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1
        CreateSwapchains();
#endif
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_3
        CreateResources();
#endif

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_3
        while (m_applicationRunning) {
            PollSystemEvents();
            PollEvents();
            if (m_sessionRunning) {
                RenderFrame();
            }
        }
#endif

#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_1
        DestroySwapchains();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
        DestroyReferenceSpace();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_3
        DestroyResources();
#endif
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_2
        DestroySession();
#endif

        DestroyDebugMessenger();
        DestroyInstance();
    }

private:
    void CreateInstance() {
        // Fill out an XrApplicationInfo structure detailing the names and OpenXR version.
        // The application/engine name and version are user-definied. These may help IHVs or runtimes.
        // XR_DOCS_TAG_BEGIN_XrApplicationInfo
        XrApplicationInfo AI;
        strncpy(AI.applicationName, "OpenXR Tutorial Chapter 4", XR_MAX_APPLICATION_NAME_SIZE);
        AI.applicationVersion = 1;
        strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
        AI.engineVersion = 1;
        AI.apiVersion = XR_CURRENT_API_VERSION;
        // XR_DOCS_TAG_END_XrApplicationInfo

        // Add additional instance layers/extensions that the application wants.
        // Add both required and requested instance extensions.
        {
            // XR_DOCS_TAG_BEGIN_instanceExtensions
            m_instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
            // Ensure m_apiType is already defined when we call this line.
            m_instanceExtensions.push_back(GetGraphicsAPIInstanceExtensionString(m_apiType));
            // XR_DOCS_TAG_END_instanceExtensions
        }

        // XR_DOCS_TAG_BEGIN_find_apiLayer_extension
        // Get all the API Layers from the OpenXR runtime.
        uint32_t apiLayerCount = 0;
        std::vector<XrApiLayerProperties> apiLayerProperties;
        OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
        apiLayerProperties.resize(apiLayerCount, {XR_TYPE_API_LAYER_PROPERTIES});
        OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");

        // Check the requested API layers against the ones from the OpenXR. If found add it to the Active API Layers.
        for (auto &requestLayer : m_apiLayers) {
            for (auto &layerProperty : apiLayerProperties) {
                // strcmp returns 0 if the strings match.
                if (strcmp(requestLayer.c_str(), layerProperty.layerName) != 0) {
                    continue;
                } else {
                    m_activeAPILayers.push_back(requestLayer.c_str());
                    break;
                }
            }
        }

        // Get all the Instance Extensions from the OpenXR instance.
        uint32_t extensionCount = 0;
        std::vector<XrExtensionProperties> extensionProperties;
        OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");
        extensionProperties.resize(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
        OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");

        // Check the requested Instance Extensions against the ones from the OpenXR runtime.
        // If an extension is found add it to Active Instance Extensions.
        // Log error if the Instance Extension is not found.
        for (auto &requestedInstanceExtension : m_instanceExtensions) {
            bool found = false;
            for (auto &extensionProperty : extensionProperties) {
                // strcmp returns 0 if the strings match.
                if (strcmp(requestedInstanceExtension.c_str(), extensionProperty.extensionName) != 0) {
                    continue;
                } else {
                    m_activeInstanceExtensions.push_back(requestedInstanceExtension.c_str());
                    found = true;
                    break;
                }
            }
            if (!found) {
                XR_TUT_LOG_ERROR("Failed to find OpenXR instance extension: " << requestedInstanceExtension);
            }
        }
        // XR_DOCS_TAG_END_find_apiLayer_extension

        // Fill out an XrInstanceCreateInfo structure and create an XrInstance.
        // XR_DOCS_TAG_BEGIN_XrInstanceCreateInfo
        XrInstanceCreateInfo instanceCI{XR_TYPE_INSTANCE_CREATE_INFO};
        instanceCI.createFlags = 0;
        instanceCI.applicationInfo = AI;
        instanceCI.enabledApiLayerCount = static_cast<uint32_t>(m_activeAPILayers.size());
        instanceCI.enabledApiLayerNames = m_activeAPILayers.data();
        instanceCI.enabledExtensionCount = static_cast<uint32_t>(m_activeInstanceExtensions.size());
        instanceCI.enabledExtensionNames = m_activeInstanceExtensions.data();
        OPENXR_CHECK(xrCreateInstance(&instanceCI, &m_xrInstance), "Failed to create Instance.");
        // XR_DOCS_TAG_END_XrInstanceCreateInfo
    }

    void DestroyInstance() {
        // Destroy the XrInstance.
        // XR_DOCS_TAG_BEGIN_XrInstanceDestroy
        OPENXR_CHECK(xrDestroyInstance(m_xrInstance), "Failed to destroy Instance.");
        // XR_DOCS_TAG_END_XrInstanceDestroy
    }

    void CreateDebugMessenger() {
        // XR_DOCS_TAG_BEGIN_CreateDebugMessenger
        // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before creating an XrDebugUtilsMessengerEXT.
        if (IsStringInVector(m_activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            m_debugUtilsMessenger = CreateOpenXRDebugUtilsMessenger(m_xrInstance);  // From OpenXRDebugUtils.h.
        }
        // XR_DOCS_TAG_END_CreateDebugMessenger
    }
    void DestroyDebugMessenger() {
        // XR_DOCS_TAG_BEGIN_DestroyDebugMessenger
        // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before destroying the XrDebugUtilsMessengerEXT.
        if (m_debugUtilsMessenger != XR_NULL_HANDLE) {
            DestroyOpenXRDebugUtilsMessenger(m_xrInstance, m_debugUtilsMessenger);  // From OpenXRDebugUtils.h.
        }
        // XR_DOCS_TAG_END_DestroyDebugMessenger
    }

    void GetInstanceProperties() {
        // Get the instance's properties and log the runtime name and version.
        // XR_DOCS_TAG_BEGIN_GetInstanceProperties
        XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
        OPENXR_CHECK(xrGetInstanceProperties(m_xrInstance, &instanceProperties), "Failed to get InstanceProperties.");

        XR_TUT_LOG("OpenXR Runtime: " << instanceProperties.runtimeName << " - "
                                    << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "."
                                    << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << "."
                                    << XR_VERSION_PATCH(instanceProperties.runtimeVersion));
        // XR_DOCS_TAG_END_GetInstanceProperties
    }

    void GetSystemID() {
        // XR_DOCS_TAG_BEGIN_GetSystemID
        // Get the XrSystemId from the instance and the supplied XrFormFactor.
        XrSystemGetInfo systemGI{XR_TYPE_SYSTEM_GET_INFO};
        systemGI.formFactor = m_formFactor;
        OPENXR_CHECK(xrGetSystem(m_xrInstance, &systemGI, &m_systemID), "Failed to get SystemID.");

        // Get the System's properties for some general information about the hardware and the vendor.
        OPENXR_CHECK(xrGetSystemProperties(m_xrInstance, m_systemID, &m_systemProperties), "Failed to get SystemProperties.");
        // XR_DOCS_TAG_END_GetSystemID
    }

    // XR_DOCS_TAG_BEGIN_CreateXrPath
    XrPath CreateXrPath(const char *path_string) {
        XrPath xrPath;
        OPENXR_CHECK(xrStringToPath(m_xrInstance, path_string, &xrPath), "Failed to create XrPath from string.");
        return xrPath;
    }
    std::string FromXrPath(XrPath path) {
        uint32_t strl;
        char text[XR_MAX_PATH_LENGTH];
        XrResult res;
        res = xrPathToString(m_xrInstance, path, XR_MAX_PATH_LENGTH, &strl, text);
        std::string str;
        if (res == XR_SUCCESS) {
            str = text;
        } else {
            OPENXR_CHECK(res, "Failed to retrieve path.");
        }
        return str;
    }
    // XR_DOCS_TAG_END_CreateXrPath
    // XR_DOCS_TAG_BEGIN_CreateActionSet
    void CreateActionSet() {
        XrActionSetCreateInfo actionSetCI{XR_TYPE_ACTION_SET_CREATE_INFO};
        // The internal name the runtime uses for this Action Set.
        strncpy(actionSetCI.actionSetName, "openxr-tutorial-actionset", XR_MAX_ACTION_SET_NAME_SIZE);
        // Localized names are required so there is a human-readable action name to show the user if they are rebinding Actions in an options screen.
        strncpy(actionSetCI.localizedActionSetName, "OpenXR Tutorial ActionSet", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
        OPENXR_CHECK(xrCreateActionSet(m_xrInstance, &actionSetCI, &m_actionSet), "Failed to create ActionSet.");
        // Set a priority: this comes into play when we have multiple Action Sets, and determines which Action takes priority in binding to a specific input.
        actionSetCI.priority = 0;
        // XR_DOCS_TAG_END_CreateActionSet

        // XR_DOCS_TAG_BEGIN_CreateActionLambda
        auto CreateAction = [this](XrAction &xrAction, const char *name, XrActionType xrActionType, std::vector<const char *> subaction_paths = {}) -> void {
            XrActionCreateInfo actionCI{XR_TYPE_ACTION_CREATE_INFO};
            // The type of action: float input, pose, haptic output etc.
            actionCI.actionType = xrActionType;
            // Subaction paths, e.g. left and right hand. To distinguish the same action performed on different devices.
            std::vector<XrPath> subaction_xrpaths;
            for (auto p : subaction_paths) {
                subaction_xrpaths.push_back(CreateXrPath(p));
            }
            actionCI.countSubactionPaths = (uint32_t)subaction_xrpaths.size();
            actionCI.subactionPaths = subaction_xrpaths.data();
            // The internal name the runtime uses for this Action.
            strncpy(actionCI.actionName, name, XR_MAX_ACTION_NAME_SIZE);
            // Localized names are required so there is a human-readable action name to show the user if they are rebinding the Action in an options screen.
            strncpy(actionCI.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE);
            OPENXR_CHECK(xrCreateAction(m_actionSet, &actionCI, &xrAction), "Failed to create Action.");
        };
        // XR_DOCS_TAG_END_CreateActionLambda
        // XR_DOCS_TAG_BEGIN_CreateActions
        // An Action for grabbing cubes.
        CreateAction(m_grabCubeAction, "grab-cube", XR_ACTION_TYPE_FLOAT_INPUT, {"/user/hand/left", "/user/hand/right"});
        CreateAction(m_spawnCubeAction, "spawn-cube", XR_ACTION_TYPE_BOOLEAN_INPUT);
        CreateAction(m_changeColorAction, "change-color", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/left", "/user/hand/right"});
        // An Action for the position of the palm of the user's hand - appropriate for the location of a grabbing Actions.
        CreateAction(m_palmPoseAction, "palm-pose", XR_ACTION_TYPE_POSE_INPUT, {"/user/hand/left", "/user/hand/right"});
        // An Action for a vibration output on one or other hand.
        CreateAction(m_buzzAction, "buzz", XR_ACTION_TYPE_VIBRATION_OUTPUT, {"/user/hand/left", "/user/hand/right"});
        // For later convenience we create the XrPaths for the subaction path names.
        m_handPaths[0] = CreateXrPath("/user/hand/left");
        m_handPaths[1] = CreateXrPath("/user/hand/right");
    }
    // XR_DOCS_TAG_END_CreateActions

    // XR_DOCS_TAG_BEGIN_SuggestBindings1
    void SuggestBindings() {
        auto SuggestBindings = [this](const char *profile_path, std::vector<XrActionSuggestedBinding> bindings) -> bool {
            // The application can call xrSuggestInteractionProfileBindings once per interaction profile that it supports.
            XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
            interactionProfileSuggestedBinding.interactionProfile = CreateXrPath(profile_path);
            interactionProfileSuggestedBinding.suggestedBindings = bindings.data();
            interactionProfileSuggestedBinding.countSuggestedBindings = (uint32_t)bindings.size();
            if (xrSuggestInteractionProfileBindings(m_xrInstance, &interactionProfileSuggestedBinding) == XrResult::XR_SUCCESS)
                return true;
            XR_TUT_LOG("Failed to suggest bindings with " << profile_path);
            return false;
        };
        // XR_DOCS_TAG_END_SuggestBindings1
        // XR_DOCS_TAG_BEGIN_SuggestBindings2
        bool any_ok = false;
        // Each Action here has two paths, one for each SubAction path.
        any_ok |= SuggestBindings("/interaction_profiles/khr/simple_controller", {{m_changeColorAction, CreateXrPath("/user/hand/left/input/select/click")},
                                                                                  {m_grabCubeAction, CreateXrPath("/user/hand/right/input/select/click")},
                                                                                  {m_palmPoseAction, CreateXrPath("/user/hand/left/input/grip/pose")},
                                                                                  {m_palmPoseAction, CreateXrPath("/user/hand/right/input/grip/pose")},
                                                                                  {m_buzzAction, CreateXrPath("/user/hand/left/output/haptic")},
                                                                                  {m_buzzAction, CreateXrPath("/user/hand/right/output/haptic")}});
        // XR_DOCS_TAG_END_SuggestBindings2
        // XR_DOCS_TAG_BEGIN_SuggestTouchNativeBindings
        // Each Action here has two paths, one for each SubAction path.
        any_ok |= SuggestBindings("/interaction_profiles/oculus/touch_controller", {{m_grabCubeAction, CreateXrPath("/user/hand/left/input/squeeze/value")},
                                                                                    {m_grabCubeAction, CreateXrPath("/user/hand/right/input/squeeze/value")},
                                                                                    {m_spawnCubeAction, CreateXrPath("/user/hand/right/input/a/click")},
                                                                                    {m_changeColorAction, CreateXrPath("/user/hand/left/input/trigger/value")},
                                                                                    {m_changeColorAction, CreateXrPath("/user/hand/right/input/trigger/value")},
                                                                                    {m_palmPoseAction, CreateXrPath("/user/hand/left/input/grip/pose")},
                                                                                    {m_palmPoseAction, CreateXrPath("/user/hand/right/input/grip/pose")},
                                                                                    {m_buzzAction, CreateXrPath("/user/hand/left/output/haptic")},
                                                                                    {m_buzzAction, CreateXrPath("/user/hand/right/output/haptic")}});
        // XR_DOCS_TAG_END_SuggestTouchNativeBindings
        // XR_DOCS_TAG_BEGIN_SuggestBindings3
        if (!any_ok) {
            DEBUG_BREAK;
        }
    }
    void RecordCurrentBindings() {
        if (m_session) {
            // now we are ready to:
            XrInteractionProfileState interactionProfile = {XR_TYPE_INTERACTION_PROFILE_STATE, 0, 0};
            // for each action, what is the binding?
            OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[0], &interactionProfile), "Failed to get profile.");
            if (interactionProfile.interactionProfile) {
                XR_TUT_LOG("user/hand/left ActiveProfile " << FromXrPath(interactionProfile.interactionProfile).c_str());
            }
            OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[1], &interactionProfile), "Failed to get profile.");
            if (interactionProfile.interactionProfile) {
                XR_TUT_LOG("user/hand/right ActiveProfile " << FromXrPath(interactionProfile.interactionProfile).c_str());
            }
        }
    }
    // XR_DOCS_TAG_END_SuggestBindings3
    // XR_DOCS_TAG_BEGIN_CreateActionPoses
    void CreateActionPoses() {
        // Create an xrSpace for a pose action.
        auto CreateActionPoseSpace = [this](XrSession session, XrAction xrAction, const char *subaction_path = nullptr) -> XrSpace {
            XrSpace xrSpace;
            const XrPosef xrPoseIdentity = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
            // Create frame of reference for a pose action
            XrActionSpaceCreateInfo actionSpaceCI{XR_TYPE_ACTION_SPACE_CREATE_INFO};
            actionSpaceCI.action = xrAction;
            actionSpaceCI.poseInActionSpace = xrPoseIdentity;
            if (subaction_path)
                actionSpaceCI.subactionPath = CreateXrPath(subaction_path);
            OPENXR_CHECK(xrCreateActionSpace(session, &actionSpaceCI, &xrSpace), "Failed to create ActionSpace.");
            return xrSpace;
        };
        m_handPoseSpace[0] = CreateActionPoseSpace(m_session, m_palmPoseAction, "/user/hand/left");
        m_handPoseSpace[1] = CreateActionPoseSpace(m_session, m_palmPoseAction, "/user/hand/right");
    }
    // XR_DOCS_TAG_END_CreateActionPoses
    // XR_DOCS_TAG_BEGIN_AttachActionSet
    void AttachActionSet() {
        // Attach the action set we just made to the session. We could attach multiple action sets!
        XrSessionActionSetsAttachInfo actionSetAttachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
        actionSetAttachInfo.countActionSets = 1;
        actionSetAttachInfo.actionSets = &m_actionSet;
        OPENXR_CHECK(xrAttachSessionActionSets(m_session, &actionSetAttachInfo), "Failed to attach ActionSet to Session.");
    }
    // XR_DOCS_TAG_END_AttachActionSet

    void GetEnvironmentBlendModes() {
        // XR_DOCS_TAG_BEGIN_GetEnvironmentBlendModes
        // Retrieves the available blend modes. The first call gets the count of the array that will be returned. The next call fills out the array.
        uint32_t environmentBlendModeCount = 0;
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, 0, &environmentBlendModeCount, nullptr), "Failed to enumerate EnvironmentBlend Modes.");
        m_environmentBlendModes.resize(environmentBlendModeCount);
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, environmentBlendModeCount, &environmentBlendModeCount, m_environmentBlendModes.data()), "Failed to enumerate EnvironmentBlend Modes.");

        // Pick the first application supported blend mode supported by the hardware.
        for (const XrEnvironmentBlendMode &environmentBlendMode : m_applicationEnvironmentBlendModes) {
            if (std::find(m_environmentBlendModes.begin(), m_environmentBlendModes.end(), environmentBlendMode) != m_environmentBlendModes.end()) {
                m_environmentBlendMode = environmentBlendMode;
                break;
            }
        }
        if (m_environmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM) {
            XR_TUT_LOG_ERROR("Failed to find a compatible blend mode. Defaulting to XR_ENVIRONMENT_BLEND_MODE_OPAQUE.");
            m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
        }
        // XR_DOCS_TAG_END_GetEnvironmentBlendModes
    }

    void GetViewConfigurationViews() {
        // XR_DOCS_TAG_BEGIN_GetViewConfigurationViews
        // Gets the View Configuration Types. The first call gets the count of the array that will be returned. The next call fills out the array.
        uint32_t viewConfigurationCount = 0;
        OPENXR_CHECK(xrEnumerateViewConfigurations(m_xrInstance, m_systemID, 0, &viewConfigurationCount, nullptr), "Failed to enumerate View Configurations.");
        m_viewConfigurations.resize(viewConfigurationCount);
        OPENXR_CHECK(xrEnumerateViewConfigurations(m_xrInstance, m_systemID, viewConfigurationCount, &viewConfigurationCount, m_viewConfigurations.data()), "Failed to enumerate View Configurations.");

        // Pick the first application supported View Configuration Type con supported by the hardware.
        for (const XrViewConfigurationType &viewConfiguration : m_applicationViewConfigurations) {
            if (std::find(m_viewConfigurations.begin(), m_viewConfigurations.end(), viewConfiguration) != m_viewConfigurations.end()) {
                m_viewConfiguration = viewConfiguration;
                break;
            }
        }
        if (m_viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM) {
            XR_TUT_LOG_ERROR("Failed to find a view configuration type. Defaulting to XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO.");
            m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        }

        // Gets the View Configuration Views. The first call gets the count of the array that will be returned. The next call fills out the array.
        uint32_t viewConfigurationViewCount = 0;
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, 0, &viewConfigurationViewCount, nullptr), "Failed to enumerate ViewConfiguration Views.");
        m_viewConfigurationViews.resize(viewConfigurationViewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, viewConfigurationViewCount, &viewConfigurationViewCount, m_viewConfigurationViews.data()), "Failed to enumerate ViewConfiguration Views.");
        // XR_DOCS_TAG_END_GetViewConfigurationViews
    }

    void CreateSession() {
        // Create an XrSessionCreateInfo structure.
        // XR_DOCS_TAG_BEGIN_CreateSession1
        XrSessionCreateInfo sessionCI{XR_TYPE_SESSION_CREATE_INFO};
        // XR_DOCS_TAG_END_CreateSession1

        // Create a std::unique_ptr<GraphicsAPI_...> from the instance and system.
        // This call sets up a graphics API that's suitable for use with OpenXR.
        if (m_apiType == D3D11) {
#if defined(XR_USE_GRAPHICS_API_D3D11)
            m_graphicsAPI = std::make_unique<GraphicsAPI_D3D11>(m_xrInstance, m_systemID);
#endif
        } else if (m_apiType == D3D12) {
#if defined(XR_USE_GRAPHICS_API_D3D12)
            m_graphicsAPI = std::make_unique<GraphicsAPI_D3D12>(m_xrInstance, m_systemID);
#endif
        } else if (m_apiType == OPENGL) {
#if defined(XR_USE_GRAPHICS_API_OPENGL)
            m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL>(m_xrInstance, m_systemID);
#endif
        } else if (m_apiType == OPENGL_ES) {
#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
            m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL_ES>(m_xrInstance, m_systemID);
#endif
        } else if (m_apiType == VULKAN) {
#if defined(XR_USE_GRAPHICS_API_VULKAN)
            m_graphicsAPI = std::make_unique<GraphicsAPI_Vulkan>(m_xrInstance, m_systemID);
#endif
        } else {
            XR_TUT_LOG_ERROR("ERROR: Unknown Graphics API.");
            DEBUG_BREAK;
        }
        // Fill out the XrSessionCreateInfo structure and create an XrSession.
        //  XR_DOCS_TAG_BEGIN_CreateSession2
        sessionCI.next = m_graphicsAPI->GetGraphicsBinding();
        sessionCI.createFlags = 0;
        sessionCI.systemId = m_systemID;

        OPENXR_CHECK(xrCreateSession(m_xrInstance, &sessionCI, &m_session), "Failed to create Session.");
        // XR_DOCS_TAG_END_CreateSession2
    }

    void DestroySession() {
        // Destroy the XrSession.
        // XR_DOCS_TAG_BEGIN_DestroySession
        OPENXR_CHECK(xrDestroySession(m_session), "Failed to destroy Session.");
        // XR_DOCS_TAG_END_DestroySession
    }

    // XR_DOCS_TAG_BEGIN_CreateResources1
    struct CameraConstants {
        XrMatrix4x4f viewProj[2];
        XrMatrix4x4f modelViewProj[2];
        XrMatrix4x4f model;
        XrVector4f color;
        XrVector4f pad1;
        XrVector4f pad2;
        XrVector4f pad3;
    };
    CameraConstants cameraConstants;
    XrVector4f normals[6] = {
        {1.00f, 0.00f, 0.00f, 0},
        {-1.00f, 0.00f, 0.00f, 0},
        {0.00f, 1.00f, 0.00f, 0},
        {0.00f, -1.00f, 0.00f, 0},
        {0.00f, 0.00f, 1.00f, 0},
        {0.00f, 0.0f, -1.00f, 0}};
    // XR_DOCS_TAG_END_CreateResources1

    void CreateResources() {
        // XR_DOCS_TAG_BEGIN_CreateResources1_1
        // Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
        constexpr XrVector4f vertexPositions[] = {
            {+0.5f, +0.5f, +0.5f, 1.0f},
            {+0.5f, +0.5f, -0.5f, 1.0f},
            {+0.5f, -0.5f, +0.5f, 1.0f},
            {+0.5f, -0.5f, -0.5f, 1.0f},
            {-0.5f, +0.5f, +0.5f, 1.0f},
            {-0.5f, +0.5f, -0.5f, 1.0f},
            {-0.5f, -0.5f, +0.5f, 1.0f},
            {-0.5f, -0.5f, -0.5f, 1.0f}};

#define CUBE_FACE(V1, V2, V3, V4, V5, V6) vertexPositions[V1], vertexPositions[V2], vertexPositions[V3], vertexPositions[V4], vertexPositions[V5], vertexPositions[V6],

        XrVector4f cubeVertices[] = {
            CUBE_FACE(2, 1, 0, 2, 3, 1)  // -X
            CUBE_FACE(6, 4, 5, 6, 5, 7)  // +X
            CUBE_FACE(0, 1, 5, 0, 5, 4)  // -Y
            CUBE_FACE(2, 6, 7, 2, 7, 3)  // +Y
            CUBE_FACE(0, 4, 6, 0, 6, 2)  // -Z
            CUBE_FACE(1, 3, 7, 1, 7, 5)  // +Z
        };

        uint32_t cubeIndices[36] = {
            0, 1, 2, 3, 4, 5,        // -X
            6, 7, 8, 9, 10, 11,      // +X
            12, 13, 14, 15, 16, 17,  // -Y
            18, 19, 20, 21, 22, 23,  // +Y
            24, 25, 26, 27, 28, 29,  // -Z
            30, 31, 32, 33, 34, 35,  // +Z
        };

        m_vertexBuffer = m_graphicsAPI->CreateBuffer({GraphicsAPI::BufferCreateInfo::Type::VERTEX, sizeof(float) * 4, sizeof(cubeVertices), &cubeVertices});

        m_indexBuffer = m_graphicsAPI->CreateBuffer({GraphicsAPI::BufferCreateInfo::Type::INDEX, sizeof(uint32_t), sizeof(cubeIndices), &cubeIndices});

        // XR_DOCS_TAG_BEGIN_Update_numberOfCuboids
        size_t numberOfCuboids = 64 + 2 + 2;
        // XR_DOCS_TAG_END_Update_numberOfCuboids
        m_uniformBuffer_Camera = m_graphicsAPI->CreateBuffer({GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, m_graphicsAPI->AlignSizeForUniformBuffer(sizeof(CameraConstants)) * numberOfCuboids, nullptr});
        m_uniformBuffer_Normals = m_graphicsAPI->CreateBuffer({GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(normals), &normals});
        // XR_DOCS_TAG_END_CreateResources1_1

        // XR_DOCS_TAG_BEGIN_CreateResources2_OpenGL
        if (m_apiType == OPENGL) {
            std::string vertexSource = ReadTextFile("VertexShader_GL_MV.glsl");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::string fragmentSource = ReadTextFile("PixelShader_GL_MV.glsl");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        // XR_DOCS_TAG_END_CreateResources2_OpenGL
        // XR_DOCS_TAG_BEGIN_CreateResources2_VulkanWindowsLinux
        if (m_apiType == VULKAN) {
            std::vector<char> vertexSource = ReadBinaryFile("VertexShader_VK_MV.spv");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::vector<char> fragmentSource = ReadBinaryFile("PixelShader_VK_MV.spv");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        // XR_DOCS_TAG_END_CreateResources2_VulkanWindowsLinux
#if defined(__ANDROID__)
        // XR_DOCS_TAG_BEGIN_CreateResources2_VulkanAndroid
        if (m_apiType == VULKAN) {
            std::vector<char> vertexSource = ReadBinaryFile("shaders/VertexShader_VK_MV.spv", androidApp->activity->assetManager);
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});
            std::vector<char> fragmentSource = ReadBinaryFile("shaders/PixelShader_VK_MV.spv", androidApp->activity->assetManager);
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        // XR_DOCS_TAG_END_CreateResources2_VulkanAndroid
        // XR_DOCS_TAG_BEGIN_CreateResources2_OpenGLES
        if (m_apiType == OPENGL_ES) {
            std::string vertexSource = ReadTextFile("shaders/VertexShader_GLES_MV.glsl", androidApp->activity->assetManager);
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});
            std::string fragmentSource = ReadTextFile("shaders/PixelShader_GLES_MV.glsl", androidApp->activity->assetManager);
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        // XR_DOCS_TAG_END_CreateResources2_OpenGLES
#endif
        // XR_DOCS_TAG_BEGIN_CreateResources2_D3D11
        if (m_apiType == D3D11) {
            std::vector<char> vertexSource = ReadBinaryFile("VertexShader_DX11_MV_5_0.cso");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::vector<char> fragmentSource = ReadBinaryFile("PixelShader_DX11_MV_5_0.cso");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        // XR_DOCS_TAG_END_CreateResources2_D3D11
        // XR_DOCS_TAG_BEGIN_CreateResources2_D3D12
        if (m_apiType == D3D12) {
            std::vector<char> vertexSource = ReadBinaryFile("VertexShader_DX12_MV_6_1.cso");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::vector<char> fragmentSource = ReadBinaryFile("PixelShader_DX12_MV_6_1.cso");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        // XR_DOCS_TAG_END_CreateResources2_D3D12

        // XR_DOCS_TAG_BEGIN_CreateResources3
        GraphicsAPI::PipelineCreateInfo pipelineCI;
        pipelineCI.shaders = {m_vertexShader, m_fragmentShader};
        pipelineCI.vertexInputState.attributes = {{0, 0, GraphicsAPI::VertexType::VEC4, 0, "TEXCOORD"}};
        pipelineCI.vertexInputState.bindings = {{0, 0, 4 * sizeof(float)}};
        pipelineCI.inputAssemblyState = {GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, false};
        pipelineCI.rasterisationState = {false, false, GraphicsAPI::PolygonMode::FILL, GraphicsAPI::CullMode::BACK, GraphicsAPI::FrontFace::COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f};
        pipelineCI.multisampleState = {1, false, 1.0f, 0xFFFFFFFF, false, false};
        pipelineCI.depthStencilState = {true, true, GraphicsAPI::CompareOp::LESS_OR_EQUAL, false, false, {}, {}, 0.0f, 1.0f};
        pipelineCI.colorBlendState = {false, GraphicsAPI::LogicOp::NO_OP, {{true, GraphicsAPI::BlendFactor::SRC_ALPHA, GraphicsAPI::BlendFactor::ONE_MINUS_SRC_ALPHA, GraphicsAPI::BlendOp::ADD, GraphicsAPI::BlendFactor::ONE, GraphicsAPI::BlendFactor::ZERO, GraphicsAPI::BlendOp::ADD, (GraphicsAPI::ColorComponentBit)15}}, {0.0f, 0.0f, 0.0f, 0.0f}};
        pipelineCI.colorFormats = {m_colorSwapchainInfo.swapchainFormat};
        pipelineCI.depthFormat = m_depthSwapchainInfo.swapchainFormat;
        pipelineCI.layout = {{0, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX},
                             {1, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX},
                             {2, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT}};
        pipelineCI.viewMask = 0b11;
        m_pipeline = m_graphicsAPI->CreatePipeline(pipelineCI);
        // XR_DOCS_TAG_END_CreateResources3

        // XR_DOCS_TAG_BEGIN_Setup_Blocks
        // Create sixty-four cubic blocks, 20cm wide, evenly distributed,
        // and randomly colored.
        float scale = 0.2f;
        // Center the blocks a little way from the origin.
        XrVector3f center = {0.0f, -0.2f, -0.7f};
        for (int i = 0; i < 4; i++) {
            float x = scale * (float(i) - 1.5f) + center.x;
            for (int j = 0; j < 4; j++) {
                float y = scale * (float(j) - 1.5f) + center.y;
                for (int k = 0; k < 4; k++) {
                    float angleRad = 0;
                    float z = scale * (float(k) - 1.5f) + center.z;
                    // No rotation
                    XrQuaternionf q = {0.0f, 0.0f, 0.0f, 1.0f};
                    // A random color.
                    XrVector3f color = {pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator)};
                    m_blocks.push_back({{q, {x, y, z}}, {0.095f, 0.095f, 0.095f}, color});
                }
            }
        }
        // XR_DOCS_TAG_END_Setup_Blocks
    }
    void DestroyResources() {
        // XR_DOCS_TAG_BEGIN_DestroyResources
        m_graphicsAPI->DestroyPipeline(m_pipeline);
        m_graphicsAPI->DestroyShader(m_fragmentShader);
        m_graphicsAPI->DestroyShader(m_vertexShader);
        m_graphicsAPI->DestroyBuffer(m_uniformBuffer_Camera);
        m_graphicsAPI->DestroyBuffer(m_uniformBuffer_Normals);
        m_graphicsAPI->DestroyBuffer(m_indexBuffer);
        m_graphicsAPI->DestroyBuffer(m_vertexBuffer);
        // XR_DOCS_TAG_END_DestroyResources
    }

    void PollEvents() {
        // XR_DOCS_TAG_BEGIN_PollEvents
        // Poll OpenXR for a new event.
        XrEventDataBuffer eventData{XR_TYPE_EVENT_DATA_BUFFER};
        auto XrPollEvents = [&]() -> bool {
            eventData = {XR_TYPE_EVENT_DATA_BUFFER};
            return xrPollEvent(m_xrInstance, &eventData) == XR_SUCCESS;
        };

        while (XrPollEvents()) {
            switch (eventData.type) {
            // Log the number of lost events from the runtime.
            case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
                XrEventDataEventsLost *eventsLost = reinterpret_cast<XrEventDataEventsLost *>(&eventData);
                XR_TUT_LOG("OPENXR: Events Lost: " << eventsLost->lostEventCount);
                break;
            }
            // Log that an instance loss is pending and shutdown the application.
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                XrEventDataInstanceLossPending *instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending *>(&eventData);
                XR_TUT_LOG("OPENXR: Instance Loss Pending at: " << instanceLossPending->lossTime);
                m_sessionRunning = false;
                m_applicationRunning = false;
                break;
            }
            // Log that the interaction profile has changed.
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
                XrEventDataInteractionProfileChanged *interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged *>(&eventData);
                XR_TUT_LOG("OPENXR: Interaction Profile changed for Session: " << interactionProfileChanged->session);
                if (interactionProfileChanged->session != m_session) {
                    XR_TUT_LOG("XrEventDataInteractionProfileChanged for unknown Session");
                    break;
                }
                // XR_DOCS_TAG_BEGIN_CallRecordCurrentBindings
                RecordCurrentBindings();
                // XR_DOCS_TAG_END_CallRecordCurrentBindings
                break;
            }
            // Log that there's a reference space change pending.
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                XrEventDataReferenceSpaceChangePending *referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending *>(&eventData);
                XR_TUT_LOG("OPENXR: Reference Space Change pending for Session: " << referenceSpaceChangePending->session);
                if (referenceSpaceChangePending->session != m_session) {
                   XR_TUT_LOG("XrEventDataReferenceSpaceChangePending for unknown Session");
                    break;
                }
                break;
            }
            // Session State changes:
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                XrEventDataSessionStateChanged *sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged *>(&eventData);
                if (sessionStateChanged->session != m_session) {
                    XR_TUT_LOG("XrEventDataSessionStateChanged for unknown Session");
                    break;
                }

                if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
                    // SessionState is ready. Begin the XrSession using the XrViewConfigurationType.
                    XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
                    sessionBeginInfo.primaryViewConfigurationType = m_viewConfiguration;
                    OPENXR_CHECK(xrBeginSession(m_session, &sessionBeginInfo), "Failed to begin Session.");
                    m_sessionRunning = true;
                }
                if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
                    // SessionState is stopping. End the XrSession.
                    OPENXR_CHECK(xrEndSession(m_session), "Failed to end Session.");
                    m_sessionRunning = false;
                }
                if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
                    // SessionState is exiting. Exit the application.
                    m_sessionRunning = false;
                    m_applicationRunning = false;
                }
                if (sessionStateChanged->state == XR_SESSION_STATE_LOSS_PENDING) {
                    // SessionState is loss pending. Exit the application.
                    // It's possible to try a reestablish an XrInstance and XrSession, but we will simply exit here.
                    m_sessionRunning = false;
                    m_applicationRunning = false;
                }
                // Store state for reference across the application.
                m_sessionState = sessionStateChanged->state;
                break;
            }
            default: {
                break;
            }
            }
        }
        // XR_DOCS_TAG_END_PollEvents
    }
    // XR_DOCS_TAG_BEGIN_PollActions
    void PollActions(XrTime predictedTime) {
        // Update our action set with up-to-date input data.
        // First, we specify the actionSet we are polling.
        XrActiveActionSet activeActionSet{};
        activeActionSet.actionSet = m_actionSet;
        activeActionSet.subactionPath = XR_NULL_PATH;
        // Now we sync the Actions to make sure they have current data.
        XrActionsSyncInfo actionsSyncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
        actionsSyncInfo.countActiveActionSets = 1;
        actionsSyncInfo.activeActionSets = &activeActionSet;
        OPENXR_CHECK(xrSyncActions(m_session, &actionsSyncInfo), "Failed to sync Actions.");
        // XR_DOCS_TAG_END_PollActions
        // XR_DOCS_TAG_BEGIN_PollActions2
        XrActionStateGetInfo actionStateGetInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        // We pose a single Action, twice - once for each subAction Path.
        actionStateGetInfo.action = m_palmPoseAction;
        // For each hand, get the pose state if possible.
        for (int i = 0; i < 2; i++) {
            // Specify the subAction Path.
            actionStateGetInfo.subactionPath = m_handPaths[i];
            OPENXR_CHECK(xrGetActionStatePose(m_session, &actionStateGetInfo, &m_handPoseState[i]), "Failed to get Pose State.");
            if (m_handPoseState[i].isActive) {
                XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
                XrResult res = xrLocateSpace(m_handPoseSpace[i], m_localSpace, predictedTime, &spaceLocation);
                if (XR_UNQUALIFIED_SUCCESS(res) &&
                    (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
                    (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                    m_handPose[i] = spaceLocation.pose;
                } else {
                    m_handPoseState[i].isActive = false;
                }
            }
        }
        // XR_DOCS_TAG_END_PollActions2
        // XR_DOCS_TAG_BEGIN_PollActions3
        for (int i = 0; i < 2; i++) {
            actionStateGetInfo.action = m_grabCubeAction;
            actionStateGetInfo.subactionPath = m_handPaths[i];
            OPENXR_CHECK(xrGetActionStateFloat(m_session, &actionStateGetInfo, &m_grabState[i]), "Failed to get Float State of Grab Cube action.");
        }
        for (int i = 0; i < 2; i++) {
            actionStateGetInfo.action = m_changeColorAction;
            actionStateGetInfo.subactionPath = m_handPaths[i];
            OPENXR_CHECK(xrGetActionStateBoolean(m_session, &actionStateGetInfo, &m_changeColorState[i]), "Failed to get Boolean State of Change Color action.");
        }
        // The Spawn Cube action has no subActionPath:
        {
            actionStateGetInfo.action = m_spawnCubeAction;
            actionStateGetInfo.subactionPath = 0;
            OPENXR_CHECK(xrGetActionStateBoolean(m_session, &actionStateGetInfo, &m_spawnCubeState), "Failed to get Boolean State of Spawn Cube action.");
        }
        // XR_DOCS_TAG_END_PollActions3
        // XR_DOCS_TAG_BEGIN_PollActions4
        for (int i = 0; i < 2; i++) {
            m_buzz[i] *= 0.5f;
            if (m_buzz[i] < 0.01f)
                m_buzz[i] = 0.0f;
            XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
            vibration.amplitude = m_buzz[i];
            vibration.duration = XR_MIN_HAPTIC_DURATION;
            vibration.frequency = XR_FREQUENCY_UNSPECIFIED;

            XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
            hapticActionInfo.action = m_buzzAction;
            hapticActionInfo.subactionPath = m_handPaths[i];
            OPENXR_CHECK(xrApplyHapticFeedback(m_session, &hapticActionInfo, (XrHapticBaseHeader *)&vibration), "Failed to apply haptic feedback.");
        }
    }
    // XR_DOCS_TAG_END_PollActions4
    // XR_DOCS_TAG_BEGIN_BlockInteraction
    // Helper function to snap a 3D position to the nearest 10cm
    static XrVector3f FixPosition(XrVector3f pos) {
        int x = int(std::nearbyint(pos.x * 10.f));
        int y = int(std::nearbyint(pos.y * 10.f));
        int z = int(std::nearbyint(pos.z * 10.f));
        pos.x = float(x) / 10.f;
        pos.y = float(y) / 10.f;
        pos.z = float(z) / 10.f;
        return pos;
    }
    // Handle the interaction between the user's hands, the grab action, and the 3D blocks.
    void BlockInteraction() {
        // For each hand:
        for (int i = 0; i < 2; i++) {
            float nearest = 1.0f;
            // If not currently holding a block:
            if (m_grabbedBlock[i] == -1) {
                m_nearBlock[i] = -1;
                // Only if the pose was detected this frame:
                if (m_handPoseState[i].isActive) {
                    // For each block:
                    for (int j = 0; j < m_blocks.size(); j++) {
                        auto block = m_blocks[j];
                        // How far is it from the hand to this block?
                        XrVector3f diff = block.pose.position - m_handPose[i].position;
                        float distance = std::max(fabs(diff.x), std::max(fabs(diff.y), fabs(diff.z)));
                        if (distance < 0.05f && distance < nearest) {
                            m_nearBlock[i] = j;
                            nearest = distance;
                        }
                    }
                }
                if (m_nearBlock[i] != -1) {
                    if (m_grabState[i].isActive && m_grabState[i].currentState > 0.5f) {
                        m_grabbedBlock[i] = m_nearBlock[i];
                        m_buzz[i] = 1.0f;
                    } else if (m_changeColorState[i].isActive == XR_TRUE && m_changeColorState[i].currentState == XR_FALSE && m_changeColorState[i].changedSinceLastSync == XR_TRUE) {
                        auto &thisBlock = m_blocks[m_nearBlock[i]];
                        XrVector3f color = {pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator)};
                        thisBlock.color = color;
                    }
                } else {
                    // not near a block? We can spawn one.
                    if (m_spawnCubeState.isActive == XR_TRUE && m_spawnCubeState.currentState == XR_FALSE && m_spawnCubeState.changedSinceLastSync == XR_TRUE && m_blocks.size() < m_maxBlockCount) {
                        XrQuaternionf q = {0.0f, 0.0f, 0.0f, 1.0f};
                        XrVector3f color = {pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator)};
                        m_blocks.push_back({{q, FixPosition(m_handPose[i].position)}, {0.095f, 0.095f, 0.095f}, color});
                    }
                }
            } else {
                m_nearBlock[i] = m_grabbedBlock[i];
                if (m_handPoseState[i].isActive)
                    m_blocks[m_grabbedBlock[i]].pose.position = m_handPose[i].position;
                if (!m_grabState[i].isActive || m_grabState[i].currentState < 0.5f) {
                    m_blocks[m_grabbedBlock[i]].pose.position = FixPosition(m_blocks[m_grabbedBlock[i]].pose.position);
                    m_grabbedBlock[i] = -1;
                    m_buzz[i] = 0.2f;
                }
            }
        }
    }
    // XR_DOCS_TAG_END_BlockInteraction

    void CreateReferenceSpace() {
        // XR_DOCS_TAG_BEGIN_CreateReferenceSpace
        // Fill out an XrReferenceSpaceCreateInfo structure and create a reference XrSpace, specifying a Local space with an identity pose as the origin.
        XrReferenceSpaceCreateInfo referenceSpaceCI{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        referenceSpaceCI.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
        OPENXR_CHECK(xrCreateReferenceSpace(m_session, &referenceSpaceCI, &m_localSpace), "Failed to create ReferenceSpace.");
        // XR_DOCS_TAG_END_CreateReferenceSpace
    }

    void DestroyReferenceSpace() {
        // XR_DOCS_TAG_BEGIN_DestroyReferenceSpace
        // Destroy the reference XrSpace.
        OPENXR_CHECK(xrDestroySpace(m_localSpace), "Failed to destroy Space.")
        // XR_DOCS_TAG_END_DestroyReferenceSpace
    }

    void CreateSwapchains() {
        // XR_DOCS_TAG_BEGIN_EnumerateSwapchainFormats
        // Get the supported swapchain formats as an array of int64_t and ordered by runtime preference.
        uint32_t formatCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, 0, &formatCount, nullptr), "Failed to enumerate Swapchain Formats");
        std::vector<int64_t> formats(formatCount);
        OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, formatCount, &formatCount, formats.data()), "Failed to enumerate Swapchain Formats");
        if (m_graphicsAPI->SelectDepthSwapchainFormat(formats) == 0) {
            XR_TUT_LOG_ERROR("Failed to find depth format for Swapchain.");
            DEBUG_BREAK;
        }
        // XR_DOCS_TAG_END_EnumerateSwapchainFormats

        // XR_DOCS_TAG_BEGIN_CreateViewConfigurationView
        bool coherentViews = m_viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        for (const XrViewConfigurationView &viewConfigurationView : m_viewConfigurationViews) {
            // Check the current view size against the first view.
            coherentViews |= m_viewConfigurationViews[0].recommendedImageRectWidth == viewConfigurationView.recommendedImageRectWidth;
            coherentViews |= m_viewConfigurationViews[0].recommendedImageRectHeight == viewConfigurationView.recommendedImageRectHeight;
        }
        if (!coherentViews) {
            XR_TUT_LOG_ERROR("The views are not coherent. Unable to create a single Swapchain.");
            DEBUG_BREAK;
        }
        const XrViewConfigurationView &viewConfigurationView = m_viewConfigurationViews[0];
        uint32_t viewCount = static_cast<uint32_t>(m_viewConfigurationViews.size());
        // XR_DOCS_TAG_END_CreateViewConfigurationView

        // XR_DOCS_TAG_BEGIN_CreateSwapchains
        // Create a color and depth swapchain, and their associated image views.
        // Fill out an XrSwapchainCreateInfo structure and create an XrSwapchain.
        // Color.
        XrSwapchainCreateInfo swapchainCI{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchainCI.createFlags = 0;
        swapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCI.format = m_graphicsAPI->SelectColorSwapchainFormat(formats);          // Use GraphicsAPI to select the first compatible format.
        swapchainCI.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount;  // Use the recommended values from the XrViewConfigurationView.
        swapchainCI.width = viewConfigurationView.recommendedImageRectWidth;
        swapchainCI.height = viewConfigurationView.recommendedImageRectHeight;
        swapchainCI.faceCount = 1;
        swapchainCI.arraySize = viewCount;
        swapchainCI.mipCount = 1;
        OPENXR_CHECK(xrCreateSwapchain(m_session, &swapchainCI, &m_colorSwapchainInfo.swapchain), "Failed to create Color Swapchain");
        m_colorSwapchainInfo.swapchainFormat = swapchainCI.format;  // Save the swapchain format for later use.

        // Depth.
        swapchainCI.createFlags = 0;
        swapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        swapchainCI.format = m_graphicsAPI->SelectDepthSwapchainFormat(formats);          // Use GraphicsAPI to select the first compatible format.
        swapchainCI.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount;  // Use the recommended values from the XrViewConfigurationView.
        swapchainCI.width = viewConfigurationView.recommendedImageRectWidth;
        swapchainCI.height = viewConfigurationView.recommendedImageRectHeight;
        swapchainCI.faceCount = 1;
        swapchainCI.arraySize = viewCount;
        swapchainCI.mipCount = 1;
        OPENXR_CHECK(xrCreateSwapchain(m_session, &swapchainCI, &m_depthSwapchainInfo.swapchain), "Failed to create Depth Swapchain");
        m_depthSwapchainInfo.swapchainFormat = swapchainCI.format;  // Save the swapchain format for later use.
        // XR_DOCS_TAG_END_CreateSwapchains
        
        // XR_DOCS_TAG_BEGIN_EnumerateSwapchainImages
        // Get the number of images in the color/depth swapchain and allocate Swapchain image data via GraphicsAPI to store the returned array.
        uint32_t colorSwapchainImageCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, 0, &colorSwapchainImageCount, nullptr), "Failed to enumerate Color Swapchain Images.");
        XrSwapchainImageBaseHeader *colorSwapchainImages = m_graphicsAPI->AllocateSwapchainImageData(m_colorSwapchainInfo.swapchain, GraphicsAPI::SwapchainType::COLOR, colorSwapchainImageCount);
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, colorSwapchainImageCount, &colorSwapchainImageCount, colorSwapchainImages), "Failed to enumerate Color Swapchain Images.");
        
        uint32_t depthSwapchainImageCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, 0, &depthSwapchainImageCount, nullptr), "Failed to enumerate Depth Swapchain Images.");
        XrSwapchainImageBaseHeader *depthSwapchainImages = m_graphicsAPI->AllocateSwapchainImageData(m_depthSwapchainInfo.swapchain, GraphicsAPI::SwapchainType::DEPTH, depthSwapchainImageCount);
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, depthSwapchainImageCount, &depthSwapchainImageCount, depthSwapchainImages), "Failed to enumerate Depth Swapchain Images.");
        // XR_DOCS_TAG_END_EnumerateSwapchainImages

        // XR_DOCS_TAG_BEGIN_CreateImageViews
        // Per image in the swapchains, fill out a GraphicsAPI::ImageViewCreateInfo structure and create a color/depth image view.
        for (uint32_t j = 0; j < colorSwapchainImageCount; j++) {
            GraphicsAPI::ImageViewCreateInfo imageViewCI;
            imageViewCI.image = m_graphicsAPI->GetSwapchainImage(m_colorSwapchainInfo.swapchain, j);
            imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::RTV;
            imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D_ARRAY;
            imageViewCI.format = m_colorSwapchainInfo.swapchainFormat;
            imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::COLOR_BIT;
            imageViewCI.baseMipLevel = 0;
            imageViewCI.levelCount = 1;
            imageViewCI.baseArrayLayer = 0;
            imageViewCI.layerCount = viewCount;
            m_colorSwapchainInfo.imageViews.push_back(m_graphicsAPI->CreateImageView(imageViewCI));
        }
        for (uint32_t j = 0; j < depthSwapchainImageCount; j++) {
            GraphicsAPI::ImageViewCreateInfo imageViewCI;
            imageViewCI.image = m_graphicsAPI->GetSwapchainImage(m_depthSwapchainInfo.swapchain, j);
            imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::DSV;
            imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D_ARRAY;
            imageViewCI.format = m_depthSwapchainInfo.swapchainFormat;
            imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::DEPTH_BIT;
            imageViewCI.baseMipLevel = 0;
            imageViewCI.levelCount = 1;
            imageViewCI.baseArrayLayer = 0;
            imageViewCI.layerCount = viewCount;
            m_depthSwapchainInfo.imageViews.push_back(m_graphicsAPI->CreateImageView(imageViewCI));
        }
        // XR_DOCS_TAG_END_CreateImageViews
    }

    void DestroySwapchains() {
        // XR_DOCS_TAG_BEGIN_DestroySwapchains
        // Destroy the color and depth image views from GraphicsAPI.
        for (void*& imageView : m_colorSwapchainInfo.imageViews) {
            m_graphicsAPI->DestroyImageView(imageView);
        }
        for (void*& imageView : m_depthSwapchainInfo.imageViews) {
            m_graphicsAPI->DestroyImageView(imageView);
        }

        // Free the Swapchain Image Data.
        m_graphicsAPI->FreeSwapchainImageData(m_colorSwapchainInfo.swapchain);
        m_graphicsAPI->FreeSwapchainImageData(m_depthSwapchainInfo.swapchain);

        // Destroy the swapchains.
        OPENXR_CHECK(xrDestroySwapchain(m_colorSwapchainInfo.swapchain), "Failed to destroy Color Swapchain");
        OPENXR_CHECK(xrDestroySwapchain(m_depthSwapchainInfo.swapchain), "Failed to destroy Depth Swapchain");
        // XR_DOCS_TAG_END_DestroySwapchains
    }

    // XR_DOCS_TAG_BEGIN_RenderCuboid1
    size_t renderCuboidIndex = 0;
    // XR_DOCS_TAG_END_RenderCuboid1
    void RenderCuboid(XrPosef pose, XrVector3f scale, XrVector3f color) {
        // XR_DOCS_TAG_BEGIN_RenderCuboid2
        XrMatrix4x4f_CreateTranslationRotationScale(&cameraConstants.model, &pose.position, &pose.orientation, &scale);

        const uint32_t viewCount = 2; // This will only work for stereo rendering.
        for (uint32_t i = 0; i < viewCount; i++) {
            XrMatrix4x4f_Multiply(&cameraConstants.modelViewProj[i], &cameraConstants.viewProj[i], &cameraConstants.model);
        }
        cameraConstants.color = {color.x, color.y, color.z, 1.0};
        size_t offsetCameraUB = m_graphicsAPI->AlignSizeForUniformBuffer(sizeof(CameraConstants)) * renderCuboidIndex;

        m_graphicsAPI->SetPipeline(m_pipeline);

        m_graphicsAPI->SetBufferData(m_uniformBuffer_Camera, offsetCameraUB, sizeof(CameraConstants), &cameraConstants);
        m_graphicsAPI->SetDescriptor({0, m_uniformBuffer_Camera, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX, false, offsetCameraUB, sizeof(CameraConstants)});
        m_graphicsAPI->SetDescriptor({1, m_uniformBuffer_Normals, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX, false, 0, sizeof(normals)});

        m_graphicsAPI->UpdateDescriptors();

        m_graphicsAPI->SetVertexBuffers(&m_vertexBuffer, 1);
        m_graphicsAPI->SetIndexBuffer(m_indexBuffer);
        m_graphicsAPI->DrawIndexed(36, m_apiType == D3D11 ? 2 : 1); // For D3D11, use instanced rendering for multiview.

        renderCuboidIndex++;
        // XR_DOCS_TAG_END_RenderCuboid2
    }

    void RenderFrame() {
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
        // XR_DOCS_TAG_BEGIN_RenderFrame
        // Get the XrFrameState for timing and rendering info.
        XrFrameState frameState{XR_TYPE_FRAME_STATE};
        XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
        OPENXR_CHECK(xrWaitFrame(m_session, &frameWaitInfo, &frameState), "Failed to wait for XR Frame.");

        // Tell the OpenXR compositor that the application is beginning the frame.
        XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
        OPENXR_CHECK(xrBeginFrame(m_session, &frameBeginInfo), "Failed to begin the XR Frame.");

        // Variables for rendering and layer composition.
        bool rendered = false;
        RenderLayerInfo renderLayerInfo;
        renderLayerInfo.predictedDisplayTime = frameState.predictedDisplayTime;

        // Check that the session is active and that we should render.
        bool sessionActive = (m_sessionState == XR_SESSION_STATE_SYNCHRONIZED || m_sessionState == XR_SESSION_STATE_VISIBLE || m_sessionState == XR_SESSION_STATE_FOCUSED);
        if (sessionActive && frameState.shouldRender) {
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_4_2
            // XR_DOCS_TAG_BEGIN_CallPollActions
            // poll actions here because they require a predicted display time, which we've only just obtained.
            PollActions(frameState.predictedDisplayTime);
            // Handle the interaction between the user and the 3D blocks.
            BlockInteraction();
            // XR_DOCS_TAG_END_CallPollActions
#endif
            // Render the stereo image and associate one of swapchain images with the XrCompositionLayerProjection structure.
            rendered = RenderLayer(renderLayerInfo);
            if (rendered) {
                renderLayerInfo.layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader *>(&renderLayerInfo.layerProjection));
            }
        }

        // Tell OpenXR that we are finished with this frame; specifying its display time, environment blending and layers.
        XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
        frameEndInfo.displayTime = frameState.predictedDisplayTime;
        frameEndInfo.environmentBlendMode = m_environmentBlendMode;
        frameEndInfo.layerCount = static_cast<uint32_t>(renderLayerInfo.layers.size());
        frameEndInfo.layers = renderLayerInfo.layers.data();
        OPENXR_CHECK(xrEndFrame(m_session, &frameEndInfo), "Failed to end the XR Frame.");
        // XR_DOCS_TAG_END_RenderFrame
#endif
    }

    bool RenderLayer(RenderLayerInfo &renderLayerInfo) {
        // XR_DOCS_TAG_BEGIN_RenderLayer1
        // Locate the views from the view configuration within the (reference) space at the display time.
        std::vector<XrView> views(m_viewConfigurationViews.size(), {XR_TYPE_VIEW});

        XrViewState viewState{XR_TYPE_VIEW_STATE};  // Will contain information on whether the position and/or orientation is valid and/or tracked.
        XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
        viewLocateInfo.viewConfigurationType = m_viewConfiguration;
        viewLocateInfo.displayTime = renderLayerInfo.predictedDisplayTime;
        viewLocateInfo.space = m_localSpace;
        uint32_t viewCount = 0;
        XrResult result = xrLocateViews(m_session, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());
        if (result != XR_SUCCESS) {
            XR_TUT_LOG("Failed to locate Views.");
            return false;
        }

        // Resize the layer projection views to match the view count. The layer projection views are used in the layer projection.
        renderLayerInfo.layerProjectionViews.resize(viewCount, {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW});

        // Acquire and wait for an image from the swapchains.
        // Get the image index of an image in the swapchains.
        // The timeout is infinite.
        uint32_t colorImageIndex = 0;
        uint32_t depthImageIndex = 0;
        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        OPENXR_CHECK(xrAcquireSwapchainImage(m_colorSwapchainInfo.swapchain, &acquireInfo, &colorImageIndex), "Failed to acquire Image from the Color Swapchian");
        OPENXR_CHECK(xrAcquireSwapchainImage(m_depthSwapchainInfo.swapchain, &acquireInfo, &depthImageIndex), "Failed to acquire Image from the Depth Swapchian");

        XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        OPENXR_CHECK(xrWaitSwapchainImage(m_colorSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Color Swapchain");
        OPENXR_CHECK(xrWaitSwapchainImage(m_depthSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Depth Swapchain");

        // Get the width and height and construct the viewport and scissors.
        const uint32_t &width = m_viewConfigurationViews[0].recommendedImageRectWidth;
        const uint32_t &height = m_viewConfigurationViews[0].recommendedImageRectHeight;
        GraphicsAPI::Viewport viewport = {0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
        GraphicsAPI::Rect2D scissor = {{(int32_t)0, (int32_t)0}, {width, height}};
        float nearZ = 0.05f;
        float farZ = 100.0f;

        // Fill out the XrCompositionLayerProjectionView structure specifying the pose and fov from the view.
        // This also associates the swapchain image with this layer projection view.
        // Per view in the view configuration:
        for (uint32_t i = 0; i < viewCount; i++) {
            renderLayerInfo.layerProjectionViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
            renderLayerInfo.layerProjectionViews[i].pose = views[i].pose;
            renderLayerInfo.layerProjectionViews[i].fov = views[i].fov;
            renderLayerInfo.layerProjectionViews[i].subImage.swapchain = m_colorSwapchainInfo.swapchain;
            renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.x = 0;
            renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.y = 0;
            renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.width = static_cast<int32_t>(width);
            renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.height = static_cast<int32_t>(height);
            renderLayerInfo.layerProjectionViews[i].subImage.imageArrayIndex = i;  // Useful for multiview rendering.
        }

        // Rendering code to clear the color and depth image views.
        m_graphicsAPI->BeginRendering();

        if (m_environmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_OPAQUE) {
            // VR mode use a background color.
            m_graphicsAPI->ClearColor(m_colorSwapchainInfo.imageViews[colorImageIndex], 0.17f, 0.17f, 0.17f, 1.00f);
        } else {
            // In AR mode make the background color black.
            m_graphicsAPI->ClearColor(m_colorSwapchainInfo.imageViews[colorImageIndex], 0.00f, 0.00f, 0.00f, 1.00f);
        }
        m_graphicsAPI->ClearDepth(m_depthSwapchainInfo.imageViews[depthImageIndex], 1.0f);
        // XR_DOCS_TAG_END_RenderLayer1

        // XR_DOCS_TAG_BEGIN_SetupFrameRendering
        m_graphicsAPI->SetRenderAttachments(&m_colorSwapchainInfo.imageViews[colorImageIndex], 1, m_depthSwapchainInfo.imageViews[depthImageIndex], width, height, m_pipeline);
        m_graphicsAPI->SetViewports(&viewport, 1);
        m_graphicsAPI->SetScissors(&scissor, 1);

        // Compute the view-projection transforms.
        // All matrices (including OpenXR's) are column-major, right-handed.
        for (uint32_t i = 0; i < viewCount; i++) {
            XrMatrix4x4f proj;
            XrMatrix4x4f_CreateProjectionFov(&proj, m_apiType, views[i].fov, nearZ, farZ);
            XrMatrix4x4f toView;
            XrVector3f scale1m{ 1.0f, 1.0f, 1.0f };
            XrMatrix4x4f_CreateTranslationRotationScale(&toView, &views[i].pose.position, &views[i].pose.orientation, &scale1m);
            XrMatrix4x4f view;
            XrMatrix4x4f_InvertRigidBody(&view, &toView);
            XrMatrix4x4f_Multiply(&cameraConstants.viewProj[i], &proj, &view);
        }
        // XR_DOCS_TAG_END_SetupFrameRendering

        // XR_DOCS_TAG_BEGIN_CallRenderCuboid
        renderCuboidIndex = 0;
        // Draw a floor. Scale it by 2 in the X and Z, and 0.1 in the Y,
        RenderCuboid({{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, -m_viewHeightM, 0.0f}}, {2.0f, 0.1f, 2.0f}, {0.4f, 0.5f, 0.5f});
        // Draw a "table".
        RenderCuboid({{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, -m_viewHeightM + 0.9f, -0.7f}}, {1.0f, 0.2f, 1.0f}, {0.6f, 0.6f, 0.4f});
        // XR_DOCS_TAG_END_CallRenderCuboid

        // XR_DOCS_TAG_BEGIN_CallRenderCuboid2
        // Draw some blocks at the controller positions:
        for (int i = 0; i < 2; i++) {
            if (m_handPoseState[i].isActive) {
                RenderCuboid(m_handPose[i], {0.02f, 0.04f, 0.10f}, {1.f, 1.f, 1.f});
            }
        }
        // XR_DOCS_TAG_END_CallRenderCuboid2
        for (int i = 0; i < m_blocks.size(); i++) {
            auto &thisBlock = m_blocks[i];
            XrVector3f sc = thisBlock.scale;
            if (i == m_nearBlock[0] || i == m_nearBlock[1])
                sc = thisBlock.scale * 1.05f;
            RenderCuboid(thisBlock.pose, sc, thisBlock.color);
        }
        // XR_DOCS_TAG_BEGIN_RenderLayer2
        m_graphicsAPI->EndRendering();

        // Give the swapchain image back to OpenXR, allowing the compositor to use the image.
        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        OPENXR_CHECK(xrReleaseSwapchainImage(m_colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
        OPENXR_CHECK(xrReleaseSwapchainImage(m_depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");

        // Fill out the XrCompositionLayerProjection structure for usage with xrEndFrame().
        renderLayerInfo.layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
        renderLayerInfo.layerProjection.space = m_localSpace;
        renderLayerInfo.layerProjection.viewCount = static_cast<uint32_t>(renderLayerInfo.layerProjectionViews.size());
        renderLayerInfo.layerProjection.views = renderLayerInfo.layerProjectionViews.data();

        return true;
        // XR_DOCS_TAG_END_RenderLayer2
    }

#if defined(__ANDROID__)
    // XR_DOCS_TAG_BEGIN_Android_System_Functionality1
public:
    // Stored pointer to the android_app structure from android_main().
    static android_app *androidApp;

    // Custom data structure that is used by PollSystemEvents().
    // Modified from https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/d6b6d7a10bdcf8d4fe806b4f415fde3dd5726878/src/tests/hello_xr/main.cpp#L133C1-L189C2
    struct AndroidAppState {
        ANativeWindow *nativeWindow = nullptr;
        bool resumed = false;
    };
    static AndroidAppState androidAppState;

    // Processes the next command from the Android OS. It updates AndroidAppState.
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
    // XR_DOCS_TAG_END_Android_System_Functionality1

private:
    void PollSystemEvents() {
        // XR_DOCS_TAG_BEGIN_Android_System_Functionality2
        // Checks whether Android has requested that application should by destroyed.
        if (androidApp->destroyRequested != 0) {
            m_applicationRunning = false;
            return;
        }
        while (true) {
            // Poll and process the Android OS system events.
            struct android_poll_source *source = nullptr;
            int events = 0;
            // The timeout depends on whether the application is active.
            const int timeoutMilliseconds = (!androidAppState.resumed && !m_sessionRunning && androidApp->destroyRequested == 0) ? -1 : 0;
            if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void **)&source) >= 0) {
                if (source != nullptr) {
                    source->process(androidApp, source);
                }
            } else {
                break;
            }
        }
        // XR_DOCS_TAG_END_Android_System_Functionality2
    }
#else
    void PollSystemEvents() {
        return;
    }
#endif

private:
    XrInstance m_xrInstance = XR_NULL_HANDLE;
    std::vector<const char *> m_activeAPILayers = {};
    std::vector<const char *> m_activeInstanceExtensions = {};
    std::vector<std::string> m_apiLayers = {};
    std::vector<std::string> m_instanceExtensions = {};

    XrDebugUtilsMessengerEXT m_debugUtilsMessenger = XR_NULL_HANDLE;

    XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId m_systemID = {};
    XrSystemProperties m_systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

    GraphicsAPI_Type m_apiType = UNKNOWN;
    std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;

    XrSession m_session = {};
    XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;
    bool m_applicationRunning = true;
    bool m_sessionRunning = false;

    std::vector<XrViewConfigurationType> m_applicationViewConfigurations = {XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO};
    std::vector<XrViewConfigurationType> m_viewConfigurations;
    XrViewConfigurationType m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
    std::vector<XrViewConfigurationView> m_viewConfigurationViews;

    struct SwapchainInfo {
        XrSwapchain swapchain = XR_NULL_HANDLE;
        int64_t swapchainFormat = 0;
        std::vector<void *> imageViews;
    };
    // XR_DOCS_TAG_BEGIN_SingleSwapchainInfo
    SwapchainInfo m_colorSwapchainInfo = {};
    SwapchainInfo m_depthSwapchainInfo = {};
    // XR_DOCS_TAG_END_SingleSwapchainInfo

    std::vector<XrEnvironmentBlendMode> m_applicationEnvironmentBlendModes = {XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE};
    std::vector<XrEnvironmentBlendMode> m_environmentBlendModes = {};
    XrEnvironmentBlendMode m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;

    XrSpace m_localSpace = XR_NULL_HANDLE;
    struct RenderLayerInfo {
        XrTime predictedDisplayTime = 0;
        std::vector<XrCompositionLayerBaseHeader *> layers;
        XrCompositionLayerProjection layerProjection = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
    };

    // In STAGE space, viewHeightM should be 0. In LOCAL space, it should be offset downwards, below the viewer's initial position.
    float m_viewHeightM = 1.5f;

    // Vertex and index buffers: geometry for our cuboids.
    void *m_vertexBuffer = nullptr;
    void *m_indexBuffer = nullptr;
    // Camera values constant buffer for the shaders.
    void *m_uniformBuffer_Camera = nullptr;
    // The normals are stored in a uniform buffer to simplify our vertex geometry.
    void *m_uniformBuffer_Normals = nullptr;

    // We use only two shaders in this app.
    void *m_vertexShader = nullptr, *m_fragmentShader = nullptr;

    // The pipeline is a graphics-API specific state object.
    void *m_pipeline = nullptr;

    // XR_DOCS_TAG_BEGIN_Objects
    // An instance of a 3d colored block.
    struct Block {
        XrPosef pose;
        XrVector3f scale;
        XrVector3f color;
    };
    // The list of block instances.
    std::vector<Block> m_blocks;
    // Don't let too many m_blocks get created.
    const size_t m_maxBlockCount = 100;
    // Which block, if any, is being held by each of the user's hands or controllers.
    int m_grabbedBlock[2] = {-1, -1};
    // Which block, if any, is nearby to each hand or controller.
    int m_nearBlock[2] = {-1, -1};
    // XR_DOCS_TAG_END_Objects

    // XR_DOCS_TAG_BEGIN_Actions
    XrActionSet m_actionSet;
    // An action for grabbing blocks, and an action to change the color of a block.
    XrAction m_grabCubeAction, m_spawnCubeAction, m_changeColorAction;
    // The realtime states of these actions.
    XrActionStateFloat m_grabState[2] = {{XR_TYPE_ACTION_STATE_FLOAT}, {XR_TYPE_ACTION_STATE_FLOAT}};
    XrActionStateBoolean m_changeColorState[2] = {{XR_TYPE_ACTION_STATE_BOOLEAN}, {XR_TYPE_ACTION_STATE_BOOLEAN}};
    XrActionStateBoolean m_spawnCubeState = {XR_TYPE_ACTION_STATE_BOOLEAN};
    // The haptic output action for grabbing cubes.
    XrAction m_buzzAction;
    // The current haptic output value for each controller.
    float m_buzz[2] = {0, 0};
    // The action for getting the hand or controller position and orientation.
    XrAction m_palmPoseAction;
    // The XrPaths for left and right hand hands or controllers.
    XrPath m_handPaths[2] = {0, 0};
    // The spaces that represents the two hand poses.
    XrSpace m_handPoseSpace[2];
    XrActionStatePose m_handPoseState[2] = {{XR_TYPE_ACTION_STATE_POSE}, {XR_TYPE_ACTION_STATE_POSE}};
    // The current poses obtained from the XrSpaces.
    XrPosef m_handPose[2] = {
        {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -m_viewHeightM}},
        {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -m_viewHeightM}}};
    // XR_DOCS_TAG_END_Actions
};

void OpenXRTutorial_Main(GraphicsAPI_Type apiType) {
    DebugOutput debugOutput;  // This redirects std::cerr and std::cout to the IDE's output or Android Studio's logcat.
    XR_TUT_LOG("OpenXR Tutorial Chapter 6");

    OpenXRTutorial app(apiType);
    app.Run();
}

#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
int main(int argc, char **argv) {
    OpenXRTutorial_Main(XR_TUTORIAL_GRAPHICS_API);
}
/*
// XR_DOCS_TAG_BEGIN_main_Windows_Linux_OPENGL
int main(int argc, char **argv) {
    OpenXRTutorial_Main(OPENGL);
}
// XR_DOCS_TAG_END_main_Windows_Linux_OPENGL
// XR_DOCS_TAG_BEGIN_main_Windows_Linux_VULKAN
int main(int argc, char **argv) {
    OpenXRTutorial_Main(VULKAN);
}
// XR_DOCS_TAG_END_main_Windows_Linux_VULKAN
// XR_DOCS_TAG_BEGIN_main_Windows_Linux_D3D11
int main(int argc, char **argv) {
    OpenXRTutorial_Main(D3D11);
}
// XR_DOCS_TAG_END_main_Windows_Linux_D3D11
// XR_DOCS_TAG_BEGIN_main_Windows_Linux_D3D12
int main(int argc, char **argv) {
    OpenXRTutorial_Main(D3D12);
}
// XR_DOCS_TAG_END_main_Windows_Linux_D3D12
*/
#elif (__ANDROID__)
// XR_DOCS_TAG_BEGIN_android_main___ANDROID__
android_app *OpenXRTutorial::androidApp = nullptr;
OpenXRTutorial::AndroidAppState OpenXRTutorial::androidAppState = {};

void android_main(struct android_app *app) {
    // Allow interaction with JNI and the JVM on this thread.
    // https://developer.android.com/training/articles/perf-jni#threads
    JNIEnv *env;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    // https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_KHR_loader_init
    // Load xrInitializeLoaderKHR() function pointer. On Android, the loader must be initialized with variables from android_app *.
    // Without this, there's is no loader and thus our function calls to OpenXR would fail.
    XrInstance m_xrInstance = XR_NULL_HANDLE;  // Dummy XrInstance variable for OPENXR_CHECK macro.
    PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR = nullptr;
    OPENXR_CHECK(xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction *)&xrInitializeLoaderKHR), "Failed to get InstanceProcAddr for xrInitializeLoaderKHR.");
    if (!xrInitializeLoaderKHR) {
        return;
    }

    // Fill out an XrLoaderInitInfoAndroidKHR structure and initialize the loader for Android.
    XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid{XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR};
    loaderInitializeInfoAndroid.applicationVM = app->activity->vm;
    loaderInitializeInfoAndroid.applicationContext = app->activity->clazz;
    OPENXR_CHECK(xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR *)&loaderInitializeInfoAndroid), "Failed to initialize Loader for Android.");

    // Set userData and Callback for PollSystemEvents().
    app->userData = &OpenXRTutorial::androidAppState;
    app->onAppCmd = OpenXRTutorial::AndroidAppHandleCmd;

    OpenXRTutorial::androidApp = app;
    // XR_DOCS_TAG_END_android_main___ANDROID__
    OpenXRTutorial_Main(XR_TUTORIAL_GRAPHICS_API);
}
/*
// XR_DOCS_TAG_BEGIN_android_main_OPENGL_ES
    OpenXRTutorial_Main(OPENGL_ES);
}
// XR_DOCS_TAG_END_android_main_OPENGL_ES
// XR_DOCS_TAG_BEGIN_android_main_VULKAN
    OpenXRTutorial_Main(VULKAN);
}
// XR_DOCS_TAG_END_android_main_VULKAN
*/
#endif
