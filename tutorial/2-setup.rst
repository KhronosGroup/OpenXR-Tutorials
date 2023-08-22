##########
Setup
##########

Select your platform, as the instructions are different depending on your selection.

.. raw:: html
	:file: platforms.html

With your project setup and your application building and running, we can start to use OpenXR to create our application.
The goal of this chapter is to create an ``XrInstance`` and an ``XrSession``, and setup the OpenXR event loop. This OpenXR code is needed to setup the core functionality of an OpenXR application to have that application interact with the OpenXR runtime and your graphics API correctly.

****************************************
2.1 Creating an XrInstance / xrGetSystem
****************************************

We will continue to use the ``OpenXRTutorial`` class in ``Chapter2/main.cpp`` that we created in :ref:`Chapter 1.4 <1.4 Project Setup>`.

Here, we will add the following highlighted text to the ``OpenXRTutorial`` class:

.. code-block:: cpp
	:emphasize-lines: 10-17 , 20-50
	
	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type apiType) 
		{
		}
		~OpenXRTutorial() = default;
	
		void Run()
		{
			CreateInstance();
			CreateDebugMessenger();

			GetInstanceProperties();
			GetSystemID();

			DestroyDebugMessenger();
			DestroyInstance();
		}

	private:
		void CreateInstance() 
		{
		}
		void DestroyInstance()
		{
		}
		void CreateDebugMessenger()
		{
		}
		void DestroyDebugMessenger()
		{
		}
		void GetInstanceProperties()
		{
		}
		void GetSystemID()
		{
		}
	
	private:
		XrInstance m_xrInstance = {};
		std::vector<const char *> m_activeAPILayers = {};
		std::vector<const char *> m_activeInstanceExtensions = {};
		std::vector<std::string> m_apiLayers = {};
		std::vector<std::string> m_instanceExtensions = {};

		XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

		XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemId m_systemID = {};
	};

First, we updated ``OpenXRTutorial::Run()`` to call the new methods ``CreateInstance()``, ``GetInstanceProperties()``, ``GetSystemID()``and ``DestroyInstance()`` in that order. Finally, we added those methods and the following members to the class within thier separate private sections.

2.1.1 XrInstance
================

The ``XrInstance`` is the foundational object that we need to create first. The ``XrInstance`` encompasses the application setup state, OpenXR API version and any layers and extensions. So inside the ``CreateInstance()`` method, we will first add at the code for the ``XrApplicationInfo``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrApplicationInfo
	:end-before: XR_DOCS_TAG_END_XrApplicationInfo
	:dedent: 8

This structure allows you specify both the name and the version for your application and engine. These members are solely for your use as the application developer. The main member here is the ``XrApplicationInfo::apiVersion``. Here we use the ``XR_CURRENT_API_VERSION`` macro to specific the OpenXR version that we want to run. Also note here the use of ``strncpy()`` to set the applicationName and engineName. If you look at ``XrApplicationInfo::applicationName`` and ``XrApplicationInfo::engineName`` members, they are of type ``char[]``, hence you must copy your string into that ``char[]`` and you must also by aware of the allowable length.

.. _instanceextensions:

Similar to Vulkan, OpenXR allows applications to extend functionality past what is provided by the core specification. The functionality could be hardware/vendor specific. Most vital of course is which Graphics API to use with OpenXR. OpenXR supports D3D11, D3D12, Vulkan, OpenGL and OpenGL ES. Due the extensible nature of specification, it allows newer Graphics APIs and hardware functionality to be added with ease. Following on from the previous code in the ``CreateInstance()`` method, add the following:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_instanceExtensions
	:end-before: XR_DOCS_TAG_END_instanceExtensions
	:dedent: 12

Here, we store in a ``std::vector<std::string>`` the extension names that we would like to use. ``XR_EXT_DEBUG_UTILS_EXTENSION_NAME`` is a macro of a string defined in ``openxr.h``. The XR_EXT_debug_utils is extension that checks the validity of calls made to OpenXR, and can use a call back function to handle any raised errors. We will explore this extension more in :ref:`Chapter 2.1<2.1.2 XR_EXT_debug_utils>`. Depending on which ``XR_USE_GRAPHICS_API_...`` macro that you have defined, this code will add the relevant extension.

Not all API layers and extensions are available to use, so we much check which ones can use. We will use ``xrEnumerateApiLayerProperties()`` and ``xrEnumerateInstanceExtensionProperties()`` to check which ones the runtime can provide. We will do this by adding the following code to the ``CreateInstance()`` method.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_find_apiLayer_extension
	:end-before: XR_DOCS_TAG_END_find_apiLayer_extension
	:dedent: 8

These functions are called twice. The first time is to get the count of the API layers or extensions and the second is to fill out the array of structures. Before the second call, we need set ``XrApiLayerProperties::type`` or ``XrExtensionProperties::type`` to the correct value, so that the second call can correctly fill out the data. After we have enumerated the API layers and extensions, we use a nested loop to check to see whether an API layers or extensions is available and add it to the ``m_activeAPILayers`` and/or ``m_activeInstanceExtensions`` respectively. 

Note the ``m_activeAPILayers`` and ``m_activeInstanceExtensions`` are of type ``std::vector<const char *>``. This will help us when fill out the next structure ``XrInstanceCreateInfo``.

Now that we have assembled all of the information need we can go ahead and fill out the ``XrInstanceCreateInfo`` structure. Add the following code to the ``CreateInstance()`` method.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrInstanceCreateInfo
	:end-before: XR_DOCS_TAG_END_XrInstanceCreateInfo
	:dedent: 8

This section is fairly simple, as we have used the previously collected data and assigned it to the members in the ``XrInstanceCreateInfo`` structure. Then, we called ``xrCreateInstance()`` where we took pointers to the ``XrInstanceCreateInfo`` and ``XrInstance`` objects. When the function is called, if successful, it will return a value of ``XR_SUCCESS``, and ``XrInstance`` will be non-null.

At the end of the application, we should destroy the ``XrInstance``. This is simple done with the function ``xrDestroyInstance()``. Add the following code to the ``DestroyInstance()`` method:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: void DestroyInstance()
	:end-before: }
	:dedent: 8

Whilst we have an ``XrInstance``, let's check its properties. Add the following code to the ``GetInstanceProperties()`` method:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GetInstanceProperties
	:end-before: XR_DOCS_TAG_END_GetInstanceProperties
	:dedent: 8

Here, we have initialized the ``XrInstanceProperties`` structure with the correct ``XrStructureType`` and passed it along with the ``XrInstance`` to the ``xrGetInstanceProperties()`` function. This function will fill out the rest of that structure for us to use. Next, we have loggod to stdout the runtime's name, and with the use of the ``XR_VERSION_MAJOR``, ``XR_VERSION_MINOR`` and ``XR_VERSION_PATCH`` macros, we have parsed and logged the runtime version.

2.1.2 XR_EXT_debug_utils
========================

XR_EXT_debug_utils is an instance extension for OpenXR, which allows the application to get more information on any errors or warnings etc. raised by the runtime. You can specify which message severities and types will checked. If a debug message raised, it is passed to the callback function, which can optionally use the user data pointer provided in the ``XrDebugUtilsMessengerCreateInfoEXT`` structure.

Message Severities: 
 * Verbose: Output all diagnostic messages.
 * Info: Output information messages helpful in debugging.
 * Warning: Output messages that could suggest an application bug and that need reviewing.
 * Error: Output messages from errors that may cause undefined behavior and/or crashes.
 
Message Types:
 * General: An event type for general information.
 * Validation: An event type that may indicate invalid usage of OpenXR.
 * Performance: An event type that may indicate non-optimal usage of OpenXR.
 * Conformance: An event type that indicating a non-conformant OpenXR result from the runtime.

`OpenXR Specification 12.26.3. Debug Message Categorization <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#debug-message-categorization>`_. 

Copy the following code into ``CreateDebugMessenger()`` and ``DestroyDebugMessenger()`` respectively:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateDebugMessenger
	:end-before: XR_DOCS_TAG_END_CreateDebugMessenger
	:dedent: 8

In the code above, we first check that ``XR_EXT_DEBUG_UTILS_EXTENSION_NAME`` or ``"XR_EXT_debug_utils"`` is in ``activeInstanceExtensions``, which we used to create the ``XrInstance``. Next, we call the ``CreateOpenXRDebugUtilsMessenger()`` function. 

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroyDebugMessenger
	:end-before: XR_DOCS_TAG_END_DestroyDebugMessenger
	:dedent: 8

At the end of the program, we should destroy the ``XrDebugUtilsMessengerEXT`` by calling ``DestroyOpenXRDebugUtilsMessenger``. 

.. literalinclude:: ../Common/OpenXRDebugUtils.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Create_DestroyDebugMessenger
	:end-before: XR_DOCS_TAG_END_Create_DestroyDebugMessenger

*The above code is an excerpt from Common/OpenXRDebugUtils.cpp*

In ``CreateOpenXRDebugUtilsMessenger()``, we use a ``XrDebugUtilsMessengerCreateInfoEXT`` structure to specify which message severities and types will checked. Next, we set the callback function that we want to use, and it must match the ``PFN_xrDebugUtilsMessengerCallbackEXT`` signature. Optionally, you can set a ``userData`` pointer, perhaps to a class, but here we have set it to ``nullptr`` in this example.

XR_EXT_debug_utils is an extension and as such its functions are not loaded by default by the OpenXR loader. Therefore, we need to get the address of the function through the use of ``xrGetInstanceProcAddr()``. We pass the ``XrInstance`` and a string of the function we want to get, along with a pointer to a function pointer variable. We need to cast that pointer to a function pointer variable to a ``PFN_xrVoidFunction*`` type. Once we have the ``xrCreateDebugUtilsMessengerEXT()`` function, we call it by passing the ``XrInstance``, a pointer to our ``XrDebugUtilsMessengerCreateInfoEXT`` structure and a pointer to our ``XrDebugUtilsMessengerEXT``. If all is successful, we have set up the DebugUtilsMessenger callback.

In ``DestroyOpenXRDebugUtilsMessenger()``, the ``xrDestroyDebugUtilsMessengerEXT()`` function also needs to be loaded through the use of ``xrGetInstanceProcAddr()``. Once loaded, we can call the function and pass the ``XrDebugUtilsMessengerEXT`` and thus destroying it.

.. literalinclude:: ../Common/OpenXRDebugUtils.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_OpenXRMessageCallbackFunction
	:end-before: XR_DOCS_TAG_END_OpenXRMessageCallbackFunction

*The above code is an excerpt from Common/OpenXRDebugUtils.cpp*

Above is an example of a OpenXR DebugUtilsMessenger Callback function. This function can be completely customised to your liking, but here we simply convert the message's severity and type to strings, and create a string to log to stdout. We also add a ``DEBUG_BREAK`` if the severity is an error. Just one thing to note: Applications should always return ``XR_FALSE`` from this function.

2.1.3 XrSystemId
================

The next object that we want to get is the ``XrSystemId``. OpenXR 'separates the concept of physical systems of XR devices from the logical objects that applications interact with directly. A system represents a collection of related devices in the runtime, often made up of several individual hardware components working together to enable XR experiences'. 
`OpenXR Specification 5. System <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#system>`_. 

So, a ``XrSystemId`` could represent VR headset and a pair of controllers, or perhaps mobile device with video pass-through for AR. So we need to decide what type of ``XrFormFactor`` we are wanting to use, as some runtimes support multiple form factors. Here, we are selecting ``XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY``, which is initialised in the class, for a Meta Quest or Pico Neo. OpenXR currently offers two option for the ``XrFormFactor``.

.. literalinclude:: ../build/openxr/include/openxr/openxr.h
	:language: cpp
	:start-at: typedef enum XrFormFactor {
	:end-at: } XrFormFactor;

*The above code is an excerpt from openxr/openxr.h*

Add the following code to the ``GetSystemID()`` method:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GetSystemID
	:end-before: XR_DOCS_TAG_END_GetSystemID
	:dedent: 8

Here, we have filled out the ``XrSystemGetInfo`` structure with the desired ``XrFormFactor`` and pass it as a pointer along with the ``XrInstance`` and a pointer to the ``XrSystemId`` to the ``xrGetSystem()`` function. When the function is called, if successful, ``XrSystemId`` will be non-null.

With the above code, we have also got the system's properties. We partially filled out a ``XrSystemProperties`` structure and passed it as a pointer along with the ``XrInstance`` and the ``XrSystemId`` to the ``xrGetSystemProperties()`` function. This function will fill out the rest of the ``XrSystemProperties`` structure; detailing the vendor's ID, system's name and the system's graphics and tracking properties.

.. literalinclude:: ../build/openxr/include/openxr/openxr.h
	:language: cpp
	:start-at: typedef struct XrSystemGraphicsProperties {
	:end-at: } XrSystemProperties;

*The above code is an excerpt from openxr/openxr.h*

You can now run the application to check that you have a valid ``XrInstnace`` and ``XrSystemId``.

*************************
2.2 Creating an XrSession
*************************

The next major component of OpenXR that needs to be created in an ``XrSession``. An ``XrSession`` encapulates the state of application from the perspective of OpenXR. When an ``XrSession`` is created, it starts in the ``XR_SESSION_STATE_IDLE``. It is upto the runtime to provide any updates to the ``XrSessionState`` and for the application to query them and react to them. We will explore this in :ref:`Chapter 2.3<2.3 Polling the Event Loop>`.

For now, we are just going to create an ``XrSession``. At this point, you'll need to select which Graphics API you wish to use. Only one Graphics API can be used with an ``XrSession``. This tutorial demonstrates how to use D3D11, D3D12, OpenGL, OpenGL ES and Vulkan in conjunction with OpenXR for the purpose of rendering graphics to the provided views. Ultimately, you will most likely be bringing your own rendering solution to this tutorial, therefore the code examples provided for the Graphics APIs are `placeholders` for you own code base; demonstrating in this sub-chapter what objects are needed from your Graphics API in order to create an ``XrSession``. This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. There are both compile and runtime checks to select the requested Graphics API, and we construct an appropriate derived classes through the use of ``std::unique_ptr<>``. 

Update the constructor of the ``OpenXRTutorial`` class, the ``OpenXRTutorial::Run()`` method and also add in the definitions of the new methods and the members to their separate private sections. All the new code is highlighted code below.

.. code-block:: cpp
	:emphasize-lines: 4-9, 19-20, 51-56, 70-73

	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type api)
			: apiType(api) {
			if(!CheckGraphicsAPI_TypeIsValidForPlatform(apiType)) {
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
		
			CreateSession();
			DestroySession();
		
			DestroyDebugMessenger();
			DestroyInstance();
		}

	private:
		void CreateInstance()
		{
			// [...]
		}
		void DestroyInstance()
		{
			// [...]
		}
		void GetInstanceProperties()
		{
			// [...]
		}
		void GetSystemID()
		{
			// [...]
		}
		void CreateDebugMessenger()
		{
			// [...]
		}
		void DestroyDebugMessenger()
		{
			// [...]
		}
		void CreateSession()
		{
		}
		void DestroySession()
		{
		}

	private:
		XrInstance m_xrInstance = {};
		std::vector<const char *> m_activeAPILayers = {};
		std::vector<const char *> m_activeInstanceExtensions = {};
		std::vector<std::string> m_apiLayers = {};
		std::vector<std::string> m_instanceExtensions = {};

		XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

		XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemId m_systemID = {};

		GraphicsAPI_Type m_apiType = UNKNOWN;
		std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;

		XrSession m_session = {};
	};

2.2.1 XrSession
===============

Copy the following code into the ``CreateSession()`` method.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateSession1
	:end-before: XR_DOCS_TAG_END_CreateSession1
	:dedent: 8

.. container:: d3d11
	:name: d3d11-id-0

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_D3D11>
		:end-at: );
		:dedent: 12

.. container:: d3d12
	:name: d3d12-id-0

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_D3D12>
		:end-at: );
		:dedent: 12

.. container:: opengl
	:name: opengl-id-0

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL>
		:end-at: );
		:dedent: 12

.. container:: opengles
	:name: opengles-id-0

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL_ES>
		:end-at: );
		:dedent: 12

.. container:: vulkan
	:name: vulkan-id-0

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_Vulkan>
		:end-at: );
		:dedent: 12

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateSession2
	:end-before: XR_DOCS_TAG_END_CreateSession2
	:dedent: 8

For the ``DestroySession()`` method, add the following code:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroySession
	:end-before: XR_DOCS_TAG_END_DestroySession
	:dedent: 8

Above is the code for creating and destroying an ``XrSession``. ``xrDestroySession()`` will destroy the ``XrSession`` when we are finished and shutting down the application. ``xrCreateSession()`` takes the ``XrInstance``, ``XrSessionCreateInfo`` and ``XrSession`` return object. If the function call was successful, ``xrCreateSession()`` will return ``XR_SUCCESS`` and ``XrSession`` will be non-null. 

The ``XrSessionCreateInfo`` structure is deceptively simple. ``XrSessionCreateInfo::createFlags`` and ``XrSessionCreateInfo::systemId`` are easily filled in, but we need to specify which Graphics API we wish to use. This is achieved via the use of the ``XrSessionCreateInfo::next`` void pointer. Following Vulkan API's style of extensibility, structures for creating objects can be extended to enable extra functionality. In our case, the extension is required and thus ``XrSessionCreateInfo::next`` can not be a nullptr. That pointer must point to 'exactly one graphics API binding structure (a structure whose name begins with "XrGraphicsBinding")' (`XrSessionCreateInfo(3) Manual Page <https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrSessionCreateInfo.html>`_). We get a pointer to the correct *Graphics Binding* structure by calling ``GraphicsAPI::GetGraphicsBinding();``.

2.2.2 GraphicsAPI
=================

Below are code excerpts from ``GraphicsAPI`` and ``openxr_platform.h`` based on your selected graphics APIs, and you don't need to copy this code into project. The excerpts demonstrate the interaction of OpenXR with your selected graphics API.

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11
	
	.. literalinclude:: ../Common/GraphicsAPI_D3D11.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D11

	*The above code is an excerpt from Common/GraphicsAPI_D3D11.cpp*

	Above is the minimum code needed to create a suitable ``ID3D11Device *``. First, we get the function pointer for ``xrGetD3D11GraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsD3D11KHR`` structure.
	
	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsD3D11KHR {
		:end-at: } XrGraphicsRequirementsD3D11KHR;

	*The above code is an excerpt from openxr/openxr_platform.h*

	From this structure, we used the ``adapterLuid`` to find the appropriate ``IDXGIAdapter *``. We created a ``IDXGIFactory1 *`` and then called ``IDXGIFactory1::EnumAdapters()`` and ``IDXGIAdapter::GetDesc()`` to get the ``DXGI_ADAPTER_DESC``, so that we could compare the ``adapterLuid`` values.

	Finally, we called ``D3D11CreateDevice`` with the found adapter and the ``minFeatureLevel`` from ``XrGraphicsRequirementsD3D11KHR``, if successful, the function returned ``S_OK`` and ``ID3D11Device *`` is non-null.

	We also create ``ID3D11Debug`` and ``ID3D11InfoQueue`` for debugging.

	.. literalinclude:: ../Common/GraphicsAPI_D3D11.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D11_GetGraphicsBinding

	*The above code is an excerpt from Common/GraphicsAPI_D3D11.cpp*

	Here, we simply fill out the ``XrGraphicsBindingD3D11KHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``.

.. container:: d3d12
	:name: d3d12-id-1
	
	.. rubric:: DirectX 12

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D12

	*The above code is an excerpt from Common/GraphicsAPI_D3D12.cpp*

	Above is the minimum code needed to create a suitable ``ID3D12Device *``  and ``ID3D12CommandQueue *``. First, we get the function pointer for ``xrGetD3D12GraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsD3D12KHR`` structure.

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsD3D12KHR {
		:end-at: } XrGraphicsRequirementsD3D12KHR;

	*The above code is an excerpt from openxr/openxr_platform.h*

	From this structure, we used the ``adapterLuid`` to find the appropriate ``IDXGIAdapter1 *``. We created a ``IDXGIFactory4 *`` and then called ``IDXGIFactory4::EnumAdapters1()`` and ``IDXGIAdapter1::GetDesc()`` to get the ``DXGI_ADAPTER_DESC``, so that we could compare the ``adapterLuid`` values.

	Finally, we called ``D3D12CreateDevice`` with the found adapter and the ``minFeatureLevel`` from ``XrGraphicsRequirementsD3D12KHR``, if successful, the function returned ``S_OK`` and ``ID3D12Device *`` is non-null. Next, we created a simple a ``ID3D12CommandQueue *`` of type ``D3D12_COMMAND_LIST_TYPE_DIRECT``.

	We also queried the maximum number of descriptors and set up ``ID3D12DescriptorHeap *`` s for use in rendering. There's also commented out code to enable D3D12 debugging and GPU Based Validation.

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D12_GetGraphicsBinding
	
	*The above code is an excerpt from Common/GraphicsAPI_D3D12.cpp*

	Here, we simply fill out the ``XrGraphicsBindingD3D12KHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``.
	
.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL
	
	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL

	*The above code is an excerpt from Common/GraphicsAPI_OpenGL.cpp*

	Above is the minimum code needed to create a suitable OpenGL context. First, we get the function pointer for ``xrGetOpenGLGraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsOpenGLKHR`` structure. 

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsOpenGLKHR {
		:end-at: } XrGraphicsRequirementsOpenGLKHR;

	*The above code is an excerpt from openxr/openxr_platform.h*
	
	In this tutorial, we are using the 'gfxwrapper' for the OpenGL API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. Originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ and `OpenGL ES 3.1 <https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.withchanges.pdf>`_ specifications.

	Here, we called ``ksGpuWindow_Create()`` and passed the required parameters to setup the OpenGL context. Next, we queried the OpenGL version with ``glGetIntegerv()`` with ``GL_MAJOR_VERSION`` and ``GL_MINOR_VERSION``. With these values, we constructed an ``XrVersion`` value to compare with ``XrGraphicsRequirementsOpenGLKHR::minApiVersionSupported``.

	We also setup ``glDebugMessageCallback`` to help with debugging.

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_GetGraphicsBinding
	
	*The above code is an excerpt from Common/GraphicsAPI_OpenGL.cpp*

	Because OpenGL is very closely integrated with the platform's windowing system. We have different ``XrGraphicsBindingOpenGL[...]KHR`` structures - one per platform. At present, there are four: ``XrGraphicsBindingOpenGLXcbKHR``, ``XrGraphicsBindingOpenGLXlibKHR``, ``XrGraphicsBindingOpenGLWaylandKHR`` and ``XrGraphicsBindingOpenGLWin32KHR``. Depending on the platform, this function will fill out the relevant structure and return a pointer to that class member, which will be assigned to ``XrSessionCreateInfo::next``.
	
.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES

	*The above code is an excerpt from Common/GraphicsAPI_OpenGL_ES.cpp*

	Above is the minimum code needed to create a suitable OpenGL ES context. First, we get the function pointer for ``xrGetOpenGLESGraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsOpenGLESKHR`` structure. 

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsOpenGLESKHR {
		:end-at: } XrGraphicsRequirementsOpenGLESKHR;

	*The above code is an excerpt from openxr/openxr_platform.h*

	In this tutorial, we are using the 'gfxwrapper' for the OpenGL ES API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. Originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ and `OpenGL ES 3.1 <https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.withchanges.pdf>`_ specifications.

	Here, we called ``ksGpuWindow_Create()`` and passed the required parameters to setup the OpenGL ES context. Next, we query the OpenGL ES version with ``glGetIntegerv()`` with ``GL_MAJOR_VERSION`` and ``GL_MINOR_VERSION``. With these values, we constructed an ``XrVersion`` value to compare with ``XrGraphicsRequirementsOpenGLESKHR::minApiVersionSupported``.

	We also setup ``glDebugMessageCallback`` to help with debugging.

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES_GetGraphicsBinding

	*The above code is an excerpt from Common/GraphicsAPI_OpenGL_ES.cpp*

	Here, we simply fill out the ``XrGraphicsBindingOpenGLESAndroidKHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``. Note: This ``XrGraphicsBinding...`` structure differs from the others as it specifically tailored to the Android platform.
	
.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan
	
	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan

	*The above code is an excerpt from Common/GraphicsAPI_Vulkan.cpp*

	Above is the minimum code needed to create a suitable Vulkan Instance and Device. First, we called a helper method that loads in the pointers for the following functions:

		* ``xrGetVulkanGraphicsRequirementsKHR``: Used to fill out an ``XrGraphicsRequirementsVulkanKHR`` structure containing the minimum and maximum supported API version.
		* ``xrGetVulkanInstanceExtensionsKHR``: To retrieve a list of required ``VkInstance`` extensions.
		* ``xrGetVulkanDeviceExtensionsKHR``: To retrieve a list of required ``VkDevice`` extensions.
		* ``xrGetVulkanGraphicsDeviceKHR``:  To get the ``VkPhysicalDevice`` requested by OpenXR.

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_LoadPFN_XrFunctions
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_LoadPFN_XrFunctions
	
	*The above code is an excerpt from Common/GraphicsAPI_Vulkan.cpp*

	We called ``xrGetVulkanGraphicsRequirementsKHR()`` and fill out the ``XrGraphicsRequirementsVulkanKHR`` structure. 
	
	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsVulkanKHR {
		:end-at: } XrGraphicsRequirementsVulkanKHR;
	
	*The above code is an excerpt from openxr/openxr_platform.h*

	Then, we filled out a ``VkApplicationInfo`` where we assigned ``VkApplicationInfo::apiVersion`` a value using the retured value in ``XrGraphicsRequirementsVulkanKHR::minApiVersionSupported``. We enumerated the Instance extensions and filled out an array of structures. We used nested for-loops to find all the requested extensions and pushed them back into a ``std::vector<const char *>`` called ``activeInstanceExtensions``.

	``GetInstanceExtensionsForOpenXR()`` is another helper method.

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR
	
	*The above code is an excerpt from Common/GraphicsAPI_Vulkan.cpp*

	``xrGetVulkanInstanceExtensionsKHR()`` is called twice, first to get the size of the ``char`` buffer and the second to fill in the data. We used ``std::stringstream`` and ``std::getline()`` with a deliminator of ``' '`` to break up the string and copied that substring to an element in a ``std::vector<std::string>``, which we used when setting the instance extensions.

	We filled out the ``VkInstanceCreateInfo`` structure and called ``vkCreateInstance()``, which if successful returned ``VK_SUCCESS`` and the ``VkInstance`` will be non-null.

	Then, we enumerated the ``VkPhysicalDevice`` s. Once we had an array of all the physical devices in the system, we called ``xrGetVulkanGraphicsDeviceKHR()`` to get the ``VkPhysicalDevice`` that OpenXR has requested. We checked the requested physical device against the array of physical devices and selected the correct one.

	With a ``VkPhysicalDevice`` selected, we created a ``VkDevice``. We got the ``VkQueueFamilyProperties`` from the ``VkPhysicalDevice`` and filled out an array of ``VkDeviceQueueCreateInfo`` structures. We found the first queue family that supports graphics operations and selected its index as the ``queueFamilyIndex`` and we also selected the first queue in that family too. We enumerated the Device extensions and filled out an array of structures. We used nested for-loops to find all the requested extensions and pushed them back into a ``std::vector<const char *>`` called ``activeDeviceExtensions``.

	``GetDeviceExtensionsForOpenXR()`` is another helper method.

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR

	*The above code is an excerpt from Common/GraphicsAPI_Vulkan.cpp*

	Like with ``xrGetVulkanInstanceExtensionsKHR()``, ``xrGetVulkanDeviceExtensionsKHR()`` is called twice, first to get the size of the ``char`` buffer and the second to fill in the data. We used ``std::stringstream`` and ``std::getline()`` with a deliminator of ``' '`` to break up the string and copied that substring into an element in a ``std::vector<std::string>``, which we used when setting the device extensions.

	Finally, we got the ``VkPhysicalDeviceFeatures`` by calling ``vkGetPhysicalDeviceFeatures()`` and we filled in the ``VkDeviceCreateInfo``. We called ``vkCreateDevice()``, which if successful returned ``VK_SUCCESS`` and the ``VkDevice`` will be non-null. 

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetGraphicsBinding
	
	*The above code is an excerpt from Common/GraphicsAPI_Vulkan.cpp*

	Here, we simply fill out the ``XrGraphicsBindingVulkanKHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``.

**************************
2.3 Polling the Event Loop
**************************

OpenXR uses an event based system to describes changes within the XR system. It's the application's responsibility to poll these events and react to them. The polling of events is done by the function ``xrPollEvent()``. The application should continually call this function throughout its lifetime. Within a single XR frame, the application should continuously call ``xrPollEvent()`` until the internal event queue is 'drained'; multiple events can occurs across the XR frame and the application needs to handle and respond to each accordingly.

Firstly, we will update the class. In the ``OpenXRTutorial::Run()`` method add the highlighted code below. Also add the highlighted code for the new methods and members in their separate private sections.

.. code-block:: cpp
	:emphasize-lines: 21-27, 68-73, 91-95

	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type api)
			: apiType(api) {
			if(!CheckGraphicsAPI_TypeIsValidForPlatform(apiType)) {
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

			CreateSession();

			while (m_applicationRunning) {
				PollSystemEvents();
				PollEvents();
				if (m_sessionRunning) {
					// Draw Frame.
				}
			}

			DestroySession();

			DestroyDebugMessenger();
			DestroyInstance();
	}

	private:
		void CreateInstance()
		{
			// [...]
		}
		void DestroyInstance()
		{
			// [...]
		}
		void GetInstanceProperties()
		{
			// [...]
		}
		void GetSystemID()
		{
			// [...]
		}
		void CreateDebugMessenger()
		{
			// [...]
		}
		void DestroyDebugMessenger()
		{
			// [...]
		}
		void CreateSession()
		{
			// [...]
		}
		void DestroySession()
		{
			// [...]
		}
		void PollEvents()
		{
		}
		void PollSystemEvents()
		{
		}

	private:
		XrInstance m_xrInstance = {};
		std::vector<const char *> m_activeAPILayers = {};
		std::vector<const char *> m_activeInstanceExtensions = {};
		std::vector<std::string> m_apiLayers = {};
		std::vector<std::string> m_instanceExtensions = {};

		XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

		XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemId m_systemID = {};

		GraphicsAPI_Type m_apiType = UNKNOWN;
		std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;

		XrSession m_session = {};
		XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;
		bool m_applicationRunning = true;
		bool m_sessionRunning = false;
	
		XrViewConfigurationType m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	};

2.3.1 xrPollEvent
=================

Copy the following code into the ``PollEvents()`` method:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollEvents
	:end-before: XR_DOCS_TAG_END_PollEvents
	:dedent: 8

Above, we have defined the ``PollEvents()`` method. Here, we use a do-while loop to the check the result of ``xrPollEvent()`` - whilst that function returns ``XR_SUCCESS``, there are events for us to process. ``xrPollEvent()`` will fill in the ``XrEventDataBuffer`` structure that we pass to the function call. ``xrPollEvent()`` will update the member variable ``type`` and from this we use a switch statement to select the appropriate code path. Depending on the updated type, we use a ``reinterpret_cast<>()`` to get the actual data that ``xrPollEvent()`` returned.

The description of the events come from `2.22.1. Event Polling of the OpenXR specification <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#_xrpollevent>`_.

+---------------------------------------------------+----------------------------------------+--------------------------------------------------------------------------------+
| Event Type                                        | Event Structure Type                   | Description                                                                    |
+---------------------------------------------------+----------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_EVENTS_LOST                    | XrEventDataEventsLost                  | The event queue has overflowed and some events were lost.                      |
+---------------------------------------------------+----------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING          | XrEventDataInstanceLossPending         | The application is about to lose the instance.                                 |
+---------------------------------------------------+----------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED    | XrEventDataInteractionProfileChanged   | The active input form factor for one or more top level user paths has changed. |
+---------------------------------------------------+----------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING | XrEventDataReferenceSpaceChangePending | The runtime will begin operating with updated space bounds.                    |
+---------------------------------------------------+----------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED          | XrEventDataSessionStateChanged         | The application has changed its lifecycle state.                               |
+---------------------------------------------------+----------------------------------------+--------------------------------------------------------------------------------+

As described in the table above, most events are transparent in their intensions and how the application should react to them. For the ``XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING`` state, the application may want to try re-creating the ``XrInstance`` in a loop, and after the specified ``lossTime``, until it can create a new instance successfully. ``XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED`` and ``XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING`` are used for updating how the user interacts with the application and whether a new space change has been detected respectively.

For some platforms, we need additional functionality provided via the ``PollSystemEvents()`` method, so that our application can react to any relevant updates from the platform correctly.

.. container:: windows linux
	:name: windows-linux-id-1

	For Windows and Linux, there no relevant system event that we need to be aware of, and thus the ``PollSystemEvents()`` method definition can be left blank.

.. container:: android
	:name: android-id-1

	For Android, we have already provided the code for the ``PollSystemEvents()`` method in :ref:`Chapter 1.4.3 <1.4.3 OpenXRTutorial and Main>`. So its duplicate definition must be removed from the class. This function is outside the scope of OpenXR, but in general it polls Android for system events,updates and uses the ``AndroidAppState``, ``m_applicationRunning`` and ``m_sessionRunning`` members.


2.3.2 XrSessionState
====================

The final event type, ``XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED``, in the above code and table is what we will focus on for the rest of this chapter. There are currently nine valid ``XrSessionState`` s described:

.. literalinclude:: ../build/openxr/include/openxr/openxr.h
		:language: cpp
		:start-at: typedef enum XrSessionState {
		:end-at: } XrSessionState;

*The above code is an excerpt from openxr/openxr.h*

Below is a table describing the nine ``XrSessionState`` s:

+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| Event Type                    | Description                                                                                                             |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_UNKNOWN      | This is an unknown, pseudo-default state and should not be returned by the runtime.                                     |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_IDLE         | This is an initial state after creating or after ending the session.                                                    |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_READY        | This state, raised from the runtime, indicates that the session is ready to begin.                                      |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_SYNCHRONIZED | The application has synced its frame loop with the runtime, but isn't displaying its contents.                          |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_VISIBLE      | The application has synced its frame loop with the runtime and it is displaying its contents.                           |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_FOCUSED      | The application has synced its frame loop with the runtime and it is displaying its contents and can receive XR inputs. |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_STOPPING     | The runtime is requesting that the application stop it frame loop rendering and end the session.                        |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_LOSS_PENDING | The runtime is indicating that current session is no longer valid and should be destroyed. (*)                          |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_EXITING      | The runtime is requesting that the application to destroy the session, usually from the user's request.                 |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+

(*) Applications may wish to re-create objects like ``XrSystemId`` and ``XrSession``, if hardware changes were detected.

Developers should also be aware of the lifecycle of an ``XrSession``. Certain ``XrSessionState``s can only lead to certain others under the correct circumstances. Below is a diagram showing the lifecycle of an ``XrSession`` within an OpenXR application.

.. figure:: openxr-session-life-cycle.svg
	:alt: OpenXR Session Life-Cycle
	:align: center
	:width: 99%

	OpenXR Session Life-Cycle

2.3.3 xrBeginSession and xrEndSession
=====================================

As the application runs, if the ``XrSessionState`` changes to ``XR_SESSION_STATE_READY``, the application can call ``xrBeginSession()`` to begin the session and synchronize the application's with the runtime's frame hook.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
	:dedent: 16

From the code that we copied in Chpater 2.1.3, we've assigned to ``XrSessionBeginInfo::primaryViewConfigurationType`` the ``m_viewConfiguration`` from the class, which in the case of this tutorial is ``XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO``. This specifies the view configuration of the form factor's primary display - For Head Mounted Displays, it is two views (one per eye).

If the ``XrSessionState`` changes to ``XR_SESSION_STATE_STOPPING``, the application should call ``xrEndSession()``. This means that the runtime has stop the session either from a user's input or from some other reason, our application should response by ending the session and freeing any resources. Below is a small excerpt from the code that we copied in Chpater 2.1.3.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
	:dedent: 16