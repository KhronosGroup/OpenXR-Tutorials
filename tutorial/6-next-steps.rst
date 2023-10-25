############
6 Next Steps
############

***********************
6.1 Multiview rendering
***********************

.. container:: d3d11

	D3D11 supports rendering to a Texture2DArray RenderTarget, but requires the use of instanced rendering to render to multiviews in the same draw call.

.. container:: d3d12

	D3D12 supports rendering to both eye views with View Instancing, which simplifies the rendering code. `D3D12 View Instancing <https://microsoft.github.io/DirectX-Specs/d3d/ViewInstancing.html>`_.

.. container:: opengl opengles

	OpenGL and OpenGL ES supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL and OpenGL ES Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.

.. container:: Vulkan

	Vulkan supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.

Multiview or View Instancing can be used for stereo rendering by creating one ``XrSwapchain`` that contains 2D array images. This is done by setting the ``XrSwapchainCreateInfo::arraySize`` to ``2``; similarly we also create image views that encompasses the two subresources - one layer for each eye view.

Remember also to update any uniform/constant buffer data types to support multiple view matrics via the use of arrays. Update any changes to shaders paths and set the ``PipelineCreateInfo::viewMask`` member to ``0b11`` for two views.

.. code-block:: cpp
	:emphasize-lines: 11, 21, 27

	uint32_t viewCount = static_cast<uint32_t>(m_viewConfigurationViews.size());

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

	// Similar for depth swapchain

	for (uint32_t i = 0; i < swapchainImageCount; i++) {
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

	// Similar for depth image views

When setting up the rendering code in ``RenderLayer()``, there's no need to repeat rendering code per eye view; instead we call ``xrAcquireSwapchainImage()`` and ``xrWaitSwapchainImage()`` to get the next 2D array image from the swapchain. We are still required to submit an ``XrCompositionLayerProjectionView`` structure for each view in the system, but in the ``XrSwapchainSubImage`` we can set the ``imageArrayIndex`` to specify which layer of the swapchain image we wish to associate with that view. So in the case of stereo rendering, it would be ``0`` for left and ``1`` for right eye views. We attach our 2D array image as an render target/color attachment for the pixel/fragment shader to write to.

.. code-block:: cpp
	:emphasize-lines: 6-13, 21-33, 47

	// Acquire and wait for an image from the swapchain.
	// Get the image index of an image in the swapchain.
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
		m_graphicsAPI->ClearColor(m_colorSwapchainInfo.imageViews[colorImageIndex], 0.17f, 0.17f, 0.17f, 1.00f);
	} else {
		// In AR mode make the background color black.
		m_graphicsAPI->ClearColor(m_colorSwapchainInfo.imageViews[colorImageIndex], 0.00f, 0.00f, 0.00f, 1.00f);
	}
	m_graphicsAPI->ClearDepth(m_depthSwapchainInfo.imageViews[depthImageIndex], 1.0f);

	m_graphicsAPI->SetRenderAttachments(&m_swapchainAndDepthImage.colorImageViews[imageIndex], 1, m_swapchainAndDepthImage.depthImageView, width, eight, m_pipeline);
	
	// [...]

	m_graphicsAPI->EndRendering();

	// Give the swapchain image back to OpenXR, allowing the compositor to use the image.
	XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
	OPENXR_CHECK(xrReleaseSwapchainImage(m_colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
	OPENXR_CHECK(xrReleaseSwapchainImage(m_depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");

Shaders and Pipelines will need to be modified to support multiview rendering.

.. container:: d3d11
	
	Here we use the vertex shader to write to the ``SV_RenderTargetArrayIndex`` System-Value Semantic, which specifies which index in the array within the render target we should emit the primitives data to. This System-Value Semantic must be matched in the pixel shader input. Note that this is only supported when ``D3D11_FEATURE_DATA_D3D11_OPTIONS3::VPAndRTArrayIndexFromAnyShaderFeedingRasterizer`` is set to true. Otherwise, you would need to use a geometry shader. `HLSL Semantics <https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics>`_.
	Also see this example from Microsoft's `OpenXR-MixedReality GitHub repository <https://github.com/microsoft/OpenXR-MixedReality/blob/a46ea22a396a38725043fea91166c6d5b1a49dfc/samples/BasicXrApp/CubeGraphics.cpp>`_ and a reference page from Mixed Reality about `Rendering in DirectX <https://learn.microsoft.com/en-us/windows/mixed-reality/develop/native/rendering-in-directx>`_.

	When executing draw calls remember to set the instance count parameter to ``2``.

	Vertex Shader:

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_DX11_MV.hlsl
		:diff: ../Shaders/VertexShader.hlsl
		:language: hlsl

	Pixel Shader:

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/PixelShader_DX11_MV.hlsl
		:diff: ../Shaders/PixelShader.hlsl
		:language: hlsl


.. container:: d3d12

	First, you need to check that ``D3D12_FEATURE_DATA_D3D12_OPTIONS3::ViewInstancingTier`` doesn't equal ``D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED``. Next, within the pipeline creation, set up the ``D3D12_VIEW_INSTANCING_DESC`` and a ``std::vector<D3D12_VIEW_INSTANCE_LOCATION>`` for the ``ID3D12Pipeline``. Here, we must use ``ID3D12Device2::CreatePipelineState()`` for pipeline creation for ViewInstancing. Set the ``D3D12_VIEW_INSTANCE_LOCATION::RenderTargetArrayIndex`` to ``0`` for left and ``1`` for right eye views. This means that we don't need to write to ``SV_RenderTargetArrayIndex`` in the shader. We set ``D3D12_VIEW_INSTANCING_DESC::Flags`` to ``D3D12_VIEW_INSTANCING_FLAG_NONE``, though it's possible to set it to ``D3D12_VIEW_INSTANCING_FLAG_ENABLE_VIEW_INSTANCE_MASKING`` and use ``ID3D12CommandList2::SetViewInstanceMask()`` to control further which views will be written to.

	.. literalinclude:: ../Chapter6_1_Multiview/Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-at: if (pipelineCI.viewMask != 0) {
		:end-before: #else

	Modify the shader to use ``SV_ViewID`` and compile with shader model 6.1 to allow the vertex shader to use the new ``SV_ViewID`` System-Value Semantic. See `HLSL Semantics <https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics>`_ for its usage.

		Vertex Shader:

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_DX12_MV.hlsl
		:diff: ../Shaders/VertexShader.hlsl
		:language: hlsl

.. container:: opengl

	Enusre you have support for ``GL_OVR_multiview`` by checking the extensions and that you have loaded the ``glFramebufferTextureMultiviewOVR()`` function pointer, if you need to do so. You will use this to create a framebuffer supports rendering to multiple layers.
	See this example from ARM's OpenGL ES SDK for Android `here <https://arm-software.github.io/opengl-es-sdk-for-android/multiview.html>`_, which works for OpenGL too.

	Modify the shader to use ``gl_ViewIndex_OVR`` and the GL_OVR_multiview GLSL extension. The line ``layout(num_views = 2) in`` specifies the number of views the vertex shader a will broadcast to.

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_GL_MV.glsl
		:diff: ../Shaders/VertexShader.glsl
		:language: glsl

.. container:: opengles

	Enusre you have support for ``GL_OVR_multiview`` by checking the extensions and that you have loaded the ``glFramebufferTextureMultiviewOVR()`` function pointer, if you need to do so. You will use this to create a framebuffer supports rendering to multiple layers.
	See this example from ARM's OpenGL ES SDK for Android `here <https://arm-software.github.io/opengl-es-sdk-for-android/multiview.html>`_.

	Modify the shader to use ``gl_ViewIndex_OVR`` and the GL_OVR_multiview GLSL extension. The line ``layout(num_views = 2) in`` specifies the number of views the vertex shader a will broadcast to.

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_GL_MV.glsl
		:diff: ../Shaders/VertexShader.glsl
		:language: glsl

.. container:: vulkan

	First, add the ``VK_KHR_MULTIVIEW_EXTENSION_NAME`` or ``"VK_KHR_multiview"`` string to the device extensions list when creating the ``VkDevice``.

	At pipeline creation, chain via the next pointer a ``VkRenderPassMultiviewCreateInfo`` structure to the ``VkRenderPassCreateInfo`` structure when creating the ``VkRenderPass``. Note that there is similar functionality for ``VK_KHR_dynamic_rendering``. The viewMask specifies the number of views the rasterizer a will broadcast to.

	.. literalinclude:: ../Chapter6_1_Multiview/Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-at: bool multiview = false;
		:end-at: VULKAN_CHECK(vkCreateRenderPass

	Modify the shader to use ``gl_ViewIndex`` and the GL_EXT_multiview GLSL extension.

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_VK_MV.glsl
		:diff: ../Shaders/VertexShader.glsl
		:language: glsl

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
	

*********************
6.3 OpenXR API Layers
*********************

The OpenXR loader has a layer system that allows OpenXR API calls to pass through a number of optional layers, that add some functionality for the application. These are extremely useful for debugging.

The OpenXR SDK provides two API layers for us to use:
In the table below are the layer names and thier associated libraries and json files.

+------------------------------------+-----------------------------------------------+-------------------------------------+
| XR_APILAYER_LUNARG_api_dump        | ``XrApiLayer_api_dump.dll`` or ``.so``        | ``XrApiLayer_api_dump.json``        |
+------------------------------------+-----------------------------------------------+-------------------------------------+
| XR_APILAYER_LUNARG_core_validation | ``XrApiLayer_core_validation.dll`` or ``.so`` | ``XrApiLayer_core_validation.json`` |
+------------------------------------+-----------------------------------------------+-------------------------------------+

XR_APILAYER_LUNARG_api_dump simply logs extra/verbose information to ``std::cout`` descibing in more detail what has happened during that API call. XR_APILAYER_LUNARG_core_validation acts similarly to VK_LAYER_KHRONOS_validation in Vulkan, where the layer intercepts the API call and performs validation to ensure conformance with the specification.

Other runtimes and hardware vendors may provide layers that are useful for debugging your XR application.

To enable API layers, add the ``XR_API_LAYER_PATH=<path>`` environment variable to your project or your system. Something like this: ``XR_API_LAYER_PATH=<openxr_base>/<build_folder>/src/api_layers/;<another_path>``. 

The path must point to a folder containing a ``.json`` file similar to the one for XR_APILAYER_LUNARG_core_validation, shown below:

.. code-block::json
	{
		"file_format_version": "1.0.0",
		"api_layer": {
			"name": "XR_APILAYER_LUNARG_core_validation",
			"library_path": "./XrApiLayer_core_validation.dll",
			"api_version": "1.0",
			"implementation_version": "1",
			"description": "API Layer to perform validation of api calls and parameters as they occur"
		}
	}

This file points to the library that the loader should use for this API layer.

To select which API layers we want to use, there are two ways to do this:
 1. Add the ``XR_ENABLE_API_LAYERS=<layer_name>`` environment variable to your project or your system. Something like this: ``XR_ENABLE_API_LAYERS=XR_APILAYER_LUNARG_test1;XR_APILAYER_LUNARG_test2``.
 2. When creating the ``XrInstance``, specify the requested API layers in the ``XrInstanceCreateInfo`` structure.

Calls to ``xrEnumerateApiLayerProperties()`` should now return a pointer to an array of structs and the count of all API layers available to the application.

For more details, please see `API Layers README <https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/main/src/api_layers/README.md>`_ and see `OpenXR API Layers <https://registry.khronos.org/OpenXR/specs/1.0/loader.html#openxr-api-layers>`_.

*****************
6.4 Color Science
*****************

As OpenXR support both linear and sRGB color spaces for compositing. It is helpful to have a deeper knowledge of color science; especially if you are planning to use sRGB formats and have the OpenXR runtime/compositor do automatic conversions for you.

For more information on color spaces and gamma encoding, see Guy Davidson's `video presentation <https://www.youtube.com/watch?v=_zQ_uBAHA4A>`_.