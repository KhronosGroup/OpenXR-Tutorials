#####
Setup
#####

.. container:: windows
    :name: windows-id-1

	.. rubric:: Windows

	For your Windows OpenXR project, we'll use CMake to create some project files for Visual Studio.
	Create a directory where your code will go, we'll call this the *workspace* directory.

.. container:: linux
    :name: linux-id-1

	.. rubric:: Linux

	For your Linux OpenXR project, we'll use CMake alongside Visual Studio Code to build your project.
	Create a directory where your code will go, we'll call this the *workspace* directory. Open VS Code and from
	the File menu, select "Open Folder..."

	.. figure:: linux-vscode-open-folder.png
	   :alt: The File menu of Visual Studio Code is shown, with the command "Open Folder..." selected.
	   :align: center
	   :scale: 55%
   
	The File menu of Visual Studio Code, with the command "Open Folder..." selected

	Select your *workspace* folder, which is now empty.
	Install the CMake extension for Visual Studio Code.
	
.. container:: windows-linux
    :name: windows-linux-id-1

	.. rubric:: Windows and Linux

	Create a text file in the *workspace* folder called CMakeLists.txt.

	In it, put the following:

	.. highlight:: cmake
	.. code-block:: cmake

		cmake_minimum_required(VERSION 3.15)
		project(openxr-tutorial)
		set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
		set(OPENXR_DIR "" CACHE PATH "Location of OpenXR-SDK repository.")
		add_subdirectory(Chapter_2)

	Now let's create a folder called Chapter_2, and in it put another CMakeLists.txt file,
	this one containing:

	.. code-block:: cmake

		file(GLOB SOURCES "main.cpp" )
		add_executable(Chapter_2 ${SOURCES})
		target_include_directories( Chapter_2 PUBLIC ${OPENXR_DIR}/include )
		target_link_directories( Chapter_2 PUBLIC ${OPENXR_DIR}/build/src/loader/Debug ${OPENXR_DIR}/build/src/loader/Release )
		target_link_libraries( Chapter_2 openxr_loader$<$<CONFIG:Debug>:d> )

	That's all we need for CMake. Now we'll create our source file. Create a new text file called "main.cpp"
	and put it in the Chapter2 directory. In this file, place the following code:

.. highlight:: cpp
.. code-block:: cpp

	#if defined(_WIN32)
	#define NO_MINMAX
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#elif(__ANDROID__)
	#include "android_native_app_glue.h"
	#endif

	// C++ Headers
	#include <iostream>

This is boilerplate for the various platforms. Now add the following:

.. code-block:: cpp
	:emphasize-lines: 9
	:name: xr-headers

	// OpenXR Headers
	#include "openxr/openxr.h"

	//#define XR_USE_GRAPHICS_API_D3D11
	//#define XR_USE_GRAPHICS_API_D3D12
	//#define XR_USE_GRAPHICS_API_OPENGL
	//#define XR_USE_GRAPHICS_API_OPENGL_ES
	//#define XR_USE_GRAPHICS_API_VULKAN
	#include "openxr/openxr_platform.h"

Here we include the main OpenXR header, openxr.h; and the platform header openxr_platform.h.
What the latter does, depends on which of the preceding XR_USE\_ macros we enable. We will enable
one of these only, depending on your choice of graphics API.
Now add the following:

.. code-block:: cpp

	//Debugbreak
	#if defined(_WIN32)
	#define DEBUG_BREAK __debugbreak()
	#else
	#include <signal.h>
	#define DEBUG_BREAK raise(SIGTRAP)
	#endif

This defines the macro DEBUG_BREAK, according to what platform we're building for. This macro will
stop execution of your program when an error occurs, so you can see where it happened and fix it.

.. code-block:: cpp

	#define OPENXR_CHECK(x, y) { if (!XR_SUCCEEDED(x)) { std::cout << "ERROR: OPENXR: " << y << std::endl; } }

This defines the macro OPENXR_CHECK. Many OpenXR functions return a XrResult. This macro will check if the call has failed and logs a message to stdout. This can be modified to suit your needs.

Now we will define the main class of your application. It's just a stub for now, with an empty Run() method:

.. code-block:: cpp

	class OpenXRTutorial
	{
	public:
		OpenXRTutorial() = default;
		~OpenXRTutorial() = default;

		void Run() {
		}
	};

Finally, let's add the main function for your app. It looks slightly different, depending on your
choice of platform, but in each case, we define an instance of our OpenXRTutorial class, and call the Run()
method:

.. code-block:: cpp

	#if defined(_WIN32)
	int WINAPI wWinMain(_In_ HINSTANCE hInstance,
						 _In_opt_ HINSTANCE hPrevInstance,
						 _In_ LPWSTR    lpCmdLine,
						 _In_ int       nCmdShow) {
		OpenXRTutorial app;
		app.Run();
		return 0;
	}
	#elif(__ANDROID__)
	void android_main(struct android_app* app) {
		OpenXRTutorial app;
		app.Run();
	}
	#endif

.. rubric:: Windows
Now launch Cmake GUI, and point the "Where is the source code" box to your root solution (workspace) directory,
where your original CMakeLists.txt is located. Point the "Where to build the binaries" box to a subdirectory called "build",
click Configure, "Yes" to create the build folder, and "OK" to accept the default Generator.

.. image:: cmake-tutorial2-1.png
   :alt: Select 
   :align: right

Now we must tell the tutorial project where to find the OpenXR-SDK, which we built in Section 1.
Click on the variable OPENXR_DIR and type in or browse to the location of the OpenXR-SDK repository you downloaded
previously, the click "Generate". When the projects have been generated, open your new project, by clicking
"Open Project", or by finding the file Tutorial.sln in your build folder and double-clicking it.

.. rubric:: Linux
You now have three files, laid out as follow:

.. image:: linux-vscode-initial-files.png
   :alt: Select 
   :align: right

Having installed the CMake extension for VS Code, you can now right-click on the main CMakeLists.txt file (the one in the root *workspace* folder)
and select "Configure All Projects":

.. image:: linux-vscode-cmake-configure.png
   :alt: Select 
   :align: right

Now the CMake panel will be available by clicking its icon on the leftmost panel. Hover your mouse over the "Project Outline"
and click the three dots at the right. An option to "Edit CMake Cache" will appear, click this and you'll see the Cache Editor
page.


.. image:: linux-vscode-cmake-more-actions.png
   :alt: Select 
   :align: right
   

.. image:: linux-vscode-cmake-cache.png
   :alt: Select 
   :align: right

NOTE: It's possible to use CMake GUI in Linux, but using VS Code's integrated tools works better with VS Code.

In the CMake Cache editor, find OPENXR_DIR and enter the directory where you downloaded OpenXR-SDK in Chapter 1.
Now Configure and Build All.

.. rubric:: Windows

You can now build and run your program. It should compile and link with no errors or warnings.

.. rubric:: Linux

To enable debugging, select the Run/Debug panel in VS Code. You will now need to create a debugging configuration.
Click the "Gear" icon to edit the file launch.json, and enter the following:

.. code-block:: json

	{
		"version": "0.2.0",
		"configurations": [
			{
				"type": "cppdbg",
				"request": "launch",
				"name": "Chapter_2",
				"program": "${workspaceFolder}/build/Chapter_2/Chapter_2",
				"cwd":"${workspaceFolder}/Chapter_2",
				"externalConsole": true,
			}
		]
	}

2. SETUP (Basic structure ready)

Now that we have a basic application up and running with the OpenXR header files and libraries, we can start to set the core aspects of OpenXR. As a modern Khronos API, the OpenXR is heavily influcencd by the Vulkan API. So those who are familiar with the style of the Vulkan API will find OpenXR easy to follow.

Creating an XrInstance
----------------------
2.1. Creating an XrInstance / xrGetSystem (xrCreateInstance)

Firstly, add to the `OpenXRTutorial` class the methods: `CreateInstance()`, `GetInstanceProperties()` and `DestroyInstance()`. Update `OpenXRTutorial::Run()` to call those methods in that order and add to the class in a private section the following members.

.. code-block::
	
	class OpenXRTutorial
	{
	public:
		OpenXRTutorial() = default;
		~OpenXRTutorial() = default;
	
		Run()
		{
			CreateInstance();
			
			GetInstanceProperties();
			
			DestroyInstance();
		}

	private:
		void CreateInstance() 
		{
		} 
		
		void DestroyInstance();
		{
		}
	
		void GetInstanceProperties()
		{
		}
	
	private:
		XrInstance instance = {};
		std::vector<const char*> activeAPILayers = {};
		std::vector<const char*> activeInstanceExtensions = {};
		std::vector<std::string> apiLayers = {};
		std::vector<std::string> instanceExtensions = {};
	}

The `XrInstance` is the foundational object that we need to create first. The `XrInstance` encompasses the application setup state, OpenXR API version and any layers and extensions. So inside the `CreateInstance()` method, we will first look at the `XrApplicationInfo`.

.. code-block:: cpp

	XrApplicationInfo AI;
	strcpy(AI.applicationName, "OpenXR Tutorial Chapter 2.1");
	AI.applicationVersion = 1;
	strcpy(AI.engineName, "OpenXR Engine");
	AI.engineVersion = 1;
	AI.apiVersion = XR_CURRENT_API_VERSION;

This structure allows you specify both the name and the version for your application and engine. These members are solely for your use as the application developer. The main member here is the `XrApplicationInfo::apiVersion`. Here we use the `XR_CURRENT_API_VERSION` macro to specific the OpenXR version that we want to run. Also note here the use of `strcpy()` to set the applicationName and engineName. If you look at `XrApplicationInfo::applicationName` and `XrApplicationInfo::engineName` members, they are of type `char[]`, hence you must copy your string into that `char[]` and you must also by aware of the allowable length.

Similar to Vulkan, OpenXR allows applications to extend functionality past what is provided by the core specification. The functionality could be hardware/vendor specific. Most vital of course is which Graphics API to use with OpenXR. OpenXR supports D3D11, D3D12, Vulkan, OpenGL and OpenGL ES. Due the extensible nature of specification, it allows newer Graphics APIs and hardware functionality to be added with ease.

.. code-block:: cpp

	instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);

	#if defined(XR_USE_GRAPHICS_API_D3D11)
		instanceExtensions.push_back(XR_KHR_D3D11_ENABLE_EXTENSION_NAME);
	#elif defined(XR_USE_GRAPHICS_API_D3D12)
		instanceExtensions.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
	#elif defined(XR_USE_GRAPHICS_API_OPENGL)
		instanceExtensions.push_back(XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);
	#elif defined(XR_USE_GRAPHICS_API_OPENGL_ES)
		instanceExtensions.push_back(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);
	#elif defined(XR_USE_GRAPHICS_API_VULKAN)
		instanceExtensions.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);
	#endif

Here, we store in a `std::vector<std::string>` the extension names that we would like to use. `XR_EXT_DEBUG_UTILS_EXTENSION_NAME` is a macro of a string defined in openxr.h. The XR_EXT_debug_utils is extension that checks the validity of calls made to OpenXR, and can use a call back function to handle any raised errors. We will explore this extension more in Chapter 5.1. Depending on which `XR_USE_GRAPHICS_API_...` macro that you have defined, this code will add the relevant extension.

Not all API layers and extensions are available to use, so we much check which ones can use. We will use `xrEnumerateApiLayerProperties()` and `xrEnumerateInstanceExtensionProperties()` to check which ones the runtime can provide.

.. code-block:: cpp

	uint32_t apiLayerCount = 0;
	std::vector<XrApiLayerProperties> apiLayerProperties;
	OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
	apiLayerProperties.resize(apiLayerCount);
	for (auto& apiLayerProperty : apiLayerProperties)
		apiLayerProperty.type = XR_TYPE_API_LAYER_PROPERTIES;
	OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");

	for (auto& requestLayer : apiLayers)
	{
		for (auto& layerProperty : apiLayerProperties)
		{
			if (strcmp(requestLayer.c_str(), layerProperty.layerName))
				continue;
			else
				activeAPILayers.push_back(requestLayer.c_str()); break;
		}
	}

	uint32_t extensionCount = 0;
	std::vector<XrExtensionProperties> extensionProperties;
	OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");
	extensionProperties.resize(extensionCount);
	for (auto& extensionProperty : extensionProperties)
		extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;
	OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");

	for (auto& requestExtension : instanceExtensions)
	{
		for (auto& extensionProperty : extensionProperties)
		{
			if (strcmp(requestExtension.c_str(), extensionProperty.extensionName))
				continue;
			else
				activeInstanceExtensions.push_back(requestExtension.c_str()); break;
		}
	}

These functions are called twice. The first time is to get the count of the API layers or extensions and the second is to fill out the array of structures. Before the second call, we need set `XrApiLayerProperties::type` or `XrExtensionProperties::type` to the correct value, so that the second call can correctly fill out the data. After we have enumerated the API layers and extensions, we use a nested loop to check to see whether an API layers or extensions is availble and add it to the activeAPILayers and/or activeInstanceExtensions respectively. Note the activeAPILayers and activeInstanceExtensions are of type `std::vector<const char*>`. This will help us when fill out the next structure `XrInstanceCreateInfo`.

.. code-block:: cpp

	XrInstanceCreateInfo instanceCI;
	instanceCI.type = XR_TYPE_INSTANCE_CREATE_INFO;
	instanceCI.next = nullptr;
	instanceCI.createFlags = 0;
	instanceCI.applicationInfo = AI;
	instanceCI.enabledApiLayerCount = static_cast<uint32_t>(activeAPILayers.size());
	instanceCI.enabledApiLayerNames = activeAPILayers.data();
	instanceCI.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
	instanceCI.enabledExtensionNames = activeInstanceExtensions.data();
	OPENXR_CHECK(xrCreateInstance(&instanceCI, &instance), "Failed to create Instance.");

This section is fairly simple, as we now just collect data from before and assign them to members in the `XrInstanceCreateInfo` structure. Finally, we get to call `xrCreateInstance()` where we take pointers to thr stack `XrInstanceCreateInfo` and `XrInstance` objects. If the function succeeded, the result will be XR_SUCCESS and `XrInstance` will be non-null.

At the end of the program, we should destroy the `XrInstance`. This is simple done with the function `xrDestroyInstance()`.

.. code-block:: cpp

	void DestroyInstance()
	{
		OPENXR_CHECK(xrDestroyInstance(instance), "Failed to destroy Instance.");
	}

Whilst we have an `XrInstance`, lets check its properties. We fill out the type and next members of the structure `XrInstanceProperties` and pass it along with the `XrInstance` to `xrGetInstanceProperties()`. This function will fill out the rest of that structure for us to use. Here, we simply log to stdout the runtime's name, and with the use of the `XR_VERSION_MAJOR`, `XR_VERSION_MINOR` and `XR_VERSION_PATCH` macros, we parse and log the runtime version.

.. code-block:: cpp																								  

	void GetInstanceProperties()
	{
		XrInstanceProperties instanceProperties;
		instanceProperties.type = XR_TYPE_INSTANCE_PROPERTIES;
		instanceProperties.next = nullptr;
		OPENXR_CHECK(xrGetInstanceProperties(instance, &instanceProperties), "Failed to get InstanceProperties.");

		std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - ";
		std::cout << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_PATCH(instanceProperties.runtimeVersion);
	}


Creating an XrSession
---------------------

2.2. Creating an XrSession (xrCreateSession, OpenGL based for code brevity)

Polling the Event Loop
----------------------

2.3. 2.3. Polling the Event Loop (xrPollEvent and Session States)