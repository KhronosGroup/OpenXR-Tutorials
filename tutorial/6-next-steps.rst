############
6 Next Steps
############

Multiview rendering

Vulkan supports rendering to both eye views with multiview, which simplifies the rendering code. `Vulkan Multiview <https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_multiview.html>`_.
OpenGL supports rendering to both eye views with multiview, which simplifies the rendering code. `OpenGL Multiview <https://registry.khronos.org/OpenGL/extensions/OVR/OVR_multiview.txt>`_.


Graphics API
------------

.. rubric::  GraphicsAPI

Note: ``GraphicsAPI`` is by no means production-ready code or reflective of good practice with specific APIs. It is there solely to provide working samples in this tutorial, and demonstrate some basic rendering and interaction with OpenXR.

This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. The derived class is based on your graphics API selection. Include both the header and cpp files for both ``GraphicsAPI`` and ``GraphicsAPI...``. ``GraphicsAPI.h`` includes the headers and macros needed to set up your platform and graphics API. Below are code snippets that show how to set up the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros for your platform along with any relevant headers. In the first code block, there's also reference to ``XR_TUTORIAL_USE_...`` which we set up the ``CMakeLists.txt`` . This tutorial demonstrates all five graphics APIs, you will only need to select one ``XR_USE_PLATFORM_...`` macro and one ``XR_USE_GRAPHICS_API_...`` macro.

The code below is an example of how you might implement the inclusion and definition the relevant graphics API header along with the ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros. This will already be set up in the ``GraphicsAPI.h`` file.

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: #include <HelperFunctions
	:end-at: .h>

.. container:: windows
	:name: windows-id-4

	.. container:: d3d11
		:name: d3d11-id-4

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif // _WIN32
			:emphasize-lines: 6, 8-10

	.. container:: d3d12
		:name: d3d12-id-4

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif // _WIN32
			:emphasize-lines: 6, 11-13

	.. container:: opengl
		:name: opengl-id-6

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif // _WIN32
			:emphasize-lines: 6, 14-16

	.. container:: vulkan
		:name: vulkan-id-9

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(_WIN32)
			:end-at: #endif // _WIN32
			:emphasize-lines: 6, 17-19

.. container:: linux
	:name: linux-id-4

	.. container:: opengl
		:name: opengl-id-7

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__linux__)
			:end-at: #endif // __linux__
			:emphasize-lines: 2-14, 16-18

	.. container:: vulkan
		:name: vulkan-id-10

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__linux__)
			:end-at: #endif // __linux__
			:emphasize-lines: 2-14, 19-21

.. container:: android
	:name: android-id-4

	.. container:: opengles
		:name: opengles-id-5

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__ANDROID__)
			:end-at: endif // __ANDROID__
			:emphasize-lines: 3, 5-7

	.. container:: vulkan
		:name: vulkan-id-11

		.. literalinclude:: ../Common/GraphicsAPI.h
			:language: cpp
			:start-at: #if defined(__ANDROID__)
			:end-at: endif // __ANDROID__
			:emphasize-lines: 3, 8-10

.. container:: d3d11
	:name: d3d11-id-5

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_D3D11)
		:end-at: #endif

.. container:: d3d12
	:name: d3d12-id-5

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_D3D12)
		:end-at: #endif

.. container:: opengl
	:name: opengl-id-8

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_OPENGL)
		:end-at: #endif

.. container:: opengles
	:name: opengles-id-6

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
		:end-at: #endif

.. container:: vulkan
	:name: vulkan-id-12

	.. literalinclude:: ../Common/GraphicsAPI.h
		:language: cpp
		:start-at: #if defined(XR_USE_GRAPHICS_API_VULKAN)
		:end-at: #endif

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: // OpenXR
	:end-at: #include <OpenXRHelper.h>


	

For more information on color spaces and gamma encoding, see Guy Davidson's video presentation `here <https://www.youtube.com/watch?v=_zQ_uBAHA4A>`_.