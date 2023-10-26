##############
2 OpenXR Setup
##############

With your project created and your application building and running, we can start to use OpenXR. The goal of this chapter is to create an ``XrInstance`` and an ``XrSession``, and setup the OpenXR event loop. This OpenXR code is needed to setup the core functionality of an OpenXR application to have that application interact with the OpenXR runtime and your graphics API correctly.

****************************************
2.1 Creating an XrInstance / xrGetSystem
****************************************

We will continue to use the ``OpenXRTutorial`` class in ``Chapter2/main.cpp`` that we created in :ref:`Chapter 1.4 <1.4 Project Setup>`.

Here, we will add the following highlighted text to the ``OpenXRTutorial`` class:

.. code-block:: cpp
	:emphasize-lines: 9-16 , 20-31, 36-48
	
	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type apiType)
			: m_apiType(apiType)  {
		}
		~OpenXRTutorial() = default;
	
		void Run() {
			CreateInstance();
			CreateDebugMessenger();

			GetInstanceProperties();
			GetSystemID();

			DestroyDebugMessenger();
			DestroyInstance();
		}

	private:
		void CreateInstance() {
		}
		void DestroyInstance() {
		}
		void CreateDebugMessenger() {
		}
		void DestroyDebugMessenger() {
		}
		void GetInstanceProperties() {
		}
		void GetSystemID() {
		}
		void PollSystemEvents() {
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
		XrSystemProperties m_systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

		GraphicsAPI_Type m_apiType = UNKNOWN;

		bool m_applicationRunning = true;
		bool m_sessionRunning = false;
	};

First, we updated ``OpenXRTutorial::Run()`` to call the new methods ``CreateInstance()``, ``GetInstanceProperties()``, ``GetSystemID()`` and ``DestroyInstance()`` in that order. Finally, we added those methods and the following members to the class within their separate private sections.

2.1.1 XrInstance
================

The ``XrInstance`` is the foundational object that we need to create first. The ``XrInstance`` encompasses the application setup state, OpenXR API version and any layers and extensions. So inside the ``CreateInstance()`` method, we will first add the code for the ``XrApplicationInfo``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrApplicationInfo
	:end-before: XR_DOCS_TAG_END_XrApplicationInfo
	:dedent: 8

This structure allows you to specify both the name and the version for your application and engine. These members are solely for your use as the application developer. The main member here is the ``XrApplicationInfo::apiVersion``. Here we use the ``XR_CURRENT_API_VERSION`` macro to specify the OpenXR version that we want to run. Also, note here the use of ``strncpy()`` to set the name strings. If you look at ``XrApplicationInfo::applicationName`` and ``XrApplicationInfo::engineName`` members, they are of type ``char[]``, so you must copy your string into that buffer. Also, be aware of the allowable length.

.. container:: vulkan

	Note the slight difference in the approach the OpenXR API takes compared to the Vulkan API. In OpenXR, name strings are explicitly copied into structures like ``XrApplicationInfo``, which contain fixed-size string buffers, whereas in Vulkan, structures such as ``VkApplicationInfo`` take pointers to C strings of arbitrary size.

.. _instanceextensions:

Similarly to Vulkan, OpenXR allows applications to extend functionality past what is provided by the core specification. The added functionality could be hardware/vendor specific. Most vital of course is which Graphics API to use with OpenXR. OpenXR supports D3D11, D3D12, Vulkan, OpenGL and OpenGL ES. Due to the extensible nature of specification, it allows newer Graphics APIs and hardware functionality to be added with ease. Following on from the previous code in the ``CreateInstance()`` method, add the following:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_instanceExtensions
	:end-before: XR_DOCS_TAG_END_instanceExtensions
	:dedent: 12

Here, we store in a vector of strings the extension names that we would like to use. ``XR_EXT_DEBUG_UTILS_EXTENSION_NAME`` is a macro of a string defined in ``openxr.h``. The XR_EXT_debug_utils is an extension that checks the validity of calls made to OpenXR and can use a callback function to handle any raised errors. We will explore this extension more in :ref:`Chapter 2.1<2.1.2 XR_EXT_debug_utils>`. Depending on which ``XR_USE_GRAPHICS_API_...`` macro you defined, this code will add the relevant extension.

Not all API layers and extensions are available to use, so we must check which ones are available when OpenXR is initialized. We will use ``xrEnumerateApiLayerProperties()`` and ``xrEnumerateInstanceExtensionProperties()`` to check which ones the runtime can provide. Let's do this now by adding the following code to the ``CreateInstance()`` method:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_find_apiLayer_extension
	:end-before: XR_DOCS_TAG_END_find_apiLayer_extension
	:dedent: 8

These functions are called twice. The first time is to get the count of the API layers or extensions and the second is to fill out the array of structures - this is called the "two-call idiom". Before the second call, we need to set ``XrApiLayerProperties::type`` or ``XrExtensionProperties::type`` to the correct value, so that the second call can correctly fill out the data. After we have enumerated the API layer and extension, we use a nested loop to check to see whether an API layers or extensions is available and add it to the ``m_activeAPILayers`` and/or ``m_activeInstanceExtensions`` respectively. 

.. container:: vulkan

	There is a subtle difference here from the two-call idiom in the Vulkan API. In OpenXR, we provide an explicit capacity in the first argument, which provides an additional layer of memory-safety.

Note the ``m_activeAPILayers`` and ``m_activeInstanceExtensions`` are of type ``std::vector<const char *>``. This will help us when fill out the next structure ``XrInstanceCreateInfo``.

Now that we've assembled all of the information needed we will fill the ``XrInstanceCreateInfo`` structure, and add the following to the ``CreateInstance()`` method.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrInstanceCreateInfo
	:end-before: XR_DOCS_TAG_END_XrInstanceCreateInfo
	:dedent: 8

This section is fairly simple: we have used the previously collected data and assigned it to the members in the ``XrInstanceCreateInfo`` structure. Then, we called ``xrCreateInstance()`` where we took pointers to the ``XrInstanceCreateInfo`` and ``XrInstance`` objects. When the function is called, if successful, it returns ``XR_SUCCESS`` and ``XrInstance`` will be non-null (i.e. not equal to ``XR_NULL_HANDLE``).

At the end of the app, we should destroy the ``XrInstance`` with ``xrDestroyInstance()``. Add the following to the ``DestroyInstance()`` method:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrInstanceDestroy
	:end-before: XR_DOCS_TAG_END_XrInstanceDestroy
	:dedent: 8

While we do have an ``XrInstance``, let's check its properties. Add the following code to the ``GetInstanceProperties()`` method:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GetInstanceProperties
	:end-before: XR_DOCS_TAG_END_GetInstanceProperties
	:dedent: 8

Here, we have initialized the ``XrInstanceProperties`` with the correct ``XrStructureType`` and passed it along with the ``XrInstance`` to ``xrGetInstanceProperties()``. This will fill the rest of that structure. Next, we logged out the runtime's name, and using the ``XR_VERSION_...`` macros, we parsed and logged the runtime version.

2.1.2 XR_EXT_debug_utils
========================

XR_EXT_debug_utils is an instance extension for OpenXR, which allows the application to get more information on errors, warnings and messages raised by the runtime. You can specify which message severities and types will checked. If a debug message is raised, it is passed to the callback function, which can optionally use the user data pointer provided in the ``XrDebugUtilsMessengerCreateInfoEXT`` structure.

The message severities are:
 * Verbose: Output all messages.
 * Info: Output at least information messages helpful in debugging.
 * Warning: Output at least messages that could suggest an application bug and that need reviewing.
 * Error: Output messages from errors that may cause undefined behavior and/or crashes.
 
The message types are:
 * General: General information.
 * Validation: indicates possibly invalid usage of OpenXR.
 * Performance: indicates possible non-optimal usage of OpenXR.
 * Conformance: indicates a non-conformant OpenXR result from the runtime.

See also `Debug Message Categorization <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#debug-message-categorization>`_ in the OpenXR Specification. 

Copy the following code into ``CreateDebugMessenger()`` and ``DestroyDebugMessenger()`` respectively:

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateDebugMessenger
	:end-before: XR_DOCS_TAG_END_CreateDebugMessenger
	:dedent: 8

In the above, we first check that ``XR_EXT_DEBUG_UTILS_EXTENSION_NAME`` or ``XR_EXT_debug_utils`` is in ``activeInstanceExtensions``, which we used to create the ``XrInstance``. Next, we call the ``CreateOpenXRDebugUtilsMessenger()`` function.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_DestroyDebugMessenger
	:end-before: XR_DOCS_TAG_END_DestroyDebugMessenger
	:dedent: 8

Another feature of OpenXR is the API Layers, which may also assist you in debugging. You can read more about them in :ref:`Chapter 6.3 <6.3 OpenXR API Layers>`.

2.1.3 XrSystemId
================

The next object we want to get is the ``XrSystemId``. According to `System <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#system>`_ in the OpenXR spec, OpenXR 'separates the concept of physical systems of XR devices from the logical objects that applications interact with directly. A system represents a collection of related devices in the runtime, often made up of several individual hardware components working together to enable XR experiences. So, an ``XrSystemId`` could represent a VR headset and a pair of controllers, or perhaps a mobile device with video pass-through for AR. So we need to decide what type of ``XrFormFactor`` we want to use, as some runtimes support multiple form factors. Here, we are selecting ``XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY``. OpenXR currently offers two options for the ``XrFormFactor``:

.. literalinclude:: ../build/_deps/openxr-build/include/openxr/openxr.h
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

Here, we have filled out the ``XrSystemGetInfo`` structure with the desired ``XrFormFactor`` and passed it as a pointer along with the ``XrInstance`` and a pointer to the ``XrSystemId`` to the ``xrGetSystem()`` function. When the function is called, if successful, ``XrSystemId`` will be non-null.

With the above code, we have also got the system's properties. We partially filled out a ``XrSystemProperties`` structure and passed it as a pointer along with the ``XrInstance`` and the ``XrSystemId`` to the ``xrGetSystemProperties()`` function. This function will fill out the rest of the ``XrSystemProperties`` structure; detailing the vendor's ID, the system's name and the system's graphics and tracking properties.

.. literalinclude:: ../build/_deps/openxr-build/include/openxr/openxr.h
	:language: cpp
	:start-at: typedef struct XrSystemGraphicsProperties {
	:end-at: } XrSystemProperties;

*The above code is an excerpt from openxr/openxr.h*

You can now run the application to check that you have a valid ``XrInstance`` and ``XrSystemId``.

.. container:: linux

	Make sure your Monado service is running prior to running your app.

*************************
2.2 Creating an XrSession
*************************

The next major component of OpenXR that needs to be created in an ``XrSession``. An ``XrSession`` encapsulates the state of the application from the perspective of OpenXR. When an ``XrSession`` is created, it starts in the ``XR_SESSION_STATE_IDLE``. It is up to the runtime to provide any updates to the ``XrSessionState`` and for the application to query them and react to them. We will explore this in :ref:`Chapter 2.3<2.3 Polling the Event Loop>`.

For now, we are just going to create an ``XrSession``. At this point, you'll need to select which Graphics API you wish to use. Only one Graphics API can be used with an ``XrSession``. This tutorial demonstrates how to use D3D11, D3D12, OpenGL, OpenGL ES and Vulkan in conjunction with OpenXR to render graphics to the provided views. Ultimately, you will most likely be bringing your own rendering solution to this tutorial, therefore the code examples provided for the Graphics APIs are `placeholders` for your own code base; demonstrating in this sub-chapter what objects are needed from your Graphics API to create an ``XrSession``. This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. There are both compile and runtime checks to select the requested Graphics API, and we construct an appropriate derived class through the use of ``std::unique_ptr<>``. 

Update the constructor of the ``OpenXRTutorial`` class, the ``OpenXRTutorial::Run()`` method and also add the definitions of the new methods and the members to their separate private sections. All the new code is highlighted code below.

.. code-block:: cpp
	:emphasize-lines: 5-9, 19-20, 51-56, 76-78

	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type apiType)
			: m_apiType(apiType) {
			if(!CheckGraphicsAPI_TypeIsValidForPlatform(m_apiType)) {
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
		XrSystemProperties m_systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

		GraphicsAPI_Type m_apiType = UNKNOWN;
		std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;

		XrSession m_session = XR_NULL_HANDLE;

		bool m_applicationRunning = true;
		bool m_sessionRunning = false;
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

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_D3D11>
		:end-at: );
		:dedent: 12

.. container:: d3d12

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_D3D12>
		:end-at: );
		:dedent: 12

.. container:: opengl

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL>
		:end-at: );
		:dedent: 12

.. container:: opengles

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL_ES>
		:end-at: );
		:dedent: 12

.. container:: vulkan

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-at: m_graphicsAPI = std::make_unique<GraphicsAPI_Vulkan>
		:end-at: );
		:dedent: 12

	When creating the Vulkan Instance, developers should be aware of any Vulkan layers that they may have externally activated.

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

Above is the code for creating and destroying an ``XrSession``. ``xrDestroySession()`` will destroy the ``XrSession`` when we are finished and shutting down the application. ``xrCreateSession()`` takes the ``XrInstance``, ``XrSessionCreateInfo`` and a ``XrSession`` return. If the function call is successful, ``xrCreateSession()`` will return ``XR_SUCCESS`` and ``m_session`` will be non-null. 

In the ``XrSessionCreateInfo`` structure, ``createFlags`` and ``systemId`` are specified, and we need to specify which Graphics API we wish to use. This is achieved via the use of the ``XrSessionCreateInfo::next`` void pointer. Following the Vulkan style of extensibility, structures for creating objects can be extended to enable extra functionality. In our case, the extension is required and thus ``XrSessionCreateInfo::next`` can not be a nullptr. That pointer must point to 'exactly one graphics API binding structure (a structure whose name begins with "XrGraphicsBinding")' (`XrSessionCreateInfo(3) Manual Page <https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrSessionCreateInfo.html>`_). We get a pointer to the correct *Graphics Binding* structure by calling ``GraphicsAPI::GetGraphicsBinding();``.


**************************
2.3 Polling the Event Loop
**************************

OpenXR uses an event-based system to describe changes within the XR system. It's the application's responsibility to poll these events and react to them. The polling of events is done by the function ``xrPollEvent()``. The application should continually call this function throughout its lifetime. Within a single XR frame, the application should continuously call ``xrPollEvent()`` until the internal event queue is 'drained'; multiple events can occur across the XR frame and the application needs to handle and respond to each accordingly.

Firstly, we will update the class. In the ``OpenXRTutorial::Run()`` method add the highlighted code below. Also, add the highlighted code for the new methods and members in their separate private sections.

.. code-block:: cpp
	:emphasize-lines: 21-27, 68-70, 92, 96

	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type apiType)
			: m_apiType(apiType) {
			if(!CheckGraphicsAPI_TypeIsValidForPlatform(m_apiType)) {
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
		XrSystemProperties m_systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

		GraphicsAPI_Type m_apiType = UNKNOWN;
		std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;

		XrSession m_session = XR_NULL_HANDLE;
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

Above, we have defined the ``PollEvents()`` method. Here, we use a do-while loop to check the result of ``xrPollEvent()`` - whilst that function returns ``XR_SUCCESS``, there are events for us to process. ``xrPollEvent()`` will fill in the ``XrEventDataBuffer`` structure that we pass to the function call. ``xrPollEvent()`` will update the member variable ``type`` and from this, we use a switch statement to select the appropriate code path. Depending on the updated type, we use a ``reinterpret_cast<>()`` to get the actual data that ``xrPollEvent()`` returned.

The description of the events comes from `2.22.1. Event Polling of the OpenXR specification <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#_xrpollevent>`_.

+---------------------------------------------------+--------------------------------------------------------------------------------+
| Event Type                                        | Description                                                                    |
+---------------------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_EVENTS_LOST                    | The event queue has overflowed and some events were lost.                      |
+---------------------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING          | The application is about to lose the instance.                                 |
+---------------------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED    | The active input form factor for one or more top-level user paths has changed. |
+---------------------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING | The runtime will begin operating with updated space bounds.                    |
+---------------------------------------------------+--------------------------------------------------------------------------------+
| XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED          | The application has changed its lifecycle state.                               |
+---------------------------------------------------+--------------------------------------------------------------------------------+

As described in the table above, most events are transparent in their intentions and how the application should react to them. For the ``XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING`` state, the application may want to try re-creating the ``XrInstance`` in a loop, and after the specified ``lossTime``, until it can create a new instance successfully.
``XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED`` and ``XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING`` are used for updating how the user interacts with the application and whether a new space change has been detected respectively.

2.3.2 XrSessionState
====================

The final event type, ``XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED``, in the above code and table, is what we will focus on for the rest of this chapter. There are currently nine valid states:

.. literalinclude:: ../build/_deps/openxr-build/include/openxr/openxr.h
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
| XR_SESSION_STATE_SYNCHRONIZED | The application has synced its frame loop with the runtime but isn't displaying its contents.                           |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_VISIBLE      | The application has synced its frame loop with the runtime and it is displaying its contents.                           |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_FOCUSED      | The application has synced its frame loop with the runtime and it is displaying its contents and can receive XR inputs. |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_STOPPING     | The runtime is requesting that the application stop its frame loop rendering and end the session.                       |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_LOSS_PENDING | The runtime is indicating that the current session is no longer valid and should be destroyed. (*)                      |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_EXITING      | The runtime is requesting that the application to destroy the session, usually from the user's request.                 |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+

(*) Applications may wish to re-create objects like ``XrSystemId`` and ``XrSession`` if hardware changes are detected.

Developers should also be aware of the lifecycle of an ``XrSession``. Certain ``XrSessionState`` s can only lead to certain others under the correct circumstances. Below is a diagram showing the lifecycle of an ``XrSession`` within an OpenXR application.

.. figure:: openxr-session-life-cycle.svg
	:alt: OpenXR Session Life-Cycle
	:align: center
	:width: 99%

	OpenXR Session Life-Cycle

2.3.3 xrBeginSession and xrEndSession
=====================================

As the application runs, if the ``XrSessionState`` changes to ``XR_SESSION_STATE_READY``, the application can call ``xrBeginSession()`` to begin the session and synchronize the application with the runtime's frame hook.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
	:dedent: 16

From the code that we copied in Chapter 2.1.3, we've assigned to ``XrSessionBeginInfo::primaryViewConfigurationType`` the ``m_viewConfiguration`` from the class, which in the case of this tutorial is ``XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO``. This specifies the view configuration of the form factor's primary display - For Head Mounted Displays, it is two views (one per eye).

If the ``XrSessionState`` changes to ``XR_SESSION_STATE_STOPPING``, the application should call ``xrEndSession()``. This means that the runtime has stopped the session either from a user's input or from some other reason, our application should respond by ending the session and freeing any resources. Below is a small excerpt from the code that we copied in Chapter 2.1.3.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
	:dedent: 16

***********
2.4 Summary
***********

We have now created an XR application that can begin and stop an ``XrSession``, next we will look to add graphics to our application!

Below is a download link to a zip archive for this chapter containing all the C++ and CMake code for all platform and graphics APIs.

:download:`Chapter2.zip <../build/eoc_archs/Chapter2.zip>`