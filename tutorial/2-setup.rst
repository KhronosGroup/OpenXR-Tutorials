##########
⚙️ 2 Setup
##########

Select your platform, as the instructions are different depending on your selection.

.. raw:: html
	:file: platforms.html

.. container:: windows
	:name: windows-id-1

	For the Windows OpenXR project, we'll use CMake to create some project files for Visual Studio.
	First, create a directory where the code will go, we'll call this the *workspace* directory.

.. container:: linux
	:name: linux-id-1

	You are free to use any code editor and/or compiler; this tutorial will use Visual Studio Code.
	For the Linux OpenXR project, we'll use CMake alongside Visual Studio Code to build the project.
	First, Create a directory where the code will go, we'll call this the *workspace* directory. Open Visual Studio Code and from the File menu, select "Open Folder..."

	.. figure:: linux-vscode-open-folder.png
		:alt: The File menu of Visual Studio Code is shown, with the command "Open Folder..." selected.
		:scale: 55%

	The File menu of Visual Studio Code, with the command "Open Folder..." selected

	Select your *workspace* folder, which is now empty.
	If you haven't previously done so, install the CMake extension for Visual Studio Code.
	
.. container:: windows linux
	:name: windows-linux-id-1

	Now, create a text file in the *workspace* folder called ``CMakeLists.txt`` and in it, put the following code:

	.. highlight:: cmake
	.. code-block:: cmake

		cmake_minimum_required(VERSION 3.15)
		project(openxr-tutorial)
		set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
		set(OPENXR_DIR "" CACHE PATH "Location of OpenXR-SDK repository.")
		add_subdirectory(Chapter_2)

	Now let's create a folder called Chapter_2, and in it create another ``CMakeLists.txt`` file.
	This one contains the following code:

	.. code-block:: cmake

		file(GLOB SOURCES "main.cpp" )
		add_executable(Chapter_2 ${SOURCES})
		target_include_directories( Chapter_2 PUBLIC ${OPENXR_DIR}/include )
		target_link_directories( Chapter_2 PUBLIC ${OPENXR_DIR}/build/src/loader/Debug ${OPENXR_DIR}/build/src/loader/Release )
		target_link_libraries( Chapter_2 openxr_loader$<$<CONFIG:Debug>:d> )

	That's all we need for CMake, we need for this project. 
	Now, we'll create our source file. Create a new text file called ``main.cpp`` in the Chapter2 directory.

.. container:: android
	:name: android-id-1

	.. rubric:: Android

	Here, We'll show how to hand build an Android Studio project that runs a C++ Native Activity.
	Open Android Studio, select New Project and choose an Empty Activity. Set the names and save location. The language can be ignored here as we are using C++, and we can set the Minimum SDK to API 24: Android 7.0(Nougat). Complete set up.

	.. figure:: android-studio-newproject.png
		:alt: Android Studio - New Project - Empty Activity.
		:align: left

	.. rubric:: CMake

	With the Android Studio project now set up, we need to modify some of the files and folders so as to set up the project to support the C++ Native Activity.
	Under the ``app`` folder, you can delete the ``libs`` folder, and under the ``app/src`` you can also delete the ``androidTest`` and ``test`` folders. Finally under ``app/src/main``, delete the ``java`` folder and add a ``cpp`` folder. Under the ``app/src/main/res``, delete the ``values-night`` and ``xml`` folders. Under the ``values`` modify ``colors.xml`` and ``styles.xml`` as shown.

	.. rubric:: colors.xml

	.. literalinclude:: ../Chapter2/app/src/main/res/values/colors.xml
		:language: xml

	.. rubric:: styles.xml

	.. literalinclude:: ../Chapter2/app/src/main/res/values/styles.xml
		:language: xml

	Create a ``CMakeLists.txt`` in the directory above for compatiblity with other platforms. We will use this file to specific how our Native C++ code will be built. This ``CMakeLists.txt`` file will be invoked by Android Studio's Gradle build system and we will point Gradle to this CMake file. 

	.. rubric:: CMakeLists.txt

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake 
		
	First, we set the minimum required cmake version, here we are using 3.22.1 and the project's name. Next, we need to add a static library called native_app_glue. The native_app_glue library is compiled from a single source file ``android_native_app_glue.c``. This interfaces between the Java Virtual Machine and our C++ code. Ultimately, it allows us to use the ``void android_main(struct android_app*)`` entry point. We also include that directory as we need access to the android_native_app_glue.h header file. Next, we need to set the ``CMAKE_SHARED_LINKER_FLAGS`` so that ``ANativeActivity_onCreate()`` is exported for the Java Virtual Machine to call. Next, we add our shared library openxrtutorialch2 that houses our code. Here, I have a relative path to our single C++ file.

	Now, we import the openxr_loader library. We need to do this, because it's external to the NDK library, and won't be automatically picked up. We call ``set_target_properties()`` to specific the location of ``libopenxr_loader.so``. We also include the directory to the OpenXR headers. Next, we find the Vulkan library in the NDK and include the directory to the Android Vulkan headers. At this time, we also find the log library. Finally we link the android, native_app_glue, openxr_loader, vulkan and log libraries to our openxrtutorialch2 library. Our ``libopenxrtutorialch2.so`` will packageed inside our .apk along with any shared libraries that we have linked.

	.. rubric:: AndroidManifest.xml

	.. literalinclude:: ../Chapter2/app/src/main/AndroidManifest.xml
		:language: xml

	We now need to modify our ``AndroidManifest.xml`` file to tell Android to run a Native Activity. We set ``android:name`` to "android.app.NativeActivity" and update ``android:configChanges`` to "orientation|keyboardHidden" to not close the activity on those changes. Next under the meta-data section, we set these values: ``android:name`` to "android.app.lib_name" and ``android:value`` to "openxrtutorialch2", where ``android:value`` is name of the library we created in the CMakeLists, thus pointing our NativeActivity to the correct library.

	We need to tell the app that it should take over rendering when active, rather than appearing in a window. Set ``<category android:name="org.khronos.openxr.intent.category.IMMERSIVE_HMD" />``.
	Note: not all devices yet support this category. For example, for Oculus Quest devices you will need ``<category android:name="com.oculus.intent.category.VR" />`` for the same purpose.

	.. rubric:: Gradle

	.. literalinclude:: ../Chapter2/app/build.gradle
		:language: groovy
	
	Now, we can config our ``build.gradle`` file in the ``app`` folder. First remove any references to Java, Kotlin and to testing. Next add in the ``externalNativeBuild`` section specifying CMake, its version and the location of the CMakeLists.txt that we created earlier. Also specify under the ``ndk`` section the ``abiFilters``. We will just be using arm64-v8a in this tutorial. ``ndkVersion`` should also be specified.

	.. literalinclude:: ../Chapter2/build.gradle
		:language: groovy

	Now, we can config our ``build.gradle`` file in the root folder of the project. This is a complete replacement the default one provided by Android Studio. This file stipulates the repositories and gradle version to be used.
	The settings.gradle can be reduce to just: ``include ':app'``, and in the ``gradle.properties`` we need to remove ``kotlin.code.style=official`` and ``android.nonTransitiveRClass=true``.

	With that completed, we should now be able to sync the Gradle file and build the project.

	Now, we'll create our source file. Create a new text file called ``main.cpp`` in the Chapter2 directory.
	This file will be referenced in the CMakeLists file we created, so ensure the path is correct.

Now, that we have set up the project and source file. We will create two header files in a ``/Common`` directory called ``HelperFunctions.h`` and ``OpenXRHelper.h``
Open the header files and add the following code to ``Helperfunctions.h``:

.. literalinclude:: ../Common/HelperFunctions.h
	:language: cpp
	:start-at: // C Headers
	:end-at: #include <unordered_map>

This is boilerplate for the various platforms. Next, we'll add the header files related to OpenXR to ``OpenXRHelper.h``.

.. literalinclude:: ../Common/OpenXRHelper.h
	:language: cpp
	:start-at: // OpenXR Headers
	:end-at: #include "openxr/openxr_platform.h"
	:emphasize-lines: 3

.. code-block:: cpp

	#if defined(__ANDROID__)
	#include "android_native_app_glue.h"
	#define XR_USE_PLATFORM_ANDROID
	#endif

Here, we include the main OpenXR header file ``openxr.h`` and the OpenXR platform header file ``openxr_platform.h``.
For the OpenXR platform header file, note the preceding XR_USE\_ macros. When enabled, we gain access to functionality that interact with the chosen graphics API and/or platform. We will enable one of these graphics ones later in the tutorial. 
For Android, we include our ``android_native_app_glue.h`` header file as well as defining the ``XR_USE_PLATFORM_ANDROID`` macro, which we will need to initialise the load the OpenXR loader.

Next, we'll add the DEBUG_BREAK macro to ``HelperFunctions.h``:

.. literalinclude:: ../Common/HelperFunctions.h
	:language: cpp
	:start-at: // Debugbreak
	:end-at: #endif

This defines the macro ``DEBUG_BREAK``, according to which platform we're building for. This macro will
stop execution of your program when an error occurs, so you can see where it happened and fix it.
We use this macro in the ``OpenXRMessageCallbackFunction()`` function, which we will discuss in :doc:`Chapter 5.2. <5-extensions>` 

In ``OpenXRHelper.h``, add:

.. literalinclude:: ../Common/OpenXRHelper.h
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Helper_Functions0
	:end-before: XR_DOCS_TAG_END_Helper_Functions0

and in ``Helperfunctions.h``, add:

.. literalinclude:: ../Common/HelperFunctions.h
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Helper_Functions1
	:end-before: XR_DOCS_TAG_END_Helper_Functions1

This defines the macro ``OPENXR_CHECK``. Many OpenXR functions return a ``XrResult``. This macro will check if the call has failed and logs a message to stdout. This can be modified to suit your needs. There are three additional functions ``GetXRErrorString()``, ``IsStringInVector()`` and ``BitwiseCheck()``, which are just simple wrappers over commonly used code.

We can add the ``GraphicsAPIs.h`` header to include in turn all the Graphics API code along with the ``OpenXRHelper.h`` and ``HelperFunctions.h`` files. You can also include ``OpenXRDebugUtils.h`` to help with set up of ``XrDebugUtilsMessengerEXT``.

Now we will define the main class of the application. It's just a stub for now, with an empty ``Run()`` method:

.. code-block:: cpp

	class OpenXRTutorialChapter2 {
	public:
		OpenXRTutorialChapter2() = default;
		~OpenXRTutorialChapter2() = default;

		void Run()
		{
		}
	};

Finally, let's add the main function for the application. It will look slightly different, depending on your
chosen platform. We first create a 'pseudo-main function' called ``OpenXRTutorial_Main()``, in which we create an instance of our ``OpenXRTutorial_Ch2_1`` class, and call the ``Run()``method.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: void OpenXRTutorial_Main()
	:end-at: }

Then, we create the actual platform specific main function (our entry point to the application), which will call ``OpenXRTutorial_Main()``:

.. container:: windows linux
	:name: windows-linux-id-1

	.. rubric:: Windows and Linux

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_main_WIN32___linux__
		:end-before: XR_DOCS_TAG_END_main_WIN32___linux__

.. container:: android
	:name: android-id-1
	
	.. rubric:: Android
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_android_main___ANDROID__
		:end-before: XR_DOCS_TAG_END_android_main___ANDROID__

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_Android_System_Functionality
		:end-before: XR_DOCS_TAG_END_Android_System_Functionality

	Before we can use OpenXR for Android, we need to initialise the loader based the application's context and virtual machine. We retrieve the function pointer to ``xrInitializeLoaderKHR``, and with the ``XrLoaderInitInfoAndroidKHR`` filled out call that function to initialise OpenXR for our use. At this point, we also attach the current thread to the Java Virtual Machine. We assign our ``AndroidAppState`` static member and our ``AndroidAppHandleCmd()`` static method to the ``android_app *`` and save it to a static member in the class.

.. container:: windows
	:name: windows-id-1

	.. rubric:: Windows

	Now launch CMake GUI, and point the "Where is the source code" box to the root of your solution *workspace* directory,
	where your original ``CMakeLists.txt`` is located. Point the "Where to build the binaries" box to a subdirectory called ``build``,
	click Configure, "Yes" to create the build folder, and "OK" to accept the default Generator.

	.. figure:: cmake-tutorial2-1.png
		:alt: Select 
		:align: left

	Now, we must tell the tutorial project where to find the OpenXR-SDK, which we built in Section 1.
	Click on the variable ``OPENXR_DIR`` and type in or browse to the location of the OpenXR-SDK repository you downloaded
	previously, the click "Generate". When the projects have been generated, open your new project, by clicking
	"Open Project", or by finding the file Tutorial.sln in your build folder and open it.

.. container:: linux
	:name: linux-id-1

	.. rubric:: Linux

	You now have three files, laid out as follows:

	.. figure:: linux-vscode-initial-files.png
		:alt: Initial files in VS Code 

	Having installed the CMake extension for Visual Studio Code, you can now right-click on the main ``CMakeLists.txt`` file (the one in the root *workspace* folder) and select "Configure All Projects":

	.. figure:: linux-vscode-cmake-configure.png
		:alt: Select

	Now the CMake panel will be available by clicking its icon on the leftmost panel. Hover your mouse over the "Project Outline"
	and click the three dots at the right. An option to "Edit CMake Cache" will appear, click this and you'll see the Cache Editor
	page.

	.. figure:: linux-vscode-cmake-more-actions.png
		:alt: Select
	

	.. figure:: linux-vscode-cmake-cache.png
		:alt: Select 
		

	NOTE: It's possible to use CMake GUI in Linux, but using Visual Studio Code's integrated tools works better with Visual Studio Code.

	In the CMake Cache editor, find ``OPENXR_DIR`` and enter the directory where you downloaded OpenXR-SDK in Chapter 1.
	We can now select "Configure and Build All" from the right-click menu of the main ``CMakeLists.txt`` file.

.. container:: windows
	:name: windows-id-1

	.. rubric:: Windows

	You can now build and run your program. It should compile and link with no errors or warnings.

.. container:: linux
	:name: linux-id-1

	.. rubric:: Linux

	To enable debugging, select the Run/Debug panel in Visual Studio Code. You will now need to create a debugging configuration.
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

.. container:: android
	:name: android-id-1

	With all the source and build systems set up, we can now build the Android project. In upper right of Android Studio, you should find the toolbar below. Click the green hammer icon to build the project, if all is successful you should see "BUILD SUCCESSFUL in [...]s" in the Build Output window.

	Next to the green hammer icon is the Run/Debug configuration dropdown menu. If that isn't populated, create a configuration called app.

	Turn on and connect your Android device. Set up any requirments for USB debugging and adb. You device should appear in the dropdown. Here, I am using a Oculus Quest 2.

	.. figure:: android-studio-build-run-toolbar.png
		:alt: Build/Run Toolbar
	
	To debug/run the application click the green bug icon.

Now that we have a basic application up and running with the OpenXR header files and libraries, we can start to set up the core aspects of OpenXR. As a modern Khronos API, the OpenXR is heavily influcencd by the Vulkan API. So those who are familiar with the style of the Vulkan API will find OpenXR easy to follow.

Creating an XrInstance / xrGetSystem
------------------------------------

Firstly, add to the ``OpenXRTutorial`` class the methods: ``CreateInstance()``, ``GetInstanceProperties()``, ``GetSystemID()`` and ``DestroyInstance()``. Update ``OpenXRTutorial::Run()`` to call those methods in that order and add to the class in a private section the following members.

.. code-block:: cpp
	
	class OpenXRTutorialChapter2 {
	public:
		OpenXRTutorialChapter2() = default;
		~OpenXRTutorialChapter2() = default;
	
		void Run()
		{
			CreateInstance();

			GetInstanceProperties();
			GetSystemID();

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

		void GetSystemID()
		{
		}
	
	private:
		XrInstance instance = {};
		std::vector<const char *> activeAPILayers = {};
		std::vector<const char *> activeInstanceExtensions = {};
		std::vector<std::string> apiLayers = {};
		std::vector<std::string> instanceExtensions = {};

		XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemId systemID = {};
	}

	
XrInstance
^^^^^^^^^^

The ``XrInstance`` is the foundational object that we need to create first. The ``XrInstance`` encompasses the application setup state, OpenXR API version and any layers and extensions. So inside the ``CreateInstance()`` method, we will first look at the ``XrApplicationInfo``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrApplicationInfo
	:end-before: XR_DOCS_TAG_END_XrApplicationInfo
	:dedent: 8

This structure allows you specify both the name and the version for your application and engine. These members are solely for your use as the application developer. The main member here is the ``XrApplicationInfo::apiVersion``. Here we use the ``XR_CURRENT_API_VERSION`` macro to specific the OpenXR version that we want to run. Also note here the use of ``strcpy()`` to set the applicationName and engineName. If you look at ``XrApplicationInfo::applicationName`` and ``XrApplicationInfo::engineName`` members, they are of type ``char[]``, hence you must copy your string into that ``char[]`` and you must also by aware of the allowable length.

Similar to Vulkan, OpenXR allows applications to extend functionality past what is provided by the core specification. The functionality could be hardware/vendor specific. Most vital of course is which Graphics API to use with OpenXR. OpenXR supports D3D11, D3D12, Vulkan, OpenGL and OpenGL ES. Due the extensible nature of specification, it allows newer Graphics APIs and hardware functionality to be added with ease.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_instanceExtensions
	:end-before: XR_DOCS_TAG_END_instanceExtensions
	:dedent: 12

Here, we store in a ``std::vector<std::string>`` the extension names that we would like to use. ``XR_EXT_DEBUG_UTILS_EXTENSION_NAME`` is a macro of a string defined in ``openxr.h``. The XR_EXT_debug_utils is extension that checks the validity of calls made to OpenXR, and can use a call back function to handle any raised errors. We will explore this extension more in :doc:`Chapter 5.2. <5-extensions>` Depending on which ``XR_USE_GRAPHICS_API_...`` macro that you have defined, this code will add the relevant extension.

Not all API layers and extensions are available to use, so we much check which ones can use. We will use ``xrEnumerateApiLayerProperties()`` and ``xrEnumerateInstanceExtensionProperties()`` to check which ones the runtime can provide.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_find_apiLayer_extension
	:end-before: XR_DOCS_TAG_END_find_apiLayer_extension
	:dedent: 8

These functions are called twice. The first time is to get the count of the API layers or extensions and the second is to fill out the array of structures. Before the second call, we need set ``XrApiLayerProperties::type`` or ``XrExtensionProperties::type`` to the correct value, so that the second call can correctly fill out the data. After we have enumerated the API layers and extensions, we use a nested loop to check to see whether an API layers or extensions is availble and add it to the activeAPILayers and/or activeInstanceExtensions respectively. Note the activeAPILayers and activeInstanceExtensions are of type ``std::vector<const char *>``. This will help us when fill out the next structure ``XrInstanceCreateInfo``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrInstanceCreateInfo
	:end-before: XR_DOCS_TAG_END_XrInstanceCreateInfo
	:dedent: 8

This section is fairly simple, as we now just collect data from before and assign them to members in the ``XrInstanceCreateInfo`` structure. Finally, we get to call ``xrCreateInstance()`` where we take pointers to the stack ``XrInstanceCreateInfo`` and ``XrInstance`` objects. If the function succeeded, the result will be ``XR_SUCCESS`` and ``XrInstance`` will be non-null.

At the end of the application, we should destroy the ``XrInstance``. This is simple done with the function ``xrDestroyInstance()``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: void DestroyInstance()
	:end-at: }
	:dedent: 4

Whilst we have an ``XrInstance``, let's check its properties. We fill out the type and next members of the structure ``XrInstanceProperties`` and pass it along with the ``XrInstance`` to ``xrGetInstanceProperties()``. This function will fill out the rest of that structure for us to use. Here, we simply log to stdout the runtime's name, and with the use of the ``XR_VERSION_MAJOR``, ``XR_VERSION_MINOR`` and ``XR_VERSION_PATCH`` macros, we parse and log the runtime version.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GetInstanceProperties
	:end-before: XR_DOCS_TAG_END_GetInstanceProperties
	:dedent: 4

XrSystemId
^^^^^^^^^^

The next object that we want to get is the ``XrSystemId``. OpenXR 'separates the concept of physical systems of XR devices from the logical objects that applications interact with directly. A system represents a collection of related devices in the runtime, often made up of several individual hardware components working together to enable XR experiences'. 
`OpenXR Specification 5. System <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#system>`_. 

So, a ``XrSystemId`` could represent VR headset and a pair of contollers, or perhaps mobile device with video pass-through for AR. So we need to decide what type of ``XrFormFactor`` we are wanting to use, as some runtimes support multiple form factors. Here, we are selecting ``XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY``, which is initialised in the class, for a Meta Quest or Pico Neo. OpenXR currently offers two option for the ``XrFormFactor``.

.. literalinclude:: ../build/openxr/include/openxr/openxr.h
	:language: cpp
	:start-at: typedef enum XrFormFactor {
	:end-at: } XrFormFactor;

We fill out the ``XrSystemGetInfo`` structure as desired and pass it as a pointer along with the ``XrInstance`` and a pointer to the ``XrSystemId`` to ``xrGetSystem()``. If successful, we should now have a non-null ``XrSystemId``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_GetSystemID
	:end-before: XR_DOCS_TAG_END_GetSystemID
	:dedent: 4

We can now also get the system's properties. We partially fill out a ``XrSystemProperties`` structure and pass it as a pointer along with the ``XrInstance`` and the ``XrSystemId`` to ``xrGetSystemProperties()``. This function will fill out the rest of the ``XrSystemProperties`` structure; detailing the vendor's ID, system's name and the system's graphics and tracking properties.

.. literalinclude:: ../build/openxr/include/openxr/openxr.h
	:language: cpp
	:start-at: typedef struct XrSystemGraphicsProperties {
	:end-at: } XrSystemProperties;

Creating an XrSession
---------------------

The next major component of OpenXR that needs to be created in an ``XrSession``. An ``XrSession`` encapulates the state of application from the perspective of OpenXR. When an ``XrSession`` is created, it starts in the ``XR_SESSION_STATE_IDLE``. It is upto the runtime to provide any updates to the ``XrSessionState`` and for the appliaction to query them and react to them. We will explore this in :doc:`Chapter 2.3. <2-Polling the Event Loop>`

For now, we are just going to create an ``XrSession``. At this point, you'll need to select which Graphics API you wish to use. Only one Graphics API can be used with an ``XrSession``. This tutorial demostrates how to use D3D11, D3D12, OpenGL, OpenGL ES and Vulkan in conjunction with OpenXR for the purpose of rendering graphics to the provided views. Ultimately, you will most likely be bringing your own rendering solution to this tutorial, therefore the code examples provided for the Graphics APIs are `placeholders` for you own code base; demostrating in this sub-chapter what objects are needed from your Graphics API in order to create an ``XrSession``. This tutorial uses polymorphic classes ``GraphicsAPI_...`` which derives from ``GraphicsAPI``. There are both compile and runtime checks to select the requested Graphics API, and we construct an apropriate derived classes throught the use of ``std::unique_ptr<>``. 

Update the Constructor and ``Run()`` method as shown and add the following members:
``CheckGraphicsAPI_TypeIsValidForPlatform()`` is declared in ``GraphicsAPI.h``.

.. code-block:: cpp

	class OpenXRTutorialChapter2 {
	public:
		OpenXRTutorialChapter2(GraphicsAPI_Type api)
			: apiType(api) {
			if(!CheckGraphicsAPI_TypeIsValidForPlatform(apiType)) {
				std::cout << "ERROR: The provided Graphics API is not valid for this platform." << std::endl;
				DEBUG_BREAK;
			}
		}
		
		// [...]

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

		// [...]

	private:
		// [...]

		GraphicsAPI_Type apiType = UNKNOWN;
		std::unique_ptr<GraphicsAPI> graphicsAPI = nullptr;

		XrSession session = {};
	}

XrSession
^^^^^^^^^

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateDestroySession
	:end-before: XR_DOCS_TAG_END_CreateDestroySession

Above is the code for creating and destroying an ``XrSession``. ``xrDestroySession()`` will destroy the ``XrSession`` when we have finished and shutting down the application. ``xrCreateSession()`` takes the ``XrInstance``, ``XrSessionCreateInfo`` and ``XrSession`` return object. If the function call was successful, ``xrCreateSession()`` will return ``XR_SUCCESS`` and ``XrSession`` will be non-null. The ``XrSessionCreateInfo`` structure is deceptively simple. ``XrSessionCreateInfo::createFlags`` and ``XrSessionCreateInfo::systemId`` are easily filled in, but we need to specify which Graphics APIs we wish to use. This is achieved via the use of the ``XrSessionCreateInfo::next`` void pointer. Following the Vulkan API's style of extensibility, structures for creating objects can be extended to enable extra functionality. In our case, the extension is required and thus ``XrSessionCreateInfo::next`` can not be a nullptr. That pointer must point to 'exactly one graphics API binding structure (a structure whose name begins with "XrGraphicsBinding")' (`XrSessionCreateInfo(3) Manual Page <https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrSessionCreateInfo.html>`_).

GraphicsAPI
^^^^^^^^^^^

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11
	
	.. literalinclude:: ../Common/GraphicsAPI_D3D11.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D11

	Above is the minimum code needed to create a suitable ``ID3D11Device *``. First, we need to get the function pointer for ``xrGetD3D11GraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsD3D11KHR`` structure.
	
	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsD3D11KHR {
		:end-at: } XrGraphicsRequirementsD3D11KHR;

	From this structure, we use the ``adapterLuid`` to find the approiate ``IDXGIAdapter *``. We create a ``IDXGIFactory1 *`` and then call ``IDXGIFactory1::EnumAdapters()`` and ``IDXGIAdapter::GetDesc()`` to get the ``DXGI_ADAPTER_DESC``, so that we can compare the ``adapterLuid`` values.

	Finally, we call ``D3D11CreateDevice`` with found adapter and the ``minFeatureLevel`` from ``XrGraphicsRequirementsD3D11KHR``, if successful the function will return ``S_OK`` and ``ID3D11Device *`` is non-null.

	.. literalinclude:: ../Common/GraphicsAPI_D3D11.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D11_GetGraphicsBinding

	Here, we simply fill out the ``XrGraphicsBindingD3D11KHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``.

.. container:: d3d12
	:name: d3d12-id-1
	
	.. rubric:: DirectX 12

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D12

	Above is the minimum code needed to create a suitable ``ID3D12Device *``  and ``ID3D12CommandQueue *``. First, we need to get the function pointer for ``xrGetD3D12GraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsD3D12KHR`` structure.

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsD3D12KHR {
		:end-at: } XrGraphicsRequirementsD3D12KHR;

	From this structure, we use the ``adapterLuid`` to find the approiate ``IDXGIAdapter1 *``. We create a ``IDXGIFactory4 *`` and then call ``IDXGIFactory4::EnumAdapters1()`` and ``IDXGIAdapter1::GetDesc()`` to get the ``DXGI_ADAPTER_DESC``, so that we can compare the ``adapterLuid`` values.

	Finally, we call ``D3D12CreateDevice`` with found adapter and the ``minFeatureLevel`` from ``XrGraphicsRequirementsD3D12KHR``, if successful the function will return ``S_OK`` and ``ID3D12Device *`` is non-null. Next, we create a simple a ``ID3D12CommandQueue *`` of type ``D3D12_COMMAND_LIST_TYPE_DIRECT``.

	.. literalinclude:: ../Common/GraphicsAPI_D3D12.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_D3D12_GetGraphicsBinding

	Here, we simply fill out the ``XrGraphicsBindingD3D12KHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``.
	
.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL
	
	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL

	Above is the minimum code needed to create a suitable OpenGL context. First, we need to get the function pointer for ``xrGetOpenGLGraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsOpenGLKHR`` structure. 

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsOpenGLKHR {
		:end-at: } XrGraphicsRequirementsOpenGLKHR;
	
	In this tutorial, we are using the 'gfxwrapper' for the OpenGL API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. Originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ and `OpenGL ES 3.1 <https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.withchanges.pdf>`_ specifications.

	Here, we call ``ksGpuWindow_Create()`` and pass the required parameters to setup the OpenGL context. Next, we query the OpenGL version with ``glGetIntegerv()`` with ``GL_MAJOR_VERSION`` and ``GL_MINOR_VERSION``. With these values, we can contruct a ``XrVersion`` value to compare with ``XrGraphicsRequirementsOpenGLKHR::minApiVersionSupported``.

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_GetGraphicsBinding

	Because OpenGL is very closely integrated with the platform's windowing system. We have different ``XrGraphicsBindingOpenGL[...]KHR`` structures - one per platform. At present, there are four: ``XrGraphicsBindingOpenGLXcbKHR``, ``XrGraphicsBindingOpenGLXlibKHR``, ``XrGraphicsBindingOpenGLWaylandKHR`` and ``XrGraphicsBindingOpenGLWin32KHR``. Depending on the platform, we fill out the relevant structure and return a pointer to that class member, which will be assigned to ``XrSessionCreateInfo::next``.
	
.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES

	Above is the minimum code needed to create a suitable OpenGL ES context. First, we need to get the function pointer for ``xrGetOpenGLESGraphicsRequirementsKHR``, which went called fills out the ``XrGraphicsRequirementsOpenGLESKHR`` structure. 

	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsOpenGLESKHR {
		:end-at: } XrGraphicsRequirementsOpenGLESKHR;

	In this tutorial, we are using the 'gfxwrapper' for the OpenGL ES API found as a part of the `OpenXR-SDK-Source <https://github.com/KhronosGroup/OpenXR-SDK-Source>`_ reposity under ``src/common/``. Originally developed by Oculus VR, LLC and The Brenwill Workshop Ltd.; this wrapper is written against the `OpenGL 4.3 <https://registry.khronos.org/OpenGL/specs/gl/glspec43.core.pdf>`_ and `OpenGL ES 3.1 <https://registry.khronos.org/OpenGL/specs/es/3.1/es_spec_3.1.withchanges.pdf>`_ specifications.

	Here, we call ``ksGpuWindow_Create()`` and pass the required parameters to setup the OpenGL ES context. Next, we query the OpenGL ES version with ``glGetIntegerv()`` with ``GL_MAJOR_VERSION`` and ``GL_MINOR_VERSION``. With these values, we can contruct a ``XrVersion`` value to compare with ``XrGraphicsRequirementsOpenGLESKHR::minApiVersionSupported``.

	.. literalinclude:: ../Common/GraphicsAPI_OpenGL_ES.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES_GetGraphicsBinding

	Here, we simply fill out the ``XrGraphicsBindingOpenGLESAndroidKHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``. Note: This ``XrGraphicsBinding...`` structure differs from the others as it specifically tailored to the Android platform.
	
.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan
	
	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan

	Above is the minimum code needed to create a suitable Vulkan Instance and Device. First, we call helper method that loads in the pointers for the following functions:

		* ``xrGetVulkanGraphicsRequirementsKHR``: Used to fill out a ``XrGraphicsRequirementsVulkanKHR`` structure containing the minimum and maximum supported API version.
		* ``xrGetVulkanInstanceExtensionsKHR``: To retrieve a list of required ``VkInstance`` extensions.
		* ``xrGetVulkanDeviceExtensionsKHR``: To retrieve a list of required ``VkDevice`` extensions.
		* ``xrGetVulkanGraphicsDeviceKHR``:  To get the ``VkPhysicalDevice`` requested by OpenXR.

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_LoadPFN_XrFunctions
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_LoadPFN_XrFunctions

	We can now call ``xrGetVulkanGraphicsRequirementsKHR()`` and fill out the ``XrGraphicsRequirementsVulkanKHR`` structure. 
	
	.. literalinclude:: ../build/openxr/include/openxr/openxr_platform.h
		:language: cpp
		:start-at: typedef struct XrGraphicsRequirementsVulkanKHR {
		:end-at: } XrGraphicsRequirementsVulkanKHR;

	Next, we fill out a ``VkApplicationInfo`` where we assign ``VkApplicationInfo::apiVersion`` the value in ``XrGraphicsRequirementsVulkanKHR::minApiVersionSupported``. We can now enumerate the Instance extensions and fill out array of structures. We use nested for-loops to find all the requested extensions push them back into a ``std::vector<const char *>`` called ``activeInstanceExtensions``.
	``GetInstanceExtensionsForOpenXR()`` is another helper method.

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR

	``xrGetVulkanInstanceExtensionsKHR()`` is called twice, first to get the size of the ``char`` buffer and the second to fill in the data. We use ``std::stringstream`` and ``std::getline()`` with a deliminator of ``' '`` to break up the string and copy that subcontexts to an element in a ``std::vector<std::string>``, which we use when setting the instance extensions.

	We fill out the ``VkInstanceCreateInfo`` structure and call ``vkCreateInstance()``, which if successful will return ``VK_SUCCESS`` and the ``VkInstance`` will be non-null.

	Next, we enumerate the ``VkPhysicalDevice`` s. Once we have array of all the physical devices in the system, we can call ``xrGetVulkanGraphicsDeviceKHR()`` to get the ``VkPhysicalDevice`` that OpenXR has requested. We can check the requested physical device against the array of physical devices to select the correct one.

	With a ``VkPhysicalDevice`` selected, we can now create a ``VkDevice``. We get the ``VkQueueFamilyProperties`` from the ``VkPhysicalDevice`` and fill an array of ``VkDeviceQueueCreateInfo``. We find the first queue family that supports graphics operations and select its index as the ``queueFamilyIndex`` and we also select the first queue in that family too. We can now enumerate the Device extensions and fill out array of structures. We use nested for-loops to find all the requested extensions push them back into a ``std::vector<const char *>`` called ``activeDeviceExtensions``.
	``GetDeviceExtensionsForOpenXR()`` is another helper method.

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR

	Like with ``xrGetVulkanInstanceExtensionsKHR()``, ``xrGetVulkanDeviceExtensionsKHR()`` is called twice, first to get the size of the ``char`` buffer and the second to fill in the data. We use ``std::stringstream`` and ``std::getline()`` with a deliminator of ``' '`` to break up the string and copy that subcontexts into an element in a ``std::vector<std::string>``, which we use when setting the device extensions.

	Finally, we get the ``VkPhysicalDeviceFeatures`` by calling ``vkGetPhysicalDeviceFeatures()`` and we can fill in the ``VkDeviceCreateInfo``. We call ``vkCreateDevice()``, which if successful will return ``VK_SUCCESS`` and the ``VkDevice`` will be non-null. 

	.. literalinclude:: ../Common/GraphicsAPI_Vulkan.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetGraphicsBinding
		:end-before: XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetGraphicsBinding

	Here, we simply fill out the ``XrGraphicsBindingVulkanKHR`` structure and return a pointer to the class member, which will be assigned to ``XrSessionCreateInfo::next``.

Polling the Event Loop
----------------------

OpenXR uses an event based system to describes changes with the XR system. It's the application's responsibility to poll these events and react to them. The polling of events is done by the function ``xrPollEvent()``. The application should continually call this function throughout its lifetime. Within a single XR frame, the application should continuously call ``xrPollEvent()`` until the internal event queue is 'drained'; multiple events can occurs across the XR frame and the application needs to handle and respond to each accordingly.

Firstly, we will update the class to add the new methods and members.

.. code-block:: cpp

	class OpenXRTutorialChapter2 {
	public:
		// [...]

		void Run() {
			CreateInstance();
			CreateDebugMessenger();
		
			GetInstanceProperties();
			GetSystemID();
			CreateSession();
		
			while (applicationRunning) {
				PollSystemEvents();
				PollEvents();
				if (sessionRunning) {
					// Draw Frame.
				}
			}
		
			DestroySession();
			DestroyDebugMessenger();
			DestroyInstance();
		}
		// [...]

	private:
		// [...]

		XrViewConfigurationType viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		// [...]

		XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
		bool applicationRunning = true;
		bool sessionRunning = false;
	}

xrPollEvent()
^^^^^^^^^^^^^

Next, we will define the ``PollEvents()`` method. Here, we use a do-while loop to the check the result of ``xrPollEvent()`` - whilst that function returns ``XR_SUCCESS``, there are events for us to process. ``xrPollEvent()`` will fill in the ``XrEventDataBuffer`` structure that we pass to the function call. ``xrPollEvent()`` will update the member variable ``type`` and from this we can use a switch statement to select the appropriate code path. Depending on the updated type, we can use a ``reinterpret_cast<>()`` to get the actual data that ``xrPollEvent()`` returned.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollEvents
	:end-before: XR_DOCS_TAG_END_PollEvents

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

As described in the table above, most event are transparent in their intensions and how the application should react to them. For the ``XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING`` state, the application may want to try re-creating the ``XrInstance`` in a loop, after the specified ``lossTime``, until it can create a new instance successfully. ``XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED`` and ``XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING`` are used for updating how the user interacts with the application and whether a new space change has been detected respectively.

XrSessionState
^^^^^^^^^^^^^^

The final one, ``XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED``, is what we will focus on for the rest of this chapter. There are currently nine valid ``XrSessionState`` s described:

.. literalinclude:: ../build/openxr/include/openxr/openxr.h
		:language: cpp
		:start-at: typedef enum XrSessionState {
		:end-at: } XrSessionState;

+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| Event Type                    | Description                                                                                                             |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_UNKNOWN      | This is an unknown, pseudo-default state and should not be returned by the runtime.                                     |
+-------------------------------+-------------------------------------------------------------------------------------------------------------------------+
| XR_SESSION_STATE_IDLE         | This is an inital state after creating or after ending the session.                                                     |
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

.. figure:: openxr-session-life-cycle.svg
	:alt: OpenXR Session Life-Cycle
	:align: center
	:width: 99%

xrBeginSession() and xrEndSession()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the ``XrSessionState`` is ``XR_SESSION_STATE_READY``, the application can call ``xrBeginSession()``.
In the ``XrSessionBeginInfo`` structure, we assign to ``XrSessionBeginInfo::primaryViewConfigurationType`` the ``viewConfiguration`` from the class, which in our case is ``XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO``. This specifies the view configuration of the form factor's primary display - For Head Mounted Displays, it is two views (one per eye).

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {

If the ``XrSessionState`` is ``XR_SESSION_STATE_STOPPING``, the application should call ``xrEndSession()``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {