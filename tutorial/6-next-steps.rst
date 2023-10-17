############
6 Next Steps
############

***********************
6.1 Multiview rendering
***********************

D3D12 supports rendering to both eye views with ViewInstancing, which simplifies the rendering code. `D3D12 View Instancing <https://microsoft.github.io/DirectX-Specs/d3d/ViewInstancing.html>`_.

Vulkan supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.

OpenGL supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.


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