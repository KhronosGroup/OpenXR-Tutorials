// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#include <DebugOutput.h>
#include <GraphicsAPI_D3D11.h>
#include <GraphicsAPI_D3D12.h>
#include <GraphicsAPI_OpenGL.h>
#include <GraphicsAPI_OpenGL_ES.h>
#include <GraphicsAPI_Vulkan.h>
#include <OpenXRDebugUtils.h>

// include xr linear algebra for XrVector and XrMatrix classes.
#include <xr_linear_algebra.h>
// Declare some useful operators for vectors:
XrVector3f operator-(XrVector3f a, XrVector3f b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
XrVector3f operator*(XrVector3f a, float b) {
    return {a.x * b, a.y * b, a.z * b};
}
// Include <algorithm> for std::min and max
#include <algorithm>
// Random numbers for colorful blocks
#include <random>
static std::uniform_real_distribution<float> pseudorandom_distribution(0, 1.f);
static std::mt19937 pseudo_random_generator;

#define XR_DOCS_CHAPTER_VERSION XR_DOCS_CHAPTER_4_5

class OpenXRTutorial {
private:
    struct RenderLayerInfo;

public:
    OpenXRTutorial(GraphicsAPI_Type apiType)
        : m_apiType(apiType) {
        // Check API compatibility with Platform.
        if (!CheckGraphicsAPI_TypeIsValidForPlatform(m_apiType)) {
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
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_4_1
        CreateActionSet();
        SuggestBindings();
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
        CreateActionPoses();
        AttachActionSet();
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
        XrApplicationInfo AI;
        strncpy(AI.applicationName, "OpenXR Tutorial Chapter 4", XR_MAX_APPLICATION_NAME_SIZE);
        AI.applicationVersion = 1;
        strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
        AI.engineVersion = 1;
        AI.apiVersion = XR_CURRENT_API_VERSION;

        // Add additional instance layers/extensions that the application wants.
        // Add both required and requested instance extensions.
        {
            m_instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
            // Ensure m_apiType is already defined when we call this line.
            m_instanceExtensions.push_back(GetGraphicsAPIInstanceExtensionString(m_apiType));
        }

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
                std::cerr << "Failed to find OpenXR instance extension: " << requestedInstanceExtension << std::endl;
            }
        }

        // Fill out an XrInstanceCreateInfo structure and create an XrInstance.
        XrInstanceCreateInfo instanceCI{XR_TYPE_INSTANCE_CREATE_INFO};
        instanceCI.createFlags = 0;
        instanceCI.applicationInfo = AI;
        instanceCI.enabledApiLayerCount = static_cast<uint32_t>(m_activeAPILayers.size());
        instanceCI.enabledApiLayerNames = m_activeAPILayers.data();
        instanceCI.enabledExtensionCount = static_cast<uint32_t>(m_activeInstanceExtensions.size());
        instanceCI.enabledExtensionNames = m_activeInstanceExtensions.data();
        OPENXR_CHECK(xrCreateInstance(&instanceCI, &m_xrInstance), "Failed to create Instance.");
    }

    void DestroyInstance() {
        // Destroy the XrInstance.
        OPENXR_CHECK(xrDestroyInstance(m_xrInstance), "Failed to destroy Instance.");
    }

    void CreateDebugMessenger() {
        // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before creating an XrDebugUtilsMessengerEXT.
        if (IsStringInVector(m_activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            m_debugUtilsMessenger = CreateOpenXRDebugUtilsMessenger(m_xrInstance);  // From OpenXRDebugUtils.h.
        }
    }
    void DestroyDebugMessenger() {
        // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before destroying the XrDebugUtilsMessengerEXT.
        if (m_debugUtilsMessenger != XR_NULL_HANDLE) {
            DestroyOpenXRDebugUtilsMessenger(m_xrInstance, m_debugUtilsMessenger);  // From OpenXRDebugUtils.h.
        }
    }

    void GetInstanceProperties() {
        // Get the instance's properties and log the runtime name and version.
        XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
        OPENXR_CHECK(xrGetInstanceProperties(m_xrInstance, &instanceProperties), "Failed to get InstanceProperties.");

        std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - ";
        std::cout << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << ".";
        std::cout << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << ".";
        std::cout << XR_VERSION_PATCH(instanceProperties.runtimeVersion) << std::endl;
    }

    void GetSystemID() {
        // Get the XrSystemId from the instance and the supplied XrFormFactor.
        XrSystemGetInfo systemGI{XR_TYPE_SYSTEM_GET_INFO};
        systemGI.formFactor = m_formFactor;
        OPENXR_CHECK(xrGetSystem(m_xrInstance, &systemGI, &m_systemID), "Failed to get SystemID.");

        // Get the System's properties for some general information about the hardware and the vendor.
        OPENXR_CHECK(xrGetSystemProperties(m_xrInstance, m_systemID, &m_systemProperties), "Failed to get SystemProperties.");
    }

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
    void CreateActionSet() {
        XrActionSetCreateInfo actionSetCI{XR_TYPE_ACTION_SET_CREATE_INFO};
        // The internal name the runtime uses for this Action Set.
        strncpy(actionSetCI.actionSetName, "openxr-tutorial-actionset", XR_MAX_ACTION_SET_NAME_SIZE);
        // Localized names are required so there is a human-readable action name to show the user if they are rebinding Actions in an options screen.
        strncpy(actionSetCI.localizedActionSetName, "OpenXR Tutorial ActionSet", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
        OPENXR_CHECK(xrCreateActionSet(m_xrInstance, &actionSetCI, &m_actionSet), "Failed to create ActionSet.");
        // Set a priority: this comes into play when we have multiple Action Sets, and determines which Action takes priority in binding to a specific input.
        actionSetCI.priority=0;

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
        // An Action for grabbing cubes.
        CreateAction(m_grabCubeAction, "grab-cube", XR_ACTION_TYPE_FLOAT_INPUT, {"/user/hand/left", "/user/hand/right"});
        CreateAction(m_spawnCubeAction, "spawn-cube", XR_ACTION_TYPE_BOOLEAN_INPUT );
        CreateAction(m_changeColorAction, "change-color", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/left", "/user/hand/right"});
        // An Action for the position of the palm of the user's hand - appropriate for the location of a grabbing Actions.
        CreateAction(m_palmPoseAction, "palm-pose", XR_ACTION_TYPE_POSE_INPUT, {"/user/hand/left", "/user/hand/right"});
        // An Action for a vibration output on one or other hand.
        CreateAction(m_buzzAction, "buzz", XR_ACTION_TYPE_VIBRATION_OUTPUT, {"/user/hand/left", "/user/hand/right"});
        // For later convenience we create the XrPaths for the subaction path names.
        m_handPaths[0] = CreateXrPath("/user/hand/left");
        m_handPaths[1] = CreateXrPath("/user/hand/right");
    }

    void SuggestBindings() {
        auto SuggestBindings = [this](const char *profile_path, std::vector<XrActionSuggestedBinding> bindings) -> bool {
            // The application can call xrSuggestInteractionProfileBindings once per interaction profile that it supports.
            XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
            interactionProfileSuggestedBinding.interactionProfile = CreateXrPath(profile_path);
            interactionProfileSuggestedBinding.suggestedBindings = bindings.data();
            interactionProfileSuggestedBinding.countSuggestedBindings = (uint32_t)bindings.size();
            if (xrSuggestInteractionProfileBindings(m_xrInstance, &interactionProfileSuggestedBinding) == XrResult::XR_SUCCESS)
                return true;
            std::cout << "Failed to suggest bindings with " << profile_path << "\n";
            return false;
        };
        bool any_ok = false;
        // Each Action here has two paths, one for each SubAction path.
        any_ok |= SuggestBindings("/interaction_profiles/khr/simple_controller", {{m_changeColorAction, CreateXrPath("/user/hand/left/input/select/click")},
                                                                                  {m_grabCubeAction, CreateXrPath("/user/hand/right/input/select/click")},
                                                                                  {m_palmPoseAction, CreateXrPath("/user/hand/left/input/grip/pose")},
                                                                                  {m_palmPoseAction, CreateXrPath("/user/hand/right/input/grip/pose")},
                                                                                  {m_buzzAction, CreateXrPath("/user/hand/left/output/haptic")},
                                                                                  {m_buzzAction, CreateXrPath("/user/hand/right/output/haptic")}});
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
            if (interactionProfile.interactionProfile)
                std::cout << " user/hand/left ActiveProfile " << FromXrPath(interactionProfile.interactionProfile).c_str() << std::endl;
            OPENXR_CHECK(xrGetCurrentInteractionProfile(m_session, m_handPaths[1], &interactionProfile), "Failed to get profile.");
            if (interactionProfile.interactionProfile)
                std::cout << "user/hand/right ActiveProfile " << FromXrPath(interactionProfile.interactionProfile).c_str() << std::endl;
        }
    }
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
    void AttachActionSet() {
        // Attach the action set we just made to the session. We could attach multiple action sets!
        XrSessionActionSetsAttachInfo actionSetAttachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
        actionSetAttachInfo.countActionSets = 1;
        actionSetAttachInfo.actionSets = &m_actionSet;
        OPENXR_CHECK(xrAttachSessionActionSets(m_session, &actionSetAttachInfo), "Failed to attach ActionSet to Session.");
    }

    void GetEnvironmentBlendModes() {
        // Retrieves the available blend modes. The first call gets the size of the array that will be returned. The next call fills out the array.
        uint32_t environmentBlendModeSize = 0;
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, 0, &environmentBlendModeSize, nullptr), "Failed to enumerate EnvironmentBlend Modes.");
        m_environmentBlendModes.resize(environmentBlendModeSize);
        OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, environmentBlendModeSize, &environmentBlendModeSize, m_environmentBlendModes.data()), "Failed to enumerate EnvironmentBlend Modes.");

        // Pick the first application supported blend mode supported by the hardware.
        for (const XrEnvironmentBlendMode &environmentBlendMode : m_applicationEnvironmentBlendModes) {
            if (std::find(m_environmentBlendModes.begin(), m_environmentBlendModes.end(), environmentBlendMode) != m_environmentBlendModes.end()) {
                m_environmentBlendMode = environmentBlendMode;
                break;
            }
        }
        if (m_environmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM) {
            std::cerr << "Failed to find a compatible blend mode. Defaulting to XR_ENVIRONMENT_BLEND_MODE_OPAQUE." << std::endl;
            m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
        }
    }

    void GetViewConfigurationViews() {
        // Gets the View Configuration Views. The first call gets the size of the array that will be returned. The next call fills out the array.
        uint32_t viewConfigurationViewSize = 0;
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, 0, &viewConfigurationViewSize, nullptr), "Failed to enumerate ViewConfiguration Views.");
        m_viewConfigurationViews.resize(viewConfigurationViewSize, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, viewConfigurationViewSize, &viewConfigurationViewSize, m_viewConfigurationViews.data()), "Failed to enumerate ViewConfiguration Views.");
    }

    void CreateSession() {
        // Create an XrSessionCreateInfo structure.
        XrSessionCreateInfo sessionCI{XR_TYPE_SESSION_CREATE_INFO};

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
            std::cout << "ERROR: Unknown Graphics API." << std::endl;
            DEBUG_BREAK;
        }
        // Fill out the XrSessionCreateInfo structure and create an XrSession.
        sessionCI.next = m_graphicsAPI->GetGraphicsBinding();
        sessionCI.createFlags = 0;
        sessionCI.systemId = m_systemID;

        OPENXR_CHECK(xrCreateSession(m_xrInstance, &sessionCI, &m_session), "Failed to create Session.");
    }

    void DestroySession() {
        // Destroy the XrSession.
        OPENXR_CHECK(xrDestroySession(m_session), "Failed to destroy Session.");
    }

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

    void CreateResources() {
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

        size_t numberOfCuboids = 64 + 2 + 2;
        m_uniformBuffer_Camera = m_graphicsAPI->CreateBuffer({GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, m_graphicsAPI->AlignSizeForUniformBuffer(sizeof(CameraConstants)) * numberOfCuboids, nullptr});
        m_uniformBuffer_Normals = m_graphicsAPI->CreateBuffer({GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(normals), &normals});

        if (m_apiType == OPENGL) {
            std::string vertexSource = ReadTextFile("VertexShader_GL_MV.glsl");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::string fragmentSource = ReadTextFile("PixelShader_GL_MV.glsl");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        if (m_apiType == VULKAN) {
            std::vector<char> vertexSource = ReadBinaryFile("VertexShader_VK_MV.spv");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::vector<char> fragmentSource = ReadBinaryFile("PixelShader_VK_MV.spv");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
#if defined(__ANDROID__)
        if (m_apiType == VULKAN) {
            std::vector<char> vertexSource = ReadBinaryFile("shaders/VertexShader_VK_MV.spv", androidApp->activity->assetManager);
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});
            std::vector<char> fragmentSource = ReadBinaryFile("shaders/PixelShader_VK_MV.spv", androidApp->activity->assetManager);
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        if (m_apiType == OPENGL_ES) {
            std::string vertexSource = ReadTextFile("shaders/VertexShader_GLES_MV.glsl", androidApp->activity->assetManager);
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});
            std::string fragmentSource = ReadTextFile("shaders/PixelShader_GLES_MV.glsl", androidApp->activity->assetManager);
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
#endif
        if (m_apiType == D3D11) {
            std::vector<char> vertexSource = ReadBinaryFile("VertexShader_DX11_MV_5_0.cso");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::vector<char> fragmentSource = ReadBinaryFile("PixelShader_DX11_MV_5_0.cso");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }
        if (m_apiType == D3D12) {
            std::vector<char> vertexSource = ReadBinaryFile("VertexShader_DX12_MV_6_1.cso");
            m_vertexShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

            std::vector<char> fragmentSource = ReadBinaryFile("PixelShader_DX12_MV_6_1.cso");
            m_fragmentShader = m_graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
        }

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
        
        float scale = 0.2f;
        XrVector3f centre = {0, -0.2f, -0.7f};
        for (int i = 0; i < 4; i++) {
            float x = scale * (float(i) - 1.5f) + centre.x;
            for (int j = 0; j < 4; j++) {
                float y = scale * (float(j) - 1.5f) + centre.y;
                for (int k = 0; k < 4; k++) {
                    float angleRad = 0;
                    float z = scale * (float(k) - 1.5f) + centre.z;
                    XrQuaternionf q= {0, 0,0,1.f};
                    XrVector3f color = {pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator)};
                    blocks.push_back({{q, {x, y, z}}, {0.095f, 0.095f, 0.095f}, color});
                }
            }
        }
    }
    void DestroyResources() {
        m_graphicsAPI->DestroyPipeline(m_pipeline);
        m_graphicsAPI->DestroyShader(m_fragmentShader);
        m_graphicsAPI->DestroyShader(m_vertexShader);
        m_graphicsAPI->DestroyBuffer(m_uniformBuffer_Camera);
        m_graphicsAPI->DestroyBuffer(m_uniformBuffer_Normals);
        m_graphicsAPI->DestroyBuffer(m_indexBuffer);
        m_graphicsAPI->DestroyBuffer(m_vertexBuffer);
    }

    void PollEvents() {
        XrResult result = XR_SUCCESS;
        do {
            // Poll OpenXR for a new event.
            XrEventDataBuffer eventData{XR_TYPE_EVENT_DATA_BUFFER};
            result = xrPollEvent(m_xrInstance, &eventData);

            switch (eventData.type) {
            // Log the number of lost events from the runtime.
            case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
                XrEventDataEventsLost *eventsLost = reinterpret_cast<XrEventDataEventsLost *>(&eventData);
                std::cout << "OPENXR: Events Lost: " << eventsLost->lostEventCount << std::endl;
                break;
            }
            // Log that an instance loss is pending and shutdown the application.
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                XrEventDataInstanceLossPending *instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending *>(&eventData);
                std::cout << "OPENXR: Instance Loss Pending at: " << instanceLossPending->lossTime << std::endl;
                m_sessionRunning = false;
                m_applicationRunning = false;
                break;
            }
            // Log that the interaction profile has changed.
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
                XrEventDataInteractionProfileChanged *interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged *>(&eventData);
                std::cout << "OPENXR: Interaction Profile changed for Session: " << interactionProfileChanged->session << std::endl;
                RecordCurrentBindings();
                break;
            }
            // Log that there's a reference space change pending.
            // TODO: expand on this in text.
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                XrEventDataReferenceSpaceChangePending *referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending *>(&eventData);
                std::cout << "OPENXR: Reference Space Change pending for Session: " << referenceSpaceChangePending->session << std::endl;
                break;
            }
            // Session State changes:
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                XrEventDataSessionStateChanged *sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged *>(&eventData);

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

        } while (result == XR_SUCCESS);
    }
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
                XrResult res = xrLocateSpace(m_handPoseSpace[i], m_localOrStageSpace, predictedTime, &spaceLocation);
                if (XR_UNQUALIFIED_SUCCESS(res) &&
                    (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
                    (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                    m_handPose[i] = spaceLocation.pose;
                }
            }
        }
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
        for (int i = 0; i < 2; i++) {
            buzz[i] *= 0.5f;
            if (buzz[i] < 0.01f)
                buzz[i] = 0.0f;
            XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
            vibration.amplitude = buzz[i];
            vibration.duration = XR_MIN_HAPTIC_DURATION;
            vibration.frequency = XR_FREQUENCY_UNSPECIFIED;

            XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
            hapticActionInfo.action = m_buzzAction;
            hapticActionInfo.subactionPath = m_handPaths[i];
            OPENXR_CHECK(xrApplyHapticFeedback(m_session, &hapticActionInfo, (XrHapticBaseHeader *)&vibration), "Failed to apply haptic feedback.");
        }
    }
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
            if (grabbedBlock[i] == -1) {
                nearBlock[i] = -1;
                // Only if the pose was detected this frame:
                if (m_handPoseState[i].isActive) {
                    // For each block:
                    for (int j = 0; j < blocks.size(); j++) {
                        auto block = blocks[j];
                        // How far is it from the hand to this block?
                        XrVector3f diff = block.pose.position - m_handPose[i].position;
                        float distance = std::max(fabs(diff.x), std::max(fabs(diff.y), fabs(diff.z)));
                        if (distance < 0.05f && distance < nearest) {
                            nearBlock[i] = j;
                            nearest = distance;
                        }
                    }
                }
                if (nearBlock[i] != -1)
                {
                    if(m_grabState[i].isActive && m_grabState[i].currentState > 0.5f) {
                        grabbedBlock[i] = nearBlock[i];
                        buzz[i] = 1.0f;
                    } 
                    else if (m_changeColorState[i].isActive == XR_TRUE && m_changeColorState[i].currentState == XR_FALSE && m_changeColorState[i].changedSinceLastSync == XR_TRUE) {
                        auto &thisBlock = blocks[nearBlock[i]];
                        XrVector3f color = {pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator)};
                        thisBlock.color = color;
                    }
                }
                else
                {
                    // not near a block? We can spawn one.
                    if (m_spawnCubeState.isActive == XR_TRUE && m_spawnCubeState.currentState == XR_FALSE && m_spawnCubeState.changedSinceLastSync == XR_TRUE && blocks.size() < MaxBlockCount) {
                        XrQuaternionf q = {0, 0, 0, 1.f};
                        XrVector3f color = {pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator), pseudorandom_distribution(pseudo_random_generator)};
                        blocks.push_back({{q, FixPosition(m_handPose[i].position)}, {0.095f, 0.095f, 0.095f}, color});
                    }
                }
            } else {
                nearBlock[i] = grabbedBlock[i];
                if (m_handPoseState[i].isActive)
                    blocks[grabbedBlock[i]].pose.position = m_handPose[i].position;
                if (!m_grabState[i].isActive || m_grabState[i].currentState < 0.5f) {
                    blocks[grabbedBlock[i]].pose.position = FixPosition(blocks[grabbedBlock[i]].pose.position);
                    grabbedBlock[i] = -1;
                    buzz[i] = 0.2f;
                }
            }
        }
    }

    void CreateReferenceSpace() {
        // Fill out an XrReferenceSpaceCreateInfo structure and create a reference XrSpace, specifying a Local space with an identity pose as the origin.
        XrReferenceSpaceCreateInfo referenceSpaceCI{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        referenceSpaceCI.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
        OPENXR_CHECK(xrCreateReferenceSpace(m_session, &referenceSpaceCI, &m_localOrStageSpace), "Failed to create ReferenceSpace.");
    }

    void DestroyReferenceSpace() {
        // Destroy the reference XrSpace.
        OPENXR_CHECK(xrDestroySpace(m_localOrStageSpace), "Failed to destroy Space.")
    }

    void CreateSwapchains() {
        // Get the supported swapchain formats as an array of int64_t and ordered by runtime preference.
        uint32_t formatSize = 0;
        OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, 0, &formatSize, nullptr), "Failed to enumerate Swapchain Formats");
        std::vector<int64_t> formats(formatSize);
        OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, formatSize, &formatSize, formats.data()), "Failed to enumerate Swapchain Formats");
        if (m_graphicsAPI->SelectDepthSwapchainFormat(formats) == 0) {
            std::cerr << "Failed to find depth format for Swapchain." << std::endl;
            std::cerr << "For systems that don't support this feature, they should use a depth image." << std::endl;
            std::cerr << "They will be no support for XR_KHR_composition_layer_depth." << std::endl;
            DEBUG_BREAK;
        }

        // TODO: Don't like this, just use a for(int loop and use the correct one in the list.
        const XrViewConfigurationView &viewConfigurationView = m_viewConfigurationViews[0];
        uint32_t viewCount = static_cast<uint32_t>(m_viewConfigurationViews.size());

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

        // Get the number of images in the color/depth swapchain and allocate Swapchain image data via GraphicsAPI to store the returned array.
        uint32_t colorSwapchainImageCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, 0, &colorSwapchainImageCount, nullptr), "Failed to enumerate Color Swapchain Images.");
        XrSwapchainImageBaseHeader *colorSwapchainImages = m_graphicsAPI->AllocateSwapchainImageData(m_colorSwapchainInfo.swapchain, GraphicsAPI::SwapchainType::COLOR, colorSwapchainImageCount);
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_colorSwapchainInfo.swapchain, colorSwapchainImageCount, &colorSwapchainImageCount, colorSwapchainImages), "Failed to enumerate Color Swapchain Images.");
        
        uint32_t depthSwapchainImageCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, 0, &depthSwapchainImageCount, nullptr), "Failed to enumerate Depth Swapchain Images.");
        XrSwapchainImageBaseHeader *depthSwapchainImages = m_graphicsAPI->AllocateSwapchainImageData(m_depthSwapchainInfo.swapchain, GraphicsAPI::SwapchainType::DEPTH, depthSwapchainImageCount);
        OPENXR_CHECK(xrEnumerateSwapchainImages(m_depthSwapchainInfo.swapchain, depthSwapchainImageCount, &depthSwapchainImageCount, depthSwapchainImages), "Failed to enumerate Depth Swapchain Images.");

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
    }

    void DestroySwapchains() {
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

        // Destory the swapchains.
        OPENXR_CHECK(xrDestroySwapchain(m_colorSwapchainInfo.swapchain), "Failed to destroy Color Swapchain");
        OPENXR_CHECK(xrDestroySwapchain(m_depthSwapchainInfo.swapchain), "Failed to destroy Depth Swapchain");
    }

    size_t renderCuboidIndex = 0;
    void RenderCuboid(XrPosef pose, XrVector3f scale, XrVector3f color) {
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
    }

    void RenderFrame() {
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
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
            // poll actions here because they require a predicted display time, which we've only just obtained.
            PollActions(frameState.predictedDisplayTime);
            // Handle the interaction between the user and the 3D blocks.
            BlockInteraction();
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
#endif
    }

    bool RenderLayer(RenderLayerInfo& renderLayerInfo) {
        // Locate the views from the view configuration with in the (reference) space at the display time.
        std::vector<XrView> views(m_viewConfigurationViews.size(), {XR_TYPE_VIEW});

        XrViewState viewState{XR_TYPE_VIEW_STATE};  // Will contain information on whether the position and/or orientation is valid and/or tracked.
        XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
        viewLocateInfo.viewConfigurationType = m_viewConfiguration;
        viewLocateInfo.displayTime = renderLayerInfo.predictedDisplayTime;
        viewLocateInfo.space = m_localOrStageSpace;
        uint32_t viewCount = 0;
        XrResult result = xrLocateViews(m_session, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());
        if (result != XR_SUCCESS) {
            std::cout << "Failed to locate Views." << std::endl;
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

        renderCuboidIndex = 0;
        // Draw a floor. Scale it by 2 in the X and Z, and 0.1 in the Y,
        RenderCuboid({{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, -m_viewHeightM, 0.0f}}, {2.0f, 0.1f, 2.0f}, {0.4f, 0.5f, 0.5f});
        // Draw a "table".
        RenderCuboid({{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, -m_viewHeightM + 0.9f, -0.7f}}, {1.0f, 0.2f, 1.0f}, {0.6f, 0.6f, 0.4f});

        // Draw some blocks at the controller positions:
        for (int i = 0; i < 2; i++) {
            if (m_handPoseState[i].isActive) {
                RenderCuboid(m_handPose[i], {0.02f, 0.04f, 0.10f}, {1.f, 1.f, 1.f});
            }
        }
        for (int i = 0; i < blocks.size(); i++) {
            auto &thisBlock = blocks[i];
            XrVector3f sc = thisBlock.scale;
            if (i == nearBlock[0] || i == nearBlock[1])
                sc = thisBlock.scale * 1.05f;
            RenderCuboid(thisBlock.pose, sc, thisBlock.color);
        }

        m_graphicsAPI->EndRendering();

        // Give the swapchain image back to OpenXR, allowing the compositor to use the image.
        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        OPENXR_CHECK(xrReleaseSwapchainImage(m_colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
        OPENXR_CHECK(xrReleaseSwapchainImage(m_depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");

        // Fill out the XrCompositionLayerProjection structure for usage with xrEndFrame().
        renderLayerInfo.layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
        renderLayerInfo.layerProjection.space = m_localOrStageSpace;
        renderLayerInfo.layerProjection.viewCount = static_cast<uint32_t>(renderLayerInfo.layerProjectionViews.size());
        renderLayerInfo.layerProjection.views = renderLayerInfo.layerProjectionViews.data();

        return true;
    }

#if defined(__ANDROID__)
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

private:
    void PollSystemEvents() {
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

    XrViewConfigurationType m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    std::vector<XrViewConfigurationView> m_viewConfigurationViews;

    struct SwapchainInfo {
        XrSwapchain swapchain = XR_NULL_HANDLE;
        int64_t swapchainFormat = 0;
        std::vector<void *> imageViews;
    };
    SwapchainInfo m_colorSwapchainInfo = {};
    SwapchainInfo m_depthSwapchainInfo = {};

    std::vector<XrEnvironmentBlendMode> m_applicationEnvironmentBlendModes = {XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE};
    std::vector<XrEnvironmentBlendMode> m_environmentBlendModes = {};
    XrEnvironmentBlendMode m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;

    XrSpace m_localOrStageSpace = XR_NULL_HANDLE;
    struct RenderLayerInfo {
        XrTime predictedDisplayTime;
        std::vector<XrCompositionLayerBaseHeader *> layers;
        XrCompositionLayerProjection layerProjection = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
        std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
    };

    // In STAGE space, viewHeightM should be 0. In LOCAL space, it should be offset downwards, below the viewer's initial position.
    float m_viewHeightM = 1.5f;

    void *m_vertexBuffer = nullptr;
    void *m_indexBuffer = nullptr;
    void *m_uniformBuffer_Camera = nullptr;
    void *m_uniformBuffer_Normals = nullptr;

    void *m_vertexShader = nullptr, *m_fragmentShader = nullptr;
    void *m_pipeline = nullptr;

    struct Block {
        XrPosef pose;
        XrVector3f scale;
        XrVector3f color;
    };
    std::vector<Block> blocks;
    // Don't let too many blocks get created.
    const size_t MaxBlockCount=100;
    int grabbedBlock[2] = {-1, -1};
    int nearBlock[2] = {-1, -1};

    XrActionSet m_actionSet;
    // An action for grabbing blocks, and an action to change the color of a block.
    XrAction m_grabCubeAction, m_spawnCubeAction, m_changeColorAction;
    // The realtime states of these actions.
    XrActionStateFloat m_grabState[2] = {{XR_TYPE_ACTION_STATE_FLOAT}, {XR_TYPE_ACTION_STATE_FLOAT}};
    XrActionStateBoolean m_changeColorState[2] = {{XR_TYPE_ACTION_STATE_BOOLEAN}, {XR_TYPE_ACTION_STATE_BOOLEAN}};
    XrActionStateBoolean m_spawnCubeState = {XR_TYPE_ACTION_STATE_BOOLEAN};
    // The action haptic vibration of the right controller.
    XrAction m_buzzAction;
    float buzz[2] = {0, 0};
    // The action for getting the hand or controller position and orientation.
    XrAction m_palmPoseAction;
    // The XrPaths for left and right hand hands or controllers.
    XrPath m_handPaths[2] = {0, 0};
    // The space that represents the two hand poses.
    XrSpace m_handPoseSpace[2];
    XrActionStatePose m_handPoseState[2] = {{XR_TYPE_ACTION_STATE_POSE}, {XR_TYPE_ACTION_STATE_POSE}};
    // The current poses obtained from the XrSpace.
    XrPosef m_handPose[2];
};

void OpenXRTutorial_Main(GraphicsAPI_Type apiType) {
    DebugOutput debugOutput; // This redirects std::cerr and std::cout to the IDE's output or Android Studio's logcat.
    std::cout << "OpenXR Tutorial Chapter 6." << std::endl;

    OpenXRTutorial app(apiType);
    app.Run();
}

#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
int main(int argc, char **argv) {
    OpenXRTutorial_Main(XR_TUTORIAL_GRAPHICS_API);
}
/*
int main(int argc, char **argv) {
    OpenXRTutorial_Main(OPENGL);
}
int main(int argc, char **argv) {
    OpenXRTutorial_Main(VULKAN);
}
int main(int argc, char **argv) {
    OpenXRTutorial_Main(D3D11);
}
int main(int argc, char **argv) {
    OpenXRTutorial_Main(D3D12);
}
*/
#elif (__ANDROID__)
android_app *OpenXRTutorial::androidApp = nullptr;
OpenXRTutorial::AndroidAppState OpenXRTutorial::androidAppState = {};

void android_main(struct android_app *app) {
    // Allow interaction with JNI and the JVM on this thread.
    // https://developer.android.com/training/articles/perf-jni#threads
    JNIEnv *env;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    // https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_KHR_loader_init
    // Load xrInitializeLoaderKHR() function pointer. On Android, the loader must be initialised with variables from android_app *.
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
    OPENXR_CHECK(xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR *)&loaderInitializeInfoAndroid), "Failed to initialise Loader for Android.");

    // Set userData and Callback for PollSystemEvents().
    app->userData = &OpenXRTutorial::androidAppState;
    app->onAppCmd = OpenXRTutorial::AndroidAppHandleCmd;

    OpenXRTutorial::androidApp = app;
    OpenXRTutorial_Main(XR_TUTORIAL_GRAPHICS_API);
}
/*
    OpenXRTutorial_Main(OPENGL_ES);
}
    OpenXRTutorial_Main(VULKAN);
}
*/
#endif