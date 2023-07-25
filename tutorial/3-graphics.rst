👾 3 Graphics
==========

Select your platform, as the instructions are different depending on your selection.

.. raw:: html
	:file: platforms.html

The goal of this chapter is to build an application that creates and clears color and depth buffers within the scope of OpenXR render loop and to demostrate its interaction with all the Graphics APIs.

Creating Swapchains
-------------------

As with rendering graphics to a 2D display, OpenXR has the concept of swapchains. It's series of images that are used to present the rendered graphics to display/window/view. There are usually 2 or 3 images in the swapchain to allow the platform to present them smoothly to the user in order to create illusion of motion within the image.

All graphics APIs have this concept of a swapchain with differing levels of transparency to the programmer, but for an OpenXR application we don't use the API specific swapchain, which can be tied closely the platform's windowing system. Instead, we use OpenXR's swapchain and the OpenXR compositor to present rendered graphics to the views. XR applications are unique in that often, but not always, have multiple views that need to be rendered to create the XR experience. Listed below are a few scenarios with differing view counts:

	* 1 view  - AR viewer on a mobile device.
	* 2 views - VR head mounted display.
	* 6 views - Multi-wall projection-based (CAVE-like) VR system.

Orthogonal to multiple views is the layering of multiple rendered images or camera feeds. You could, for a example, have a background that is a video pass-through of your enviroment, a stereo view of rendering graphics and quad overlay of a HUD or UI elements; all of of which could have different spatial orientations. This layering of views is handled by the XR compositor to composite correctly the layers for each view - that quad overlay might be behind the user, and thus shouldn't be rendered to the eye views.

Firstly, we will update the class to add the new methods and members.

.. code-block:: cpp

	class OpenXRTutorialChapter3 {
	public:
		// [...]
	
		void Run() {
			CreateInstance();
			CreateDebugMessenger();
	
			GetInstanceProperties();
			GetSystemID();
	
			GetViewConfigurationViews();
	
			CreateSession();
			CreateSwapchain();
	
			while (applicationRunning) {
				PollSystemEvents();
				PollEvents();
				if (sessionRunning) {
					// Draw Frame.
				}
			}
	
			DestroySwapchain();
			DestroySession();
	
			DestroyDebugMessenger();
			DestroyInstance();
		}
	
		private:
		// [...]
		
		std::vector<XrViewConfigurationView> viewConfigurationViews;
		// [...]
		
		struct SwapchainAndDepthImage {
			XrSwapchain swapchain{};
			int64_t swapchainFormat = 0;
			void *depthImage = nullptr;
	
			std::vector<void *> colorImageViews;
			void *depthImageView;
		};
		std::vector<SwapchainAndDepthImage> swapchainAndDepthImages;
	}

XrViewConfigurationView
^^^^^^^^^^^^^^^^^^^^^^^

The first thing we need to do is get all of the views available to our view configuration. It is worth just parsing the name of this type: ``XrViewConfigurationView``. I break the typename up as follow "XrViewConfiguration" - "View", where it relates to one view in the view configuration, which may contain multiple views. We call ``xrEnumerateViewConfigurationViews()`` twice, first to get the count of the views in the view configuration, and second to fill in the data to the ``std::vector<XrViewConfigurationView>``.

.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GetViewConfigurationViews
	:end-before: XR_DOCS_TAG_END_GetViewConfigurationViews
	:dedent: 4

xrEnumerateSwapchainFormats
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Due to way that OpenXR and its composite operate, there are certain preferred image formats that should be used by the swapchain. When calling ``xrEnumerateSwapchainFormats()``, the ``XrSession`` and alongwith the Graphics API will return an array of API-specific formats ordered in preference. ``xrEnumerateSwapchainFormats()`` takes a pointer to the first element in an array of ``int64_t`` values. The use of ``int64_t`` is a simple type cast from a ``DXGI_FORMAT``, ``GLenum`` or a ``VkFormat``. The runtime "should support ``R8G8B8A8`` and ``R8G8B8A8 sRGB`` formats if possible" (`OpenXR Specification 10.1. Swapchain Image Management <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#swapchain-image-management>`_).

Linear or sRGB color space? OpenXR's compositor performs all blend operations in a linear color space (i.e. the values have not been gamma encoded). Most gamma-encoding operations are algebraically non-linear, so you can't composite the values with simple addition or multiplication operations. If you wish to use an sRGB color format, you must use an API-specific sRGB color format such as ``DXGI_FORMAT_R8G8B8A8_UNORM_SRGB``, ``GL_SRGB8_ALPHA8`` or ``VK_FORMAT_R8G8B8A8_SRGB``. The OpenXR runtime will automatically do sRGB-to-linear color space conversions when reading the image. There are two issues with this: 

1. Runtime conversion of image data could be too slow and affect performance and comfort.
2. The conversion process may not use the same style of gamma encoding/decoding and there could be a loss in color accuracy.

If you'd like more information on color spaces and gamma in computer graphics, Guy Davidson from Creative Assembly has a fantastic video presentation from Meeting C++ 2021 on this topic `here <https://www.youtube.com/watch?v=_zQ_uBAHA4A>`_.


.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_EnumerateSwapchainFormats
	:end-before: XR_DOCS_TAG_END_EnumerateSwapchainFormats
	:dedent: 8

Next, we do some checks to confirm that the views in the view configuration are the same size and thus suitable for stereo rendering. With this check done, we can alias to views together when create our ``XrSwapchain``. We resize our ``SwapchainAndDepthImage`` structure and enter a for each loop to create all required resources.

xrCreateSwapchain
^^^^^^^^^^^^^^^^^

.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateSwapchain
	:end-before: XR_DOCS_TAG_END_CreateSwapchain
	:dedent: 12

Here, we fill out the ``XrSwapchainCreateInfo`` structure. The ``sampleCount``, ``width`` and ``height`` members can be assigned from the ``XrViewConfigurationView``. We set the ``createFlags`` to 0 as we require no contraints or functionality. We set the ``usageFlags`` to ``XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT`` requesting that the image are suitable to be read in a shader and to be used as a render output.

.. literalinclude:: ../build/openxr/include/openxr/openxr.h
	:language: cpp
	:start-at: // Flag bits for XrSwapchainUsageFlags
	:end-before: typedef XrFlags64 XrCompositionLayerFlags;

Below is a explanation of XrSwapchainUsageFlagBits match with the Graphics APIs.

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11

	+-------------------------------------------------+------------------------------------+
	| XrSwapchainUsageFlagBits                        | Corresponding D3D11 bind flag bits |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT         | D3D11_BIND_RENDER_TARGET           |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | D3D11_BIND_DEPTH_STENCIL           |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_UNORDERED_ACCESS_BIT         | D3D11_BIND_UNORDERED_ACCESS        |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_TRANSFER_SRC_BIT             | ignored                            |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT             | ignored                            |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_SAMPLED_BIT                  | D3D11_BIND_SHADER_RESOURCE         |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_MUTABLE_FORMAT_BIT           | ignored                            |
	+-------------------------------------------------+------------------------------------+
	| XR_SWAPCHAIN_USAGE_INPUT_ATTACHMENT_BIT_KHR     | ignored                            |
	+-------------------------------------------------+------------------------------------+

.. container:: d3d12
	:name: d3d12-id-1

	.. rubric:: DirectX 12

	+-------------------------------------------------+--------------------------------------------+
	| XrSwapchainUsageFlagBits                        | Corresponding D3D12 resource flag bits     |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT         | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET    |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL    |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_UNORDERED_ACCESS_BIT         | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_TRANSFER_SRC_BIT             | ignored                                    |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT             | ignored                                    |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_SAMPLED_BIT                  | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE   |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_MUTABLE_FORMAT_BIT           | ignored                                    |
	+-------------------------------------------------+--------------------------------------------+
	| XR_SWAPCHAIN_USAGE_INPUT_ATTACHMENT_BIT_KHR     | ignored                                    |
	+-------------------------------------------------+--------------------------------------------+

.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL

	All ``XrSwapchainUsageFlags`` are ignored as OpenGL can't specify usage of an image.

.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	All ``XrSwapchainUsageFlags`` are ignored as OpenGL ES can't specify usage of an image.

.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan

	+-------------------------------------------------+---------------------------------------------+
	| XrSwapchainUsageFlagBits                        | Corresponding Vulkan flag bit               |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT         | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT         |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_UNORDERED_ACCESS_BIT         | VK_IMAGE_USAGE_STORAGE_BIT                  |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_TRANSFER_SRC_BIT             | VK_IMAGE_USAGE_TRANSFER_SRC_BIT             |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT             | VK_IMAGE_USAGE_TRANSFER_DST_BIT             |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_SAMPLED_BIT                  | VK_IMAGE_USAGE_SAMPLED_BIT                  |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_MUTABLE_FORMAT_BIT           | VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT          |
	+-------------------------------------------------+---------------------------------------------+
	| XR_SWAPCHAIN_USAGE_INPUT_ATTACHMENT_BIT_KHR     | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT         |
	+-------------------------------------------------+---------------------------------------------+

Next, we set the values for ``faceCount``, ``arraySize`` and ``mipCount``. ``faceCount`` describes the number of faces in the image and is used for creating cubemap textures. ``arraySize`` describes the number of layers in an image. Here, we use ``1``, as we have separate swapchain per view/eye, but for a stereo view you could pass ``2`` and have an image 2D array, which is suitable for multiview rendering. ``mipCount`` describes the number of mips levels, useful when using the swapchain image as a sampled image. Finally, we can set the format. Here, we ask our ``GraphicsAPIs`` class to pick a suitable format for the swapachain for the enumerated format we acquired early. Here is the code for ``GraphicsAPI::SelectSwapchainFormat()``:

.. literalinclude:: ../Common/GraphicsAPI.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_SelectSwapchainFormat
	:end-before: XR_DOCS_TAG_END_GraphicsAPI_SelectSwapchainFormat

The function calls a pure virtual function called ``GraphicsAPI::GetSupportedSwapchainFormats()``, which each class implements. It returns an array of API-specific format us to use.

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11

	.. literalinclude:: ../Common/GraphicsAPI_D3D11.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_GetSupportedSwapchainFormats
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D11_GetSupportedSwapchainFormats

.. container:: d3d12
	:name: d3d12-id-1

	.. rubric:: DirectX 12

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_GetSupportedSwapchainFormats
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D12_GetSupportedSwapchainFormats

.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_GetSupportedSwapchainFormats
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_GetSupportedSwapchainFormats

.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES_GetSupportedSwapchainFormats
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES_GetSupportedSwapchainFormats

.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetSupportedSwapchainFormats
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetSupportedSwapchainFormats

We call ``xrCreateSwapchain()`` to create our ``XrSwapchain``, which, if successful, will return ``XR_SUCCESS`` and the ``XrSwapchain`` will be non-null. We copy our swapchain format to our ``SwapchainAndDepthImage::swapchainFormat`` for later usage.

xrEnumerateSwapchainImages
^^^^^^^^^^^^^^^^^^^^^^^^^^

Now that we have created the ``XrSwapchain``, we need to get access to the all images in the swapchain. We first call ``xrEnumerateSwapchainImages()`` to get the count of the images in the ``XrSwapchain``. Next, we set up an array of structures to store the images from the ``XrSwapchain``. In this tutorial, this array of structures, which stores the swapchains images, are stored in the ``GraphicsAPI_...`` class. We do this, because OpenXR will return to the application an array of structures that contain the API-specific handles to the swapchain images. ``GraphicsAPI::AllocateSwapchainImageData()`` is a virtual method implemented by each graphics API, which resizes an API-specific ``std::vector<XrSwapchainImage...KHR>`` and returns a pointer to the first element in that array casting it to a ``XrSwapchainImageBaseHeader *``.

.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_EnumerateSwapchainImages
	:end-before: XR_DOCS_TAG_END_EnumerateSwapchainImages
	:dedent: 12
	:emphasize-lines: 3

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11

	.. literalinclude:: ../Common/GraphicsAPI_D3D11.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_AllocateSwapchainImageData
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D11_AllocateSwapchainImageData

	``swapchainImages`` is of type ``std::vector<XrSwapchainImageD3D11KHR>``.
		
	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrSwapchainImageD3D11KHR {
		:end-at: } XrSwapchainImageD3D11KHR;

	The structure contains a ``ID3D11Texture2D *`` member that is the handle to one of the images in the swapchain.

.. container:: d3d12
	:name: d3d12-id-1

	.. rubric:: DirectX 12

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_AllocateSwapchainImageData
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D12_AllocateSwapchainImageData
	
	``swapchainImages`` is of type ``std::vector<XrSwapchainImageD3D12KHR>``.

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrSwapchainImageD3D12KHR {
		:end-at: } XrSwapchainImageD3D12KHR;

	The structure contains a ``ID3D12Resource *`` member that is the handle to one of the images in the swapchain.

.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_AllocateSwapchainImageData
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_AllocateSwapchainImageData

	``swapchainImages`` is of type ``std::vector<XrSwapchainImageOpenGLKHR>``.

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrSwapchainImageOpenGLKHR {
		:end-at: } XrSwapchainImageOpenGLKHR;

	The structure contains a ``uint32_t`` member that is the handle to one of the images in the swapchain.

.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES_AllocateSwapchainImageData
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES_AllocateSwapchainImageData

	``swapchainImages`` is of type ``std::vector<XrSwapchainImageOpenGLESKHR>``.

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrSwapchainImageOpenGLESKHR {
		:end-at: } XrSwapchainImageOpenGLESKHR;

	The structure contains a ``uint32_t`` member that is the handle to one of the images in the swapchain.

.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_AllocateSwapchainImageData
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_AllocateSwapchainImageData

	``swapchainImages`` is of type ``std::vector<XrSwapchainImageVulkanKHR>``.

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrSwapchainImageVulkanKHR {
		:end-at: } XrSwapchainImageVulkanKHR;

	The structure contains a ``VkImage`` member that is the handle to one of the images in the swapchain.

Create Depth Image And Image Views
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Next, we create a depth image so that we can correctly render 3D prespective graphics to the view. In this tutorial, we have a ``GraphicsAPI::ImageCreateInfo`` structure and virtual method ``GraphicsAPI::CreateImage()`` that creates the API-specific objects. 

.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateDepthImage
	:end-before: XR_DOCS_TAG_END_CreateDepthImage
	:dedent: 12
	:emphasize-lines: 9

Each graphics API overrides the virtual function ``GraphicsAPI::GetDepthFormat()``, which return a API-specific image format of type ``D32_F``.

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11

	.. literalinclude:: ../Common/GraphicsAPI_D3D11.h
		:language: cpp
		:start-at: virtual int64_t GetDepthFormat() override
		:end-at: }

.. container:: d3d12
	:name: d3d12-id-1

	.. rubric:: DirectX 12

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.h
		:language: cpp
		:start-at: virtual int64_t GetDepthFormat() override
		:end-at: }

.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.h
		:language: cpp
		:start-at: virtual int64_t GetDepthFormat() override
		:end-at: }

.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.h
		:language: cpp
		:start-at: virtual int64_t GetDepthFormat() override
		:end-at: }

.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.h
		:language: cpp
		:start-at: virtual int64_t GetDepthFormat() override
		:end-at: }

We store our newly created depth image in ``SwapchainAndDepthImage::depthImage`` for later usage when rendering. 

Now, we create the image views: one per image in the ``XrSwapchain`` and an additional one for the depth image. Again in this tutorial, we have a ``GraphicsAPI::ImageViewCreateInfo`` structure and virtual method ``GraphicsAPI::CreateImageView()`` that creates the API-specific objects. 

.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateImageViews
	:end-before: XR_DOCS_TAG_END_CreateImageViews
	:dedent: 12
	:emphasize-lines: 3, 6, 16, 19

Each graphics API overrides the virtual function ``GraphicsAPI::GetSwapchainImage()``, which return a API-specific handle to the image, which is cast to a ``void *``.

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11

	.. literalinclude:: ../Common/GraphicsAPI_D3D11.h
		:language: cpp
		:start-at: virtual void* GetSwapchainImage(uint32_t index) override
		:end-at: }

.. container:: d3d12
	:name: d3d12-id-1

	.. rubric:: DirectX 12

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.h
		:language: cpp
		:start-at: virtual void* GetSwapchainImage(uint32_t index) override
		:end-at: }

.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.h
		:language: cpp
		:start-at: virtual void* GetSwapchainImage(uint32_t index) override
		:end-at: }

.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.h
		:language: cpp
		:start-at: virtual void* GetSwapchainImage(uint32_t index) override
		:end-at: }

.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.h
		:language: cpp
		:start-at: virtual void* GetSwapchainImage(uint32_t index) override
		:end-at: }

For the color image views, we use the previously stored color image format, that we used when creating the swapchain, and for the depth image view, we use the previously created depth image and the same depth format from the graphics API.
We store our newly created color image views for the swapchain in ``SwapchainAndDepthImage::colorImageViews`` and the depth image view in ``SwapchainAndDepthImage::depthImageView`` for later usage when rendering. 

xrDestroySwapchain
^^^^^^^^^^^^^^^^^^

When the main render loop has finished and the application is shutting down, we need to destroy our created ``XrSwapchain``. This is done by calling ``xrDestroySwapchain()`` with the ``XrSwapchain`` and it will return ``XR_SUCCESS`` if successful. At the same time, we destroy the associated depth image and all of the views that the graphics API created. In this tutorial, we use ``GraphicsAPI::DestroyImage()`` and ``GraphicsAPI::DestroyImageView()`` to destroy those objects.

.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroySwapchain
	:end-before: XR_DOCS_TAG_END_DestroySwapchain
	:dedent: 4

We now have ``XrSwapchain`` s and a depth images, ready for rendering. Next, we setup the render loop for OpenXR!

Building a RenderLoop
---------------------

xrEnumerateEnvironmentBlendModes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GetEnvironmentBlendModes
	:end-before: XR_DOCS_TAG_END_GetEnvironmentBlendModes
	:dedent: 4

xrCreateReferenceSpace
^^^^^^^^^^^^^^^^^^^^^^
.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateReferenceSpace
	:end-before: XR_DOCS_TAG_END_CreateReferenceSpace
	:dedent: 4

RenderFrame
^^^^^^^^^^^
.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderFrame
	:end-before: XR_DOCS_TAG_END_RenderFrame
	:dedent: 4

RenderLayer
^^^^^^^^^^^
.. literalinclude:: ../Chapter3/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_RenderLayer
	:end-before: XR_DOCS_TAG_END_RenderLayer
	:dedent: 4
