############
6 Next Steps
############

***********************
6.1 Multiview rendering
***********************

* D3D11 supports rendering to a Texture2DArray RenderTarget, but requires the use of instanced rendering to render to multiviews in the same draw call.
* D3D12 supports rendering to both eye views with View Instancing, which simplifies the rendering code. `D3D12 View Instancing <https://microsoft.github.io/DirectX-Specs/d3d/ViewInstancing.html>`_.
* Vulkan supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.
* OpenGL supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.

View Instancing/Multiview can be used for stereo rendering by creating one ``XrSwapchain`` with 2D array images. This is done by setting the ``XrSwapchainCreateInfo::arraySize`` to ``2``; similarly we also create image views that compasses the two subresources one for each eye view.

.. code-block:: cpp
	:emphasize-lines: 9, 25

	XrSwapchainCreateInfo swapchainCI{XR_TYPE_SWAPCHAIN_CREATE_INFO};
	swapchainCI.createFlags = 0;
	swapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCI.format = m_graphicsAPI->SelectColorSwapchainFormat(formats);          // Use 	GraphicsAPI to select the first compatible format.
	swapchainCI.sampleCount = viewConfigurationView.recommendedSwapchainSampleCount;  // Use the 	recommended values from the XrViewConfigurationView.
	swapchainCI.width = viewConfigurationView.recommendedImageRectWidth;
	swapchainCI.height = viewConfigurationView.recommendedImageRectHeight;
	swapchainCI.faceCount = 1;
	swapchainCI.arraySize = 2;
	swapchainCI.mipCount = 1;
	OPENXR_CHECK(xrCreateSwapchain(m_session, &swapchainCI, &swapchainAndDepthImage.swapchain), "Failed to create Swapchain");

	// [...]

	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		GraphicsAPI::ImageViewCreateInfo imageViewCI;
		imageViewCI.image = m_graphicsAPI->GetSwapchainImage(swapchainAndDepthImage.swapchain, i);
		imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::RTV;
		imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
		imageViewCI.format = swapchainAndDepthImage.swapchainFormat;
		imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::COLOR_BIT;
		imageViewCI.baseMipLevel = 0;
		imageViewCI.levelCount = 1;
		imageViewCI.baseArrayLayer = 0;
		imageViewCI.layerCount = 2;
		swapchainAndDepthImage.colorImageViews.push_back(m_graphicsAPI->CreateImageView(imageViewCI));
	}

When setting up the rendering code in ``RenderLayer()``, there's no need to repeat rendering code per eye view; instead we call ``xrAcquireSwapchainImage()`` and ``xrWaitSwapchainImage()`` to get the next 2D array from the swapchain. We are still required to submit an ``XrCompositionLayerProjectionView`` struct for each view in the system, but in the ``XrSwapchainSubImage`` we can set the ``imageArrayIndex`` to specify which layer of the swapchain image we wish to associate with that view; so in the case of stereo rendering it would be ``0`` for left and ``1`` for right eye views. We attach our 2D array image as an render target/color attachment for the pixel/fragment shader to write to.

.. code-block:: cpp
	:emphasize-lines: 5-10, 18-30, 44

	// Acquire and wait for an image from the swapchain.
	// Get the image index of an image in the swapchain.
	// The timeout is infinite.
	uint32_t imageIndex = 0;
	XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
	OPENXR_CHECK(xrAcquireSwapchainImage(m_swapchainAndDepthImage.swapchain, &acquireInfo, &imageIndex), "Failed to acquire Image from the Swapchian");

	XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
	waitInfo.timeout = XR_INFINITE_DURATION;
	OPENXR_CHECK(xrWaitSwapchainImage(m_swapchainAndDepthImage.swapchain, &waitInfo), "Failed to wait for Image from the Swapchain");

	// Get the width and height and construct the viewport and scissors.
	const uint32_t &width = m_viewConfigurationViews[0].recommendedImageRectWidth;
	const uint32_t &height = m_viewConfigurationViews[0].recommendedImageRectHeight;
	GraphicsAPI::Viewport viewport = {0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
	GraphicsAPI::Rect2D scissor = {{(int32_t)0, (int32_t)0}, {width, height}};

	// Fill out the XrCompositionLayerProjectionView structure specifying the pose and fov from the view.
	// This also associates the swapchain image with this layer projection view.
	for (uint32_t i = 0; i < viewCount; i++) {
		layerProjectionViews = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
		layerProjectionViews.pose = views.pose;
		layerProjectionViews.fov = views.fov;
		layerProjectionViews.subImage.swapchain = m_swapchainAndDepthImages.swapchain;
		layerProjectionViews.subImage.imageRect.offset.x = 0;
		layerProjectionViews.subImage.imageRect.offset.y = 0;
		layerProjectionViews.subImage.imageRect.extent.width = static_cast<int32_t>(width);
		layerProjectionViews.subImage.imageRect.extent.height = static_cast<int32_t>(height);
		layerProjectionViews.subImage.imageArrayIndex = i;  // Select the layer for this view. Left = 0, Right = 1.
	}

	// Rendering code to clear the color and depth image views.
	m_graphicsAPI->BeginRendering();

	if (m_environmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_OPAQUE) {
		// VR mode use a background color.
		m_graphicsAPI->ClearColor(m_swapchainAndDepthImage.colorImageViews[imageIndex], 0.17f, 0.17f, 0.17f, 1.00f);
	} else {
		// In AR mode make the background color black.
		m_graphicsAPI->ClearColor(m_swapchainAndDepthImage.colorImageViews[imageIndex], 0.00f, 0.00f, 0.00f, 1.00f);
	}
	m_graphicsAPI->ClearDepth(m_swapchainAndDepthImage.depthImageView, 1.0f);

	m_graphicsAPI->SetRenderAttachments(&m_swapchainAndDepthImage.colorImageViews[imageIndex], 1, m_swapchainAndDepthImage.depthImageView, width, eight, m_pipeline);
	
	// [...]

	m_graphicsAPI->EndRendering();

	// Give the swapchain image back to OpenXR, allowing the compositor to use the image.
	XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
	OPENXR_CHECK(xrReleaseSwapchainImage(m_swapchainAndDepthImage.swapchain, &releaseInfo), "Failed to release Image back to the Swapchain");

Shaders and Pipelines will need to be modified to support multiview rendering.

.. container:: d3d11
	
	Here we use the vertex shader to write to the ``SV_RenderTargetArrayIndex`` System-Value Semantics. Note that this is only supported when ``D3D11_FEATURE_DATA_D3D11_OPTIONS3::VPAndRTArrayIndexFromAnyShaderFeedingRasterizer`` is set to true. Otherwise, you would need to use a geometry shader. `HLSL Semantics <https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics>`_.
	Also see this example from Microsoft's OpenXR-MixedReality GitHub repository `here <https://github.com/microsoft/OpenXR-MixedReality/blob/a46ea22a396a38725043fea91166c6d5b1a49dfc/samples/BasicXrApp/CubeGraphics.cpp>_`.

	.. code-block:: hlsl
		:emphasize-lines: 4, 5, 20, 26, 35, 36

		// Vertex Shader
		cbuffer CameraConstants : register(b0)
		{
			float4x4 viewProj[2];
			float4x4 modelViewProj[2];
			float4x4 model;
			float4 color;
			float4 pad1;
			float4 pad2;
			float4 pad3;
		};
		cbuffer Normals : register(b1)
		{
			float4 normals[6];
		};
		
		struct VS_IN
		{
			uint vertexId : SV_VertexId;
			uint instanceId : SV_InstanceId;
			float4 a_Positions : TEXCOORD0;
		};
		struct VS_OUT
		{
			float4 o_Position : SV_Position;
			uint viewId : SV_RenderTargetArrayIndex;
			nointerpolation float2 o_TexCoord : TEXCOORD0;
			float3 o_Normal : TEXCOORD1;
			nointerpolation float3 o_Color : TEXCOORD2;
		};
		
		VS_OUT main(VS_IN IN)
		{
			VS_OUT OUT;
			OUT.o_Position = mul(modelViewProj[IN.instanceId], IN.a_Positions);
			OUT.viewId = IN.instanceId;
			int face = IN.vertexId / 6;
			OUT.o_TexCoord = float2(float(face), 0);
			OUT.o_Normal = (mul(model, normals[face])).xyz;
			OUT.o_Color = color.rgb;
			return OUT;
		}

.. container:: d3d12

	Set up the ``D3D12_VIEW_INSTANCING_DESC`` and ``D3D12_VIEW_INSTANCE_LOCATION`` for the ``ID3D12Pipeline``.
	Set the RenderTargetArrayIndex to ``0`` for left and ``1`` for right eye views. We don't need to write to SV_RenderTargetArrayIndex in the shader.

	.. code-block:: cpp

		D3D12_VIEW_INSTANCING_DESC viewInstancingDesc;
		std::vector<D3D12_VIEW_INSTANCE_LOCATION> viewInstanceLocations;

		viewInstancingDesc.ViewInstanceCount = 2;
		viewInstanceLocations.resize(viewInstancingDesc.ViewInstanceCount);
		for (size_t i = 0; i < viewInstanceLocations.size(); i++)
		{
			viewInstanceLocations.RenderTargetArrayIndex = static_cast<UINT>(i);
			viewInstanceLocations.ViewportArrayIndex = 0;
		}
		viewInstancingDesc.pViewInstanceLocations = viewInstanceLocations.data();
		viewInstancingDesc.Flags = D3D12_VIEW_INSTANCING_FLAG_ENABLE_VIEW_INSTANCE_MASKING;

	Modify the shader to use ``SV_ViewID`` and use shader model 6.1.
	See `HLSL Semantics <https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics>`_ for the usage of ``SV_ViewId``.

	.. code-block:: hlsl
		:emphasize-lines: 4, 5, 20, 34, 35

		// Vertex Shader
		cbuffer CameraConstants : register(b0)
		{
			float4x4 viewProj[2];
			float4x4 modelViewProj[2];
			float4x4 model;
			float4 color;
			float4 pad1;
			float4 pad2;
			float4 pad3;
		};
		cbuffer Normals : register(b1)
		{
			float4 normals[6];
		};

		struct VS_IN
		{
			uint vertexId : SV_VertexId;
			uint viewId : SV_ViewId
			float4 a_Positions : TEXCOORD0;
		};
		struct VS_OUT
		{
			float4 o_Position : SV_Position;
			nointerpolation float2 o_TexCoord : TEXCOORD0;
			float3 o_Normal : TEXCOORD1;
			nointerpolation float3 o_Color : TEXCOORD2;
		};

		VS_OUT main(VS_IN IN)
		{
			VS_OUT OUT;
			OUT.o_Position = mul(modelViewProj[IN.viewId], IN.a_Positions);
			int face = IN.vertexId / 6;
			OUT.o_TexCoord = float2(float(face), 0);
			OUT.o_Normal = (mul(model, normals[face])).xyz;
			OUT.o_Color = color.rgb;
			return OUT;
		}

.. container:: opengl

	TODO: Check this!

	Enusre you have support for GL_OVR_multiview  and have loaded the ``glFramebufferTextureMultiviewOVR()`` function. You will use this to create a framebuffer to render to.
	Also see this example from ARM's OpenGL ES SDK for Android `here <https://arm-software.github.io/opengl-es-sdk-for-android/multiview.html>`_.

	Modify the shader to use ``gl_ViewIndex_OVR`` and the GL_OVR_multiview GLSL extension.

	.. code-block:: glsl
		:emphasize-lines: 4, 6, 7, 22

		// Vertex Shader
		#version 450
		#extension GL_KHR_vulkan_glsl : enable
		#extension GL_OVR_multiview : enable
		layout(std140, binding = 0) uniform CameraConstants {
			mat4 viewProj[2];
			mat4 modelViewProj[2];
			mat4 model;
			vec4 color;
			vec4 pad1;
			vec4 pad2;
			vec4 pad3;
		};
		layout(std140, binding = 1) uniform Normals {
			vec4 normals[6];
		};
		layout(location = 0) in vec4 a_Positions;
		layout(location = 0) out flat uvec2 o_TexCoord;
		layout(location = 1) out flat vec3 o_Normal;
		layout(location = 2) out flat vec3 o_Color;
		void main() {
			gl_Position = modelViewProj[gl_ViewIndex_OVR] * a_Positions;
			int face = gl_VertexIndex / 6;
			o_TexCoord = uvec2(face, 0);
			o_Normal = (model * normals[face]).xyz;
			o_Color = color.rgb;
		}

.. container:: opengles

	Enusre you have support for GL_OVR_multiview  and have loaded the ``glFramebufferTextureMultiviewOVR()`` function. You will use this to create a framebuffer to render to.
	Also see this example from ARM's OpenGL ES SDK for Android `here <https://arm-software.github.io/opengl-es-sdk-for-android/multiview.html>`_.

	Modify the shader to use ``gl_ViewIndex_OVR`` and the GL_OVR_multiview GLSL extension.

	.. code-block:: glsl
		:emphasize-lines: 2, 4, 5, 20
		
		#version 310 es
		#extension GL_OVR_multiview : enable
		layout(std140, binding = 0) uniform CameraConstants {
			mat4 viewProj[2];
			mat4 modelViewProj[2];
			mat4 model;
			vec4 colour;
			vec4 pad1;
			vec4 pad2;
			vec4 pad3;
		};
		layout(std140, binding = 1) uniform Normals {
			vec4 normals[6];
		};
		layout(location = 0) in highp vec4 a_Positions;
		layout(location = 0) out flat uvec2 o_TexCoord;
		layout(location = 1) out highp vec3 o_Normal;
		layout(location = 2) out flat vec3 o_Colour;
		void main() {
			gl_Position = modelViewProj[gl_ViewID_OVR] * a_Positions;
			int face = gl_VertexID / 6;
			o_TexCoord = uvec2(face, 0);
			o_Normal = (model * normals[face]).xyz;
			o_Colour = colour.rgb;
		}

.. container:: vulkan

	Add ``VkRenderPassMultiviewCreateInfo`` to the ``VkRenderPassCreateInfo`` when creating the ``VkPipeline``. Note that there is similar functionality for Vulkan 1.3 - Dynamic Rendering.

	.. code-block:: cpp

		VkRenderPassCreateInfo renderPassCI = {};

		// [...]

		uint32_t viewMask = 0b11; //Render to layers 0 and 1

		VkRenderPassMultiviewCreateInfo multiviewCreateInfo;
		multiviewCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
		multiviewCreateInfo.pNext = nullptr;
		multiviewCreateInfo.subpassCount = 1;
		multiviewCreateInfo.pViewMasks = &viewMask;
		multiviewCreateInfo.dependencyCount = 0;
		multiviewCreateInfo.pViewOffsets = nullptr;
		multiviewCreateInfo.correlationMaskCount = 1;
		multiviewCreateInfo.pCorrelationMasks = &viewMask;
		renderPassCI.pNext = &multiviewCreateInfo;

	Modify the shader to use ``gl_ViewIndex`` and the GL_EXT_multiview GLSL extension.

	.. code-block:: glsl
		:emphasize-lines: 4, 6, 7, 22

		// Vertex Shader
		#version 450
		#extension GL_KHR_vulkan_glsl : enable
		#extension GL_EXT_multiview : enable
		layout(std140, binding = 0) uniform CameraConstants {
			mat4 viewProj[2];
			mat4 modelViewProj[2];
			mat4 model;
			vec4 color;
			vec4 pad1;
			vec4 pad2;
			vec4 pad3;
		};
		layout(std140, binding = 1) uniform Normals {
			vec4 normals[6];
		};
		layout(location = 0) in vec4 a_Positions;
		layout(location = 0) out flat uvec2 o_TexCoord;
		layout(location = 1) out flat vec3 o_Normal;
		layout(location = 2) out flat vec3 o_Color;
		void main() {
			gl_Position = modelViewProj[gl_ViewIndex] * a_Positions;
			int face = gl_VertexIndex / 6;
			o_TexCoord = uvec2(face, 0);
			o_Normal = (model * normals[face]).xyz;
			o_Color = color.rgb;
		}


****************
6.2 Graphics API
****************

.. rubric::  GraphicsAPI

Note: ``GraphicsAPI`` is by no means production-ready code or reflective of good practice with specific APIs. It is there solely to provide working samples in this tutorial, and demonstrate some basic rendering and interaction with OpenXR.

This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. The derived class is based on your graphics API selection. Include both the header and cpp files for both ``GraphicsAPI`` and ``GraphicsAPI...``. ``GraphicsAPI.h`` includes the headers and macros needed to set up your platform and graphics API. Below are code snippets that show how to set up the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros for your platform along with any relevant headers. In the first code block, there's also reference to ``XR_TUTORIAL_USE_...`` which we set up the ``CMakeLists.txt`` . This tutorial demonstrates all five graphics APIs, you will only need to select one ``XR_USE_PLATFORM_...`` macro and one ``XR_USE_GRAPHICS_API_...`` macro.

The code below is an example of how you might implement the inclusion and definition the relevant graphics API header along with the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros. This will already be set up in the ``GraphicsAPI.h`` file.

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: #include <HelperFunctions
	:end-at: .h>

.. container:: windows

	.. container:: d3d11

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif  // _WIN32
			:emphasize-lines: 6, 8-10

	.. container:: d3d12

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif  // _WIN32
			:emphasize-lines: 6, 11-13

	.. container:: opengl

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif  // _WIN32
			:emphasize-lines: 6, 14-16

	.. container:: vulkan

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif  // _WIN32
			:emphasize-lines: 6, 17-19

.. container:: linux

	.. container:: opengl

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__linux__)
			:end-at: #endif  // __linux__
			:emphasize-lines: 2-13, 15-17

	.. container:: vulkan

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__linux__)
			:end-at: #endif  // __linux__
			:emphasize-lines: 2-13, 18-20

.. container:: android

	.. container:: opengles

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__ANDROID__)
			:end-at: endif  // __ANDROID__
			:emphasize-lines: 3, 5-7

	.. container:: vulkan

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__ANDROID__)
			:end-at: endif  // __ANDROID__
			:emphasize-lines: 3, 8-10

.. container:: d3d11

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_D3D11)
		:end-at: #endif

.. container:: d3d12

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_D3D12)
		:end-at: #endif

.. container:: opengl

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_OPENGL)
		:end-at: #endif

.. container:: opengles

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
		:end-at: #endif

.. container:: vulkan

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_VULKAN)
		:end-at: #endif

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: // OpenXR Helper
	:end-at: #include <OpenXRHelper.h>

When setting up the graphics API core obejcts, there are things that we need to know from OpenXR in order to create the objects correctly. These could include the version of the graphics API required, referencing a specific GPU, required instance and/r device extension etc. Below are code examples showing how to setup your graphics for OpenXR.

.. container:: d3d11

	.. literalinclude:: ../Common/GraphicsAPI_D3D11.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_D3D11

.. container:: d3d12

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_D3D12

.. container:: opengl

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_OpenGL

.. container:: opengles

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES

.. container:: vulkan

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_Vulkan

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_LoadPFN_XrFunctions
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_Vulkan_LoadPFN_XrFunctions

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: // XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR
		:end-before: // XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR
	

*****************
6.3 Color Science
*****************

As OpenXR support both linear and sRGB color spaces for compositing. It is helpful to have a deeper knowledge of color science; especially if you are planning to use sRGB formats and have the OpenXR runtime/compositor do automatic conversions for you.

For more information on color spaces and gamma encoding, see Guy Davidson's video presentation `here <https://www.youtube.com/watch?v=_zQ_uBAHA4A>`_.