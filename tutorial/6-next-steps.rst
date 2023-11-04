############
6 Next Steps
############

So far, you've learned how to create a basic OpenXR application: how to initialize and shut down an OpenXR instance and session, how to connect a graphics API to receive rendering events, and how to poll your runtime for interactions. The next steps are up to you but in these chapter you'll find a few suggestions.

***********************
6.1 Multiview rendering
***********************

Stereoscopic rendering usually involves drawing two very similar views of the same scene, with only a slight difference in perspective due to the separation of the left and right eye positions. It's a great saving - particularly of CPU time - if we can use only one set of draw calls to render both views.

.. container:: d3d11

	D3D11 supports rendering to a Texture2DArray RenderTarget, but requires the use of instanced rendering to render to multiple views in the same draw call.

.. container:: d3d12

	D3D12 supports rendering to both eye views with View Instancing - see `D3D12 View Instancing <https://microsoft.github.io/DirectX-Specs/d3d/ViewInstancing.html>`_.

.. container:: opengl opengles

	OpenGL and OpenGL ES supports rendering to both eye views with multiview - see `OpenGL and OpenGL ES Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.

.. container:: Vulkan

	Vulkan supports rendering to both eye views with multiview - see `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.

Multiview or View Instancing can be used for stereo rendering by creating one :openxr_ref:`XrSwapchain` that contains two array images. This is done by setting the ``arraySize`` of our :openxr_ref:`XrSwapchainCreateInfo` to ``2``; similarly, we create image views that encompass the two subresources - one layer per eye view.

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SingleSwapchainInfo
	:end-before: XR_DOCS_TAG_END_SingleSwapchainInfo
	:dedent: 4

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateViewConfigurationView
	:end-before: XR_DOCS_TAG_END_CreateViewConfigurationView
	:dedent: 8

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateSwapchains
	:end-before: XR_DOCS_TAG_END_CreateSwapchains
	:dedent: 8
	:emphasize-lines: 12, 25

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_EnumerateSwapchainImages
	:end-before: XR_DOCS_TAG_END_EnumerateSwapchainImages
	:dedent: 8

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateImageViews
	:end-before: XR_DOCS_TAG_END_CreateImageViews
	:dedent: 8
	:emphasize-lines: 6, 12, 19, 25


.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroySwapchains
	:end-before: XR_DOCS_TAG_END_DestroySwapchains
	:dedent: 8

Remember also to update any uniform/constant buffer data types to support multiple view matrics via the use of arrays. Update any changes to shaders, shader paths and set the ``PipelineCreateInfo::viewMask`` member to ``0b11`` for two views. (Only found in Chapter6_1_Multiview). Note use of AlignSizeForUniformBuffer().

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateResources1
	:end-before: XR_DOCS_TAG_END_CreateResources1
	:dedent: 4
	:emphasize-lines: 2, 3

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_END_Update_numberOfCuboids
	:end-before: XR_DOCS_TAG_END_CreateResources1_1
	:dedent: 4
	:emphasize-lines: 1

.. container:: d3d11 d3d12

	.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_D3D
		:end-before: XR_DOCS_TAG_END_CreateResources2_D3D
		:dedent: 8
		:emphasize-lines: 2, 5, 9, 12

.. container:: opengl

	.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_OpenGL
		:end-before: XR_DOCS_TAG_END_CreateResources2_OpenGL
		:dedent: 8
		:emphasize-lines: 2, 5

.. container:: opengles

	.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_OpenGLES
		:end-before: XR_DOCS_TAG_END_CreateResources2_OpenGLES
		:dedent: 8
		:emphasize-lines: 2, 4

.. container:: vulkan

	.. container:: windows linux

		.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
			:language: cpp
			:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_VulkanWindowsLinux
			:end-before: XR_DOCS_TAG_END_CreateResources2_VulkanWindowsLinux
			:dedent: 8
			:emphasize-lines: 2, 5

	.. container:: android

		.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
			:language: cpp
			:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_VulkanAndroid
			:end-before: XR_DOCS_TAG_END_CreateResources2_VulkanAndroid
			:dedent: 8
			:emphasize-lines: 2, 4

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateResources3
	:end-before: XR_DOCS_TAG_END_CreateResources3
	:dedent: 8
	:emphasize-lines: 10, 11, 15

In ``RenderLayer()``, there's no need to repeat the rendering code per eye view; instead, we call :openxr_ref:`xrAcquireSwapchainImage` and :openxr_ref:`xrWaitSwapchainImage` for both the color and depth swapchains to get the next 2D array image from them. We are still required to submit an :openxr_ref:`XrCompositionLayerProjectionView` structure for each view in the system, but in the :openxr_ref:`XrSwapchainSubImage` we can set the ``imageArrayIndex`` to specify which layer of the swapchain image we wish to associate with that view. So in the case of stereo rendering, it would be ``0`` for left and ``1`` for right eye views. We attach our 2D array image as a render target/color attachment for the pixel/fragment shader to write to.

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderCuboid2
	:end-before: XR_DOCS_TAG_END_RenderCuboid2
	:dedent: 8
	:emphasize-lines: 4 - 7, 9, 21

.. literalinclude:: ../Chapter6_1_Multiview/Chapter6/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderLayer1
	:end-before: XR_DOCS_TAG_END_RenderLayer2
	:dedent: 8

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

	First, you need to check that ``D3D12_FEATURE_DATA_D3D12_OPTIONS3::ViewInstancingTier`` doesn't equal ``D3D12_VIEW_INSTANCING_TIER_NOT_SUPPORTED``. Next, within the pipeline creation, set up the ``D3D12_VIEW_INSTANCING_DESC`` and a ``std::vector<D3D12_VIEW_INSTANCE_LOCATION>`` for pipeline creation. Here, we must use ``ID3D12Device2::CreatePipelineState()`` for ViewInstancing. Set the ``D3D12_VIEW_INSTANCE_LOCATION::RenderTargetArrayIndex`` to ``0`` for left and ``1`` for right eye views. This means that we don't need to write to ``SV_RenderTargetArrayIndex`` in the shader. We set ``D3D12_VIEW_INSTANCING_DESC::Flags`` to ``D3D12_VIEW_INSTANCING_FLAG_NONE``, though it's possible to set it to ``D3D12_VIEW_INSTANCING_FLAG_ENABLE_VIEW_INSTANCE_MASKING`` and use ``ID3D12CommandList2::SetViewInstanceMask()`` to control further which views will be rendered to.

	.. literalinclude:: ../Chapter6_1_Multiview/Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-at: if (pipelineCI.viewMask != 0) {
		:end-before: #else

	Modify the shader to use ``SV_ViewID`` and compile with shader model 6.1 to allow the vertex shader to use the new ``SV_ViewID`` System-Value Semantic. See `HLSL Semantics <https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics>`_ for its usage.

		Vertex Shader:

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_DX12_MV.hlsl
		:diff: ../Shaders/VertexShader.hlsl
		:language: hlsl

	Downloads:
		* :download:`dxc_shader.cmake <../Chapter6_1_Multiview/cmake/dxc_shader.cmake>`
		* :download:`d3dx12.h <../Chapter6_1_Multiview/Common/d3dx12.h>`

.. container:: opengl

	Enusre you have support for ``GL_OVR_multiview`` by checking the extensions and that you have loaded the ``glFramebufferTextureMultiviewOVR()`` function pointer, if you need to do so. You will use this to create a framebuffer that supports rendering to multiple layers.
	See this example from ARM's OpenGL ES SDK for Android `here <https://arm-software.github.io/opengl-es-sdk-for-android/multiview.html>`_, which works for OpenGL too.

	Modify the shader to use ``gl_ViewIndex_OVR`` and the GL_OVR_multiview GLSL extension. The line ``layout(num_views = 2) in`` specifies the number of views the vertex shader a will broadcast to.

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_GL_MV.glsl
		:diff: ../Shaders/VertexShader.glsl
		:language: glsl

.. container:: opengles

	Enusre you have support for ``GL_OVR_multiview`` by checking the extensions and that you have loaded the ``glFramebufferTextureMultiviewOVR()`` function pointer, if you need to do so. You will use this to create a framebuffer that supports rendering to multiple layers.
	See this example from ARM's OpenGL ES SDK for Android `here <https://arm-software.github.io/opengl-es-sdk-for-android/multiview.html>`_.

	Modify the shader to use ``gl_ViewIndex_OVR`` and the GL_OVR_multiview GLSL extension. The line ``layout(num_views = 2) in`` specifies the number of views the vertex shader a will broadcast to.

	.. literalinclude:: ../Chapter6_1_Multiview/ShadersMultiview/VertexShader_GL_MV.glsl
		:diff: ../Shaders/VertexShader.glsl
		:language: glsl

.. container:: vulkan

	First, add the ``VK_KHR_MULTIVIEW_EXTENSION_NAME`` or ``"VK_KHR_multiview"`` string to the device extensions list when creating the ``VkDevice``.

	At pipeline creation, chain via the next pointer a ``VkRenderPassMultiviewCreateInfo`` structure to the ``VkRenderPassCreateInfo`` structure when creating the ``VkRenderPass``. Note that there is similar functionality for ``VK_KHR_dynamic_rendering``. The viewMask specifies the number of views the rasterizer will broadcast to.

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

This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. The derived class is based on your graphics API selection. Include both the header and cpp files for both ``GraphicsAPI`` and ``GraphicsAPI...``. ``GraphicsAPI.h`` includes the headers and macros needed to set up your platform and graphics API. Below are code snippets that show how to set up the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros for your platform along with any relevant headers. In the first code block, there's also a reference to ``XR_TUTORIAL_USE_...`` which we set up the ``CMakeLists.txt`` . This tutorial demonstrates all five graphics APIs.

The code below is an example of how you might implement the inclusion and definition of the relevant graphics API header along with the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros.

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

When setting up the graphics API core objects, there are things that we need to know from OpenXR in order to create the objects correctly. These could include the version of the graphics API required, referencing a specific GPU, required instance and/or device extension etc. Below are code examples showing how to set up your graphics for OpenXR.

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
In the table below are the layer names and their associated libraries and .json files.

+------------------------------------+-----------------------------------------------+-------------------------------------+
| XR_APILAYER_LUNARG_api_dump        | ``XrApiLayer_api_dump.dll`` or ``.so``        | ``XrApiLayer_api_dump.json``        |
+------------------------------------+-----------------------------------------------+-------------------------------------+
| XR_APILAYER_LUNARG_core_validation | ``XrApiLayer_core_validation.dll`` or ``.so`` | ``XrApiLayer_core_validation.json`` |
+------------------------------------+-----------------------------------------------+-------------------------------------+

XR_APILAYER_LUNARG_api_dump simply logs extra/verbose information to ``std::cout`` describing in more detail what has happened during that API call. XR_APILAYER_LUNARG_core_validation acts similarly to VK_LAYER_KHRONOS_validation in Vulkan, where the layer intercepts the API call and performs validation to ensure conformance with the specification.

Other runtimes and hardware vendors may provide layers that are useful for debugging your XR system and/or application.

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

Calls to :openxr_ref:`xrEnumerateApiLayerProperties` should now return a pointer to an array of structs and the count of all API layers available to the application.

To select which API layers we want to use, there are two ways to do this:
 1. Add the ``XR_ENABLE_API_LAYERS=<layer_name>`` environment variable to your project or your system. Something like this: ``XR_ENABLE_API_LAYERS=XR_APILAYER_LUNARG_test1;XR_APILAYER_LUNARG_test2``.
 2. When creating the :openxr_ref:`XrInstance`, specify the requested API layers in the :openxr_ref:`XrInstanceCreateInfo` structure.

For more details, please see `API Layers README <https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/main/src/api_layers/README.md>`_ and see `OpenXR API Layers <https://registry.khronos.org/OpenXR/specs/1.0/loader.html#openxr-api-layers>`_.

*****************
6.4 Color Science
*****************

As OpenXR support both linear and sRGB color spaces for compositing. It is helpful to have a deeper knowledge of color science; especially if you are planning to use sRGB formats and have the OpenXR runtime/compositor do automatic conversions for you.

For more information on color spaces and gamma encoding, see J. Guy Davidson's `video presentation <https://www.youtube.com/watch?v=_zQ_uBAHA4A>`_ on the subject.

**************
6.5 Conclusion
**************

In this chapter, we discussed a few of the possible next steps in your OpenXR journey. Be sure to refer back to the `OpenXR Specification <https://registry.khronos.org/OpenXR/specs/1.0>`_ and look out for updates, both there and here, as the specification develops.

The text of the OpenXR Tutorial is by Andrew Richards and Roderick Kennedy of Simul Software Ltd. The design of the site is by Calland Creative Ltd. The site is overseen by the Khronos OpenXR Working Group.

