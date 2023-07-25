// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

#include "DebugOutput.h"
#include "GraphicsAPIs.h"
#include "OpenXRDebugUtils.h"
// XR_DOCS_TAG_BEGIN_include_linear_algebra
#include "xr_linear_algebra.h"
// XR_DOCS_TAG_END_include_linear_algebra

#define XR_DOCS_CHAPTER_VERSION XR_DOCS_CHAPTER_4_5

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
		CreateResources();

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
		CreateSwapchain();
#endif
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
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_2_2
//XR_DOCS_TAG_BEGIN_CallDestroyResources
		DestroyResources();
//XR_DOCS_TAG_END_CallDestroyResources
		DestroySession();
#endif

		DestroyDebugMessenger();
		DestroyInstance();
	}

private:
	void CreateInstance() {
        // XR_DOCS_TAG_BEGIN_XrApplicationInfo
		XrApplicationInfo AI;
		strncpy(AI.applicationName, "OpenXR Tutorial Chapter 4",XR_MAX_APPLICATION_NAME_SIZE);
		AI.applicationVersion = 1;
		strncpy(AI.engineName, "OpenXR Engine",XR_MAX_ENGINE_NAME_SIZE);
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
		OPENXR_CHECK(xrCreateInstance(&instanceCI, &xrInstance), "Failed to create Instance.");
        // XR_DOCS_TAG_END_XrInstanceCreateInfo
	}

	void DestroyInstance() {
		OPENXR_CHECK(xrDestroyInstance(xrInstance), "Failed to destroy Instance.");
	}

    // XR_DOCS_TAG_BEGIN_Create_DestroyDebugMessenger
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
    // XR_DOCS_TAG_END_Create_DestroyDebugMessenger

    // XR_DOCS_TAG_BEGIN_GetInstanceProperties
	void GetInstanceProperties() {
		XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
		OPENXR_CHECK(xrGetInstanceProperties(xrInstance, &instanceProperties), "Failed to get InstanceProperties.");

		std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - ";
		std::cout << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_PATCH(instanceProperties.runtimeVersion) << std::endl;
	}
    // XR_DOCS_TAG_END_GetInstanceProperties

    // XR_DOCS_TAG_BEGIN_GetSystemID
	void GetSystemID() {
		XrSystemGetInfo systemGI{XR_TYPE_SYSTEM_GET_INFO};
		systemGI.formFactor = formFactor;
		OPENXR_CHECK(xrGetSystem(xrInstance, &systemGI, &systemID), "Failed to get SystemID.");

		XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
		OPENXR_CHECK(xrGetSystemProperties(xrInstance, systemID, &systemProperties), "Failed to get SystemProperties.");
	}
    // XR_DOCS_TAG_END_GetSystemID

// XR_DOCS_TAG_BEGIN_CreateActionSet
	void CreateActionSet() {
		XrActionSetCreateInfo actionset_info = {XR_TYPE_ACTION_SET_CREATE_INFO};
		strncpy(actionset_info.actionSetName, "openxr-tutorial-actionset", XR_MAX_ACTION_SET_NAME_SIZE);
		strncpy(actionset_info.localizedActionSetName, "OpenXR Tutorial ActionSet", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
		OPENXR_CHECK(xrCreateActionSet(xrInstance, &actionset_info, &actionSet), "xrCreateActionSet");
// XR_DOCS_TAG_END_CreateActionSet

// XR_DOCS_TAG_BEGIN_CreateActions
		auto CreateAction = [this](XrAction &xrAction, const char *name, XrActionType xrActionType) {
			XrActionCreateInfo action_info = {XR_TYPE_ACTION_CREATE_INFO};
			action_info.actionType = xrActionType;
			strncpy(action_info.actionName, name, XR_MAX_ACTION_NAME_SIZE);
			strncpy(action_info.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE);
			OPENXR_CHECK(xrCreateAction(actionSet, &action_info, &xrAction), "Failed to create xrAction.");
		};
		CreateAction(selectAction		, "select"		, XR_ACTION_TYPE_BOOLEAN_INPUT);
		CreateAction(triggerAction		, "trigger"		, XR_ACTION_TYPE_FLOAT_INPUT);
		CreateAction(leftGripPoseAction	, "left-grip"	, XR_ACTION_TYPE_POSE_INPUT);
		CreateAction(rightHapticAction	, "right-haptic", XR_ACTION_TYPE_VIBRATION_OUTPUT);
	}
// XR_DOCS_TAG_END_CreateActions

// XR_DOCS_TAG_BEGIN_CreateXrPath
	XrPath CreateXrPath(const char *path_string) {
		XrPath xrPath;
		OPENXR_CHECK(xrStringToPath(xrInstance, path_string, &xrPath), "Failed to create path from string.");
		return xrPath;
	}
// XR_DOCS_TAG_END_CreateXrPath
// XR_DOCS_TAG_BEGIN_SuggestBindings
	void SuggestBindings() {
		std::vector<XrActionSuggestedBinding> suggestedBindings=
										{{selectAction		,CreateXrPath("/user/hand/left/input/select/click")}
										,{triggerAction		,CreateXrPath("/user/hand/right/input/menu/click")}
										,{leftGripPoseAction,CreateXrPath("/user/hand/left/input/grip/pose")}
										,{rightHapticAction	,CreateXrPath("/user/hand/right/output/haptic")}
								   };
		// The application can call xrSuggestInteractionProfileBindings once per interaction profile that it supports.
		XrInteractionProfileSuggestedBinding suggested_binds = {XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
		suggested_binds.interactionProfile = CreateXrPath("/interaction_profiles/khr/simple_controller");
		suggested_binds.suggestedBindings = suggestedBindings.data();
		suggested_binds.countSuggestedBindings = (uint32_t)suggestedBindings.size();
		OPENXR_CHECK(xrSuggestInteractionProfileBindings(xrInstance, &suggested_binds), "xrSuggestInteractionProfileBindings failed.");
	}
// XR_DOCS_TAG_END_SuggestBindings
// XR_DOCS_TAG_BEGIN_CreateActionPoses
	void CreateActionPoses() {
		// Create an xrSpace for a pose action.
		auto CreateActionPoseSpace = [this](XrSession session, XrAction xrAction) -> XrSpace {
			// Create frame of reference for a pose action
			XrActionSpaceCreateInfo action_space_info = {XR_TYPE_ACTION_SPACE_CREATE_INFO};
			action_space_info.action = xrAction;
			const XrPosef xr_pose_identity = {{0, 0, 0, 1.0f}, {0, 0, 0}};
			action_space_info.poseInActionSpace = xr_pose_identity;
			XrSpace xrSpace;
			OPENXR_CHECK(xrCreateActionSpace(session, &action_space_info, &xrSpace), "Failed to create Action Space.");
			return xrSpace;
		};
		leftGripPoseSpace = CreateActionPoseSpace(session, leftGripPoseAction);
	}
// XR_DOCS_TAG_END_CreateActionPoses
// XR_DOCS_TAG_BEGIN_AttachActionSet
	void AttachActionSet() {
		// Attach the action set we just made to the session. We could attach multiple action sets!
		XrSessionActionSetsAttachInfo attach_info = {XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
		attach_info.countActionSets = 1;
		attach_info.actionSets = &actionSet;
		OPENXR_CHECK(xrAttachSessionActionSets(session, &attach_info), "Failed to attach ActionSet to Session.");
	}
// XR_DOCS_TAG_END_AttachActionSet
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

    // XR_DOCS_TAG_BEGIN_CreateDestroySession
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
    // XR_DOCS_TAG_END_CreateDestroySession
// XR_DOCS_TAG_BEGIN_CreateResources1
	struct CameraConstants
	{
		XrMatrix4x4f viewProj;
		XrMatrix4x4f modelViewProj;
		XrMatrix4x4f model;
	};
	CameraConstants cameraConstants;
	// six colours for the six faces of a cube. Bright for +, Dark is -
	// Red for X faces, green for Y, blue for Z.
    XrVector4f colours[6]={	 {1.0f	,0		,0		,1.f}
							,{0.1f	,0		,0		,1.f}
							,{0		,0.6f	,0		,1.f}
							,{0		,0.1f	,0		,1.f}
							,{0		,0.2f	,1.0f	,1.f}
							,{0		,0.02f	,0.1f	,1.f}};
	void CreateResources() {
		// Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
		constexpr XrVector4f vertexPositions[]={
                                                 { 0.5f,  0.5f,  0.5f, 1.f}
                                                   ,{ 0.5f,  0.5f, -0.5f, 1.f}
                                                  ,{ 0.5f, -0.5f,  0.5f, 1.f}
                                                    ,{ 0.5f, -0.5f, -0.5f, 1.f}
                                                   ,{-0.5f,  0.5f,  0.5f, 1.f}
                                                    ,{-0.5f,  0.5f, -0.5f, 1.f}
                                                     ,{-0.5f, -0.5f,  0.5f, 1.f}
                                                    ,{-0.5f, -0.5f, -0.5f, 1.f}
        };

		#define CUBE_FACE(V1, V2, V3, V4, V5, V6) vertexPositions[V1], vertexPositions[V2], vertexPositions[V3], vertexPositions[V4], vertexPositions[V5], vertexPositions[V6],

		XrVector4f cubeVertices[] = {
			CUBE_FACE(2, 1, 0, 2, 3, 1)		// -X
			CUBE_FACE(6, 4, 5, 6, 5, 7)	    // +X
			CUBE_FACE(0, 1, 5, 0, 5, 4)	// -Y
			CUBE_FACE(2, 6, 7, 2, 7, 3)	// +Y
			CUBE_FACE(0, 4, 6, 0, 6, 2)	// -Z
			CUBE_FACE(1, 3, 7, 1, 7, 5)	// +Z
		};

		uint32_t cubeIndices[36] = {
			0,  1,  2,  3,  4,  5,   // -X
			6,  7,  8,  9,  10, 11,  // +X
			12, 13, 14, 15, 16, 17,  // -Y
			18, 19, 20, 21, 22, 23,  // +Y
			24, 25, 26, 27, 28, 29,  // -Z
			30, 31, 32, 33, 34, 35,  // +Z
		};
		vertexBuffer = graphicsAPI->CreateBuffer(
			{GraphicsAPI::BufferCreateInfo::Type::VERTEX, sizeof(float) * 4, sizeof(cubeVertices),
			 &cubeVertices, false});

		indexBuffer = graphicsAPI->CreateBuffer(
			{GraphicsAPI::BufferCreateInfo::Type::INDEX, sizeof(uint32_t), sizeof(cubeIndices),
			 &cubeIndices, false});

		uniformBuffer_Frag = graphicsAPI->CreateBuffer(
			{GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(colours), colours, false});
			
		uniformBuffer_Vert = graphicsAPI->CreateBuffer(
			{GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(CameraConstants), &cameraConstants, false});

// XR_DOCS_TAG_END_CreateResources1
// XR_DOCS_TAG_BEGIN_CreateResources2_OpenGL_Vulkan
		if (apiType == OPENGL || apiType == VULKAN) {
			std::string vertexSource = R"(
				#version 450
				//Color Vertex Shader
				layout(std140, binding = 1) uniform CameraConstants
				{
                    mat4 viewProj;
                    mat4 modelViewProj;
                    mat4 model;
				};
				layout(location = 0) in highp vec4 a_Positions;
				layout(location = 0) out highp vec2 o_TexCoord;
				void main()
				{
					gl_Position = modelViewProj*a_Positions;
					int face=gl_VertexID/6;
					o_TexCoord=vec2(float(face),0);
				})";
			vertexShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

			std::string fragmentSource = R"(
				#version 450
				//Texture Fragment Shader
				layout(location = 0) in highp vec2 i_TexCoord;
				layout(location = 0) out highp vec4 o_Color;
				layout(std140, binding = 0) uniform Data
				{
					highp vec4 colours[6];
				} d_Data;
				void main()
				{
					int i=int(i_TexCoord.x);
					o_Color = d_Data.colours[i];
				})";
			fragmentShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
		}
// XR_DOCS_TAG_END_CreateResources2_OpenGL_Vulkan
// XR_DOCS_TAG_BEGIN_CreateResources2_OpenGLES
		if (apiType == OPENGL_ES) {
			std::string vertexSource = R"(
				#version 310 es
				//Color Vertex Shader
				layout(std140, binding = 1) uniform CameraConstants
				{
                    mat4 viewProj;
                    mat4 modelViewProj;
                    mat4 model;
				};
				layout(location = 0) in highp vec4 a_Positions;
				layout(location = 0) out highp vec2 o_TexCoord;
				void main()
				{
					gl_Position = modelViewProj*a_Positions;
					int face=gl_VertexID/6;
					o_TexCoord=vec2(float(face),0);
				})";
			vertexShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

			std::string fragmentSource = R"(
				#version 310 es
				//Color Fragment Shader
				layout(location = 0) in highp vec2 i_TexCoord;
				layout(location = 0) out highp vec4 o_Color;
				layout(std140, binding = 0) uniform Data
				{
					highp vec4 colours[6];
				} d_Data;
				
				void main()
				{
					int i=int(i_TexCoord.x);
					o_Color = d_Data.colours[i];
				})";
			fragmentShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
		}
// XR_DOCS_TAG_END_CreateResources2_OpenGLES
// XR_DOCS_TAG_BEGIN_CreateResources2_D3D
		if (apiType == D3D11  || apiType == D3D12) {
			std::string vertexSource = R"(
				//Color Vertex Shader

				cbuffer CameraConstants
				{
                    mat4 viewProj;
                    mat4 modelViewProj;
                    mat4 model;
				};
				struct VS_IN
				{
					float4 a_Positions : TEXCOORD0;
				};
				
				struct VS_OUT
				{
					float4 o_Position : SV_Position;
				};
				
				VS_OUT main(VS_IN IN)
				{
					VS_OUT OUT;
					OUT.o_Position = IN.a_Positions;
					return OUT;
				})";
			vertexShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

			std::string fragmentSource = R"(
				//Color Fragment Shader
				
				struct PS_OUT
				{
					float4 o_Color : SV_Target0;
				};
				
				cbuffer Data : register(b0)
				{
					float4 d_Data_color;
				};
				
				PS_OUT main()
				{
					PS_OUT OUT;
					OUT.o_Color = d_Data_color;
					return OUT;
				})";
			fragmentShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
		}
// XR_DOCS_TAG_END_CreateResources2_D3D
// XR_DOCS_TAG_BEGIN_CreateResources3
		GraphicsAPI::PipelineCreateInfo pipelineCI;
		pipelineCI.shaders = {vertexShader, fragmentShader};
		pipelineCI.vertexInputState.attributes = {{0, 0, GraphicsAPI::VertexType::VEC4, 0, "TEXCOORD"}};
		pipelineCI.vertexInputState.bindings = {{0, 0, 4 * sizeof(float)}};
		pipelineCI.inputAssemblyState = {GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, false};
		pipelineCI.rasterisationState = {false, false, GraphicsAPI::PolygonMode::FILL, GraphicsAPI::CullMode::BACK, GraphicsAPI::FrontFace::COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f};
		pipelineCI.multisampleState = {1, false, 1.0f, 0, false, false};
		pipelineCI.depthStencilState = {false, false, GraphicsAPI::CompareOp::GREATER, false, false, {}, {}, 0.0f, 1.0f};
		pipelineCI.colourBlendState = {false, GraphicsAPI::LogicOp::NO_OP, {{true, GraphicsAPI::BlendFactor::SRC_ALPHA, GraphicsAPI::BlendFactor::ONE_MINUS_SRC_ALPHA, GraphicsAPI::BlendOp::ADD, GraphicsAPI::BlendFactor::ONE, GraphicsAPI::BlendFactor::ZERO, GraphicsAPI::BlendOp::ADD, (GraphicsAPI::ColourComponentBit)15}}, {0.0f, 0.0f, 0.0f, 0.0f}};
		pipeline = graphicsAPI->CreatePipeline(pipelineCI);
	}
// XR_DOCS_TAG_END_CreateResources3
// XR_DOCS_TAG_BEGIN_DestroyResources
	void DestroyResources() {
		graphicsAPI->DestroyPipeline(pipeline);
		graphicsAPI->DestroyShader(fragmentShader);
		graphicsAPI->DestroyShader(vertexShader);
		graphicsAPI->DestroyBuffer(uniformBuffer_Vert);
		graphicsAPI->DestroyBuffer(uniformBuffer_Frag);
		graphicsAPI->DestroyBuffer(indexBuffer);
		graphicsAPI->DestroyBuffer(vertexBuffer);
	}
// XR_DOCS_TAG_END_DestroyResources

    // XR_DOCS_TAG_BEGIN_PollEvents
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
    // XR_DOCS_TAG_END_PollEvents
// XR_DOCS_TAG_BEGIN_PollActions
	void PollActions(XrTime predictedTime)
	{
		// Update our action set with up-to-date input data!
		XrActiveActionSet active_action_set = { };
		active_action_set.actionSet = actionSet;
		active_action_set.subactionPath = XR_NULL_PATH;

		XrActionsSyncInfo sync_info = { XR_TYPE_ACTIONS_SYNC_INFO };
		sync_info.countActiveActionSets = 1;
		sync_info.activeActionSets = &active_action_set;
		OPENXR_CHECK(xrSyncActions(session, &sync_info),"Failed to sync actions.");
// XR_DOCS_TAG_END_PollActions
// XR_DOCS_TAG_BEGIN_PollActions2
		XrActionStateGetInfo get_info = { XR_TYPE_ACTION_STATE_GET_INFO };

		get_info.action					= selectAction;
		xrGetActionStateBoolean(session, &get_info, &selectState);
		get_info.action					= triggerAction;
		xrGetActionStateFloat(session, &get_info, &triggerState);
// XR_DOCS_TAG_END_PollActions2
// XR_DOCS_TAG_BEGIN_PollActions3
		get_info.action					= leftGripPoseAction;
        OPENXR_CHECK(xrGetActionStatePose(session, &get_info, &leftGripPoseState),"Failed to get pose.");
		if(leftGripPoseState.isActive)
		{
			XrSpaceLocation space_location = { XR_TYPE_SPACE_LOCATION };
			XrResult		res = xrLocateSpace(leftGripPoseSpace, localOrStageSpace, predictedTime, &space_location);
			if (XR_UNQUALIFIED_SUCCESS(res) &&
				(space_location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
				(space_location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0)
			{
				leftGripPose=space_location.pose;
			}
		}
	}
// XR_DOCS_TAG_END_PollActions3
	
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
		}
	}

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

// XR_DOCS_TAG_BEGIN_RenderCuboid	
	void RenderCuboid( XrPosef pose,XrVector3f scale) {
		XrMatrix4x4f_CreateTranslationRotationScale(&cameraConstants.model, &pose.position, &pose.orientation, &scale);

		XrMatrix4x4f_Multiply(&cameraConstants.modelViewProj, &cameraConstants.viewProj, &cameraConstants.model);

		graphicsAPI->SetPipeline(pipeline);

		graphicsAPI->SetBufferData(uniformBuffer_Vert,0,sizeof(CameraConstants),&cameraConstants);
		graphicsAPI->SetDescriptor({1, uniformBuffer_Vert, GraphicsAPI::DescriptorInfo::Type::BUFFER});

		graphicsAPI->SetBufferData(uniformBuffer_Frag, 0, sizeof(colours), (void*)colours);
		graphicsAPI->SetDescriptor({0, uniformBuffer_Frag, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT});

		graphicsAPI->SetVertexBuffers(&vertexBuffer, 1);
		graphicsAPI->SetIndexBuffer(indexBuffer);

		graphicsAPI->DrawIndexed(36);
	}
// XR_DOCS_TAG_END_RenderCuboid	
	void RenderFrame() {
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_3_2
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
			// poll actions here because they require a predicted display time
#if XR_DOCS_CHAPTER_VERSION >= XR_DOCS_CHAPTER_4_2
// XR_DOCS_TAG_BEGIN_CallPollActions
			PollActions(frameState.predictedDisplayTime);
// XR_DOCS_TAG_END_CallPollActions
#endif
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
#endif
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
			OPENXR_CHECK(xrWaitSwapchainImage(swapchainAndDepthImages[i].swapchain, &waitInfo), "Failed to wait for Image from the Swapchian");

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

			graphicsAPI->ClearColor(swapchainAndDepthImages[i].colorImageViews[imageIndex], 0.22f, 0.17f, 0.35f, 1.0f);
			graphicsAPI->ClearDepth(swapchainAndDepthImages[i].depthImageView, 1.0f);
			
// XR_DOCS_TAG_BEGIN_SetupFrameRendering
			graphicsAPI->SetRenderAttachments(&swapchainAndDepthImages[i].colorImageViews[imageIndex], 1, swapchainAndDepthImages[i].depthImageView);
			graphicsAPI->SetViewports(&viewport, 1);
			graphicsAPI->SetScissors(&scissor, 1);

			// Compute the view-projection transform.
			// All matrices (including OpenXR's) are column-major, right-handed.
			XrMatrix4x4f proj;
			XrMatrix4x4f_CreateProjectionFov(&proj,OPENGL_ES, views[i].fov, 0.05f, 100.0f);
			XrMatrix4x4f toView;
			XrVector3f scale1m{1.f, 1.f, 1.f};
			XrMatrix4x4f_CreateTranslationRotationScale(&toView, &views[i].pose.position, &views[i].pose.orientation, &scale1m);
			XrMatrix4x4f view;
			XrMatrix4x4f_InvertRigidBody(&view, &toView);
			XrMatrix4x4f_Multiply(&cameraConstants.viewProj, &proj, &view);

// XR_DOCS_TAG_END_SetupFrameRendering
// XR_DOCS_TAG_BEGIN_CallRenderCuboid
			// let's draw a cuboid at the floor. Scale it by 2 in the X and Z, and 0.1 in the Y,
			RenderCuboid({{0,0,0,1.f},{0,-viewHeightM,0}}, {2.f,0.1f,2.0f});

			if(leftGripPoseState.isActive)
			{
				XrVector3f grip_scale{0.02f, 0.04f, 0.1f};
				RenderCuboid(leftGripPose, grip_scale);
			}
// XR_DOCS_TAG_END_CallRenderCuboid
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
    // XR_DOCS_TAG_BEGIN_Android_System_Functionality
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
    // XR_DOCS_TAG_END_Android_System_Functionality
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
    // In STAGE space, viewHeightM should be 0. In LOCAL space, it should be offet downwards, below the viewer's initial position.
    float viewHeightM=1.5f;

	void *vertexBuffer=nullptr;
	void *indexBuffer=nullptr;
	void *uniformBuffer_Vert=nullptr;
	void *uniformBuffer_Frag=nullptr;

	void *vertexShader=nullptr, *fragmentShader=nullptr;
	void *pipeline=nullptr;

// XR_DOCS_TAG_BEGIN_Actions
	XrActionSet actionSet;
	// The action for clicking the "select" button.
	XrAction selectAction;
	XrActionStateBoolean selectState= { XR_TYPE_ACTION_STATE_BOOLEAN };
	// The action for squeezing the trigger control.
	XrAction triggerAction;
	XrActionStateFloat triggerState	= { XR_TYPE_ACTION_STATE_FLOAT };
	// The action haptic vibration of the right controller.
	XrAction rightHapticAction;
	// The action for getting the left grip pose.
	XrAction leftGripPoseAction;
	// The space that represents the left grip pose.
	XrSpace leftGripPoseSpace;
	XrActionStatePose leftGripPoseState	= { XR_TYPE_ACTION_STATE_POSE };
	// The current left grip pose obtained from the XrSpace.
	XrPosef leftGripPose;
// XR_DOCS_TAG_END_Actions
};

void OpenXRTutorial_Main() {
	DebugOutput debugOutput;
	std::cout << "OpenXR Tutorial Chapter 4." << std::endl;
	OpenXRTutorial app(OPENGL);
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
android_app *OpenXRTutorial::androidApp = nullptr;
OpenXRTutorial::AndroidAppState OpenXRTutorial::androidAppState = {};

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