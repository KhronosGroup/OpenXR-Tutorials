##########
⚙️ 2 Setup
##########

Select your platform, as the instructions are different depending on your selection.

.. raw:: html
	:file: platforms.html

*************
Project Setup
*************

CMake and Project Files
=======================

.. container:: windows
	:name: windows-id-1

	For the Microsoft Windows OpenXR project, we'll use CMake to create the solution and project files for Visual Studio.
	First, create a directory where the code will go, we'll call this the *workspace* directory.

.. container:: linux
	:name: linux-id-1

	You are free to use any code editor and/or compiler; this tutorial will use Visual Studio Code.
	For the Linux OpenXR project, we'll use CMake alongside Visual Studio Code to build the project.
	First, create a directory where the code will go, we'll call this the *workspace* directory. Open Visual Studio Code and from the File menu, select "Open Folder..."

	.. figure:: linux-vscode-open-folder.png
		:alt: The File menu of Visual Studio Code is shown, with the command "Open Folder..." selected.
		:scale: 55%

	Select your *workspace* folder, which is currently empty.
	
	If you haven't previously done so, install the CMake extension for Visual Studio Code.
	
.. container:: windows linux
	:name: windows-linux-id-1

	Now, create a text file in the *workspace* folder called ``CMakeLists.txt`` and in it, put the following code:

	.. literalinclude:: ../CMakeLists.txt
		:language: cmake
		:emphasize-lines: 11

	Here, we specify the CMake version, project name and the configurations types, which we limit to just Debug and Release.
	You can use ``XR_RUNTIME_JSON`` to specify an optional runtime. The one provided by your hardware vendor will be automatically used, though this feature is helpful for debugging on different runtimes.
	Finally, we specify CMake to continue the build into the Chapter2 directory with ``add_subdirectory()``.

	Let's create that ``/Chapter2`` folder, and in it create another ``CMakeLists.txt`` file.
	This one contains the following code:

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: cmake_minimum_required
		:end-at: )

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: # For FetchContent_Declare() and FetchContent_MakeAvailable()
		:end-before: # Files

	After setting our CMake version, we include ``FetchContent`` and use it to get the OpenXR-SDK from Khronos's GitHub page.

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: # Files
		:end-before: if (ANDROID) # Android

	Here, we include all the files needed for our project. First, we'll create our source file called ``main.cpp`` in the ``/Chapter2`` directory. All files with ``../Common/*.*`` are available to download from this tutorial website. Below are the links and discussion of their usage within this tutorial and with OpenXR.

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-after: else() # Windows / Linux
		:end-before: endif() # EOF
		:dedent: 4

	Now, we set up the project file by adding an executable with the ``${SOURCES}`` and ``${HEADERS}``. Next, we optionally set a the ``XR_RUNTIME_JSON`` in the debugger environment. We add the ``../Common`` folder as an include directory and link ``openxr_loader`` from ``FetchContent``. We don't have to add OpenXR as a include directory; as we specified them to be located in the CMake Build directory under ``openxr/`` when using ``FetchContent``, so we just include the OpenXR headers from the CMake Build directory.
	
	Next, is the include directories and linkage for the graphics APIs.
	For Microsoft Windows, we link ``d3d11.lib``, ``d3d12.lib``, ``dxgi.lib`` and ``d3dcompiler.lib``, so that we can use Direct3D 11, Direct3D 12, the DirectX Graphics Infrastructure and the D3D Compiler. The headers are automatically included as part of the Visual Studio project.
	If you have the Vulkan SDK installed, we try to find that package, add the include directories and link the Vulkan libraries.
	As a default, we add gfxwrapper for OpenGL, we add the include directories and link the gfxwrapper libraries from the OpenXR-SDK.

	That's all we need for CMake, we need for this project. 

.. container:: android
	:name: android-id-1

	Here, We'll show how to hand build an Android Studio project that runs a C++ Native Activity.
	Fisrt, we will create a *workspace* folder and in that folder create that ``/Chapter2`` folder,
	Open Android Studio, select New Project and choose an Empty Activity. Set the Name to 'OpenXR Tutorial Chapter 2', the Package name to 'com.simul.OpenXRTutorialChapter2' and save location to that ``/Chapter2`` folder. The language can be ignored here as we are using C++, and we can set the Minimum SDK to API 24: Android 7.0(Nougat). Complete the set up.

	.. figure:: android-studio-newproject.png
		:alt: Android Studio - New Project - Empty Activity.
		:align: left

	.. rubric:: CMake

	With the Android Studio project now set up, we need to modify some of the files and folders so as to set up the project to support the C++ Native Activity.
	Under the ``app`` folder, you can delete the ``libs`` folder, and under the ``app/src`` you can also delete the ``androidTest`` and ``test`` folders. Finally under ``app/src/main``, delete the ``java`` folder and add a ``cpp`` folder. Under the ``app/src/main/res``, delete the ``values-night`` and ``xml`` folders. Under the ``values`` modify ``colors.xml``.

	.. rubric:: colors.xml

	.. literalinclude:: ../Chapter2/app/src/main/res/values/colors.xml
		:language: xml

	Delete ``themes.xml`` and add ``styles.xml`` as shown.

	.. rubric:: styles.xml

	.. literalinclude:: ../Chapter2/app/src/main/res/values/styles.xml
		:language: xml

	Create a ``CMakeLists.txt`` in the ``/Chapter2`` directory for compatiblity with other platforms. We will use this file to specific how our Native C++ code will be built. This ``CMakeLists.txt`` file will be invoked by Android Studio's Gradle build system and we will point Gradle to this CMake file. 

	.. rubric:: CMakeLists.txt

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: cmake_minimum_required
		:end-at: # Needed for Android

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: # For FetchContent_Declare() and FetchContent_MakeAvailable()
		:end-before: # Files

	After setting our CMake version to 3.22.1 and the project's name, we include ``FetchContent`` and use it to get the OpenXR-SDK from Khronos's GitHub page.

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-at: # Files
		:end-before: if (ANDROID) # Android

	Here, we include all the files needed for our project. First, we'll create our source file called ``main.cpp`` in the ``/Chapter2`` directory. All files with ``../Common/*.*`` are available to download from this tutorial website. Below are the links and discussion of their usage within this tutorial and with OpenXR.

	.. literalinclude:: ../Chapter2/CMakeLists.txt
		:language: cmake
		:start-after: if (ANDROID) # Android
		:end-before: else() # Windows / Linux
		:dedent: 4
	
	Now, we can set things up by adding a shared library with the ``${SOURCES}`` and ``${HEADERS}``. We add the ``../Common`` folder as an include directory too. Just above that we need to set the ``CMAKE_SHARED_LINKER_FLAGS`` so that ``ANativeActivity_onCreate()`` is exported for the Java Virtual Machine to call. This is used by a static library called ``native_app_glue``, which allows us to interface between the Java Virtual Machine and our C++ code. Ultimately, it allows us to use the ``void android_main(struct android_app*)`` entry point.  We add ``native_app_glue`` to our project by including ``AndroidNdkModules`` and calling ``android_ndk_import_module_native_app_glue()``. 

	Next, we find the Vulkan library in the NDK and include the directory to the Android Vulkan headers. We also add a static library called ``openxr-gfxwrapper``, which will allow us to use OpenGL ES. We compile the library the C and header file in ``${openxr_SOURCE_DIR}/src/common/gfxwrapper_opengl.*`` and add this ``${openxr_SOURCE_DIR}/external/include`` as an include directory. Next, we find the ``GLESv3`` and ``EGL`` libraries and link them to ``openxr-gfxwrapper``. We add the ``${openxr_SOURCE_DIR}/src/common`` and ``${openxr_SOURCE_DIR}/external/include`` folder as include directories to ``OpenXRTutorialChapter2`` as well.
	
	Finally we link the ``android``, ``native_app_glue``, ``openxr_loader``, ``vulkan`` and ``openxr-gfxwrapper`` libraries to our ``OpenXRTutorialChapter2`` library. Our ``libOpenXRTutorialChapter2 .so`` will packaged inside our .apk along with any shared libraries that we have linked.

	.. rubric:: AndroidManifest.xml

	.. literalinclude:: ../Chapter2/app/src/main/AndroidManifest.xml
		:language: xml

	We need to modify our ``AndroidManifest.xml`` file to allow our application to use the XR hardware:

	* First, we remove ``xmlns:tools="http://schemas.android.com/tools"`` and replace it  with ``package="com.simul.OpenXRTutorialChapter2" android:versionCode="1" android:versionName="1.0"``.
	* Next, we add ``<uses-feature android:name="android.hardware.vr.headtracking" android:required="true" />`` to specify that the application is using VR headtracking. 
	* In the ``android`` section, update ``android:allowBackup`` to ``"false"``, remove ``android:dataExtractionRules`` and update ``android:fullBackupContent`` to ``"false"``. Remove ``android:supportsRtl``, ``android:theme`` and ``tools:targetApi``, and replace them with ``android:hasCode="false"``. This one state that there is no Java or Kotlin code in the application.
	* We also need to modify this file to tell Android to run this application as a Native Activity. We set ``android:name`` to ``"android.app.NativeActivity"`` and update ``android:configChanges`` to ``"orientation|keyboardHidden"`` so as to not close the activity on those changes. Remove ``android:exported="true"`` and add ``android:debuggable="true"`` to allow debugging. 
	* Next under the ``meta-data`` section, we add these values: ``android:name="android.app.lib_name"`` and ``android:value="OpenXRTutorialChapter2"``, where ``android:value`` is name of the library we created in the CMakeLists, thus pointing our NativeActivity to the correct library.
	* Also, set this in the ``intent-filer`` section: ``<category android:name="android.intent.category.DEFAULT" />``.
	* Finally, We need to tell the app that it should take over rendering when active, rather than appearing in a window. Set ``<category android:name="org.khronos.openxr.intent.category.IMMERSIVE_HMD" />``. Note: not all devices yet support this ``category``. For example, for Oculus Quest devices you will need ``<category android:name="com.oculus.intent.category.VR" />`` for the same purpose.

	.. rubric:: Gradle

	.. literalinclude:: ../Chapter2/app/build.gradle
		:language: groovy
	
	Now, we can config our ``build.gradle`` file in the``app`` folder to remove any references to Java, Kotlin and to testing. We also want to specify the the we are doing a ``externalNativeBuild``  with CMake, the CMake version and the location of the ``CMakeLists.txt`` that we created earlier. We also specify the ``ndkVersion``.

	* First replace ``plugins {...}`` with ``apply plugin: 'com.android.application'``.
	* In the ``android`` section, remove the ``namespace`` and ``compileSdk`` members and add ``compileSdkVersion 29`` and ``ndkVersion '23.1.7779620'``
	* Replace ``minSdk`` with ``minSdkVersion`` and ``targetSdk`` with ``targetSdkVersion``. We specify ``29`` for the version, but any value great than or equal to ``24`` will work.
	* Remove ``testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"``.
	* Under ``android``, add ``buildFeatures { prefab true }``. This allow any prefabs to be used in build.
	* Under ``buildTypes``, add ``debug { jniDebuggable true debuggable true renderscriptDebuggable true minifyEnabled false }``. This allow us to debugging the code, and to debug the ``JNI``.
	* Under ``android`` again, add ``externalNativeBuild { cmake { version '3.22.1' path '../CMakeLists.txt' } }``. This specifies that we want to do an external native build with CMake and further specifying the version and the path to our ``CMakeLists.txt``.
	* Under ``dependencies``, remove all ``testImplementation`` and ``androidTestImplementation`` entries, also remove ``implementation 'androidx.core:core-ktx:...'`` and ``implementation 'com.google.android.material:material:...'`` as we don't need the Kotlin or Android materials. Add ``implementation fileTree(dir: 'libs', include: ['*.jar'])`` for any ``*.jar`` libraries. 
	* Add ``implementation 'org.khronos.openxr:openxr_loader_for_android:1.0.27'`` to access specifically the ``AndroidManifest.xml`` file that includes the required uses-permissions, authorising the OpenXR runtime broker and states the intent to use the ``OpenXRRuntimeService`` and the ``OpenXRApiLayerService``.

	.. literalinclude:: ../Chapter2/build.gradle
		:language: groovy

	Now, we can config our ``build.gradle`` file in the ``/Chapter2`` folder. This is a complete replacement the default one provided by Android Studio. This file stipulates the repositories and gradle version to be used.
	
	The ``settings.gradle`` can be reduce to just: ``include ':app'``, and in the ``gradle.properties`` we need to remove ``kotlin.code.style=official`` and ``android.nonTransitiveRClass=true`` and their comments. Update ``org.gradle.jvmargs`` to ``-Xmx1536m`` and add ``android.enableJetifier=false`` to speed up builds.

	With that completed, we should now be able to sync the Gradle file and build the project.

Common Files
============

Now, that we have set up the project and source file. We will create a few files in a ``/Common`` directory in the *workspace* folder. Below are all the files available to download:

DebugOutput
-----------
:download:`DebugOutput.h <../Common/DebugOutput.h>`

``DebugOutput`` is a class that redirects ``std::cout`` and ``std::cerr`` to the output window in your IDE.

.. container:: windows linux
	:name: windows-linux-id-1

	``DebugOutput`` inherites from ``vsBufferedStringStreamBuf``, which inherites from ``std::streambuf``. ``vsBufferedStringStreamBuf`` queues the data from the redirected ``std::streambuf`` and calls ``virtual void writeString(const std::string &)``, which ``DebugOutput`` inplements as a call to ``OutputDebugStringA()``.

.. container:: android
	:name: android-id-1

	``DebugOutput`` inherites from ``AndroidStreambuf``, which inherites from ``std::streambuf``. ``vsBufferedStringStreamBuf`` queues the data from the redirected ``std::streambuf`` and calls ``__android_log_write()`` to log the message to Android Logcat.

HelperFunctions
---------------
:download:`HelperFunctions.h <../Common/HelperFunctions.h>`

A simple header file for boilerplate code for the various platforms

.. literalinclude:: ../Common/HelperFunctions.h
	:language: cpp
	:start-at: // C/C++ Headers
	:end-at: #include <unordered_map>

Includes for various C/C++ standard header.

.. literalinclude:: ../Common/HelperFunctions.h
	:language: cpp
	:start-at: // Debugbreak
	:end-at: #endif

This code above defines the macro ``DEBUG_BREAK``, according to which platform we're building for. This macro will stop execution of your program when an error occurs, so you can see where it happened and fix it. We use this macro in the ``OpenXRMessageCallbackFunction()`` function, which is discussed in detail in :doc:`Chapter 5.2. <5-extensions>` 

.. literalinclude:: ../Common/HelperFunctions.h
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Helper_Functions1
	:end-before: XR_DOCS_TAG_END_Helper_Functions1

``IsStringInVector()`` and ``BitwiseCheck()`` are just simple wrappers over commonly used code.

OpenXRDebugUtils
----------------
:download:`OpenXRDebugUtils.h <../Common/OpenXRDebugUtils.h>`

:download:`OpenXRDebugUtils.cpp <../Common/OpenXRDebugUtils.cpp>`

A header and cpp file pair helps in setting up the DebugUtilsMessenger. ``XR_EXT_debug_utils`` in an OpenXR instance extension that can intercept call made to OpenXR and provide extra information or report warning and errors, if the usage of the API or the current state of OpenXR is not valid. As you go through this tutorial it is highly recommended to have this enable to help with debugging. This is discussed in detail in :doc:`Chapter 5.2. <5-extensions>`, but in general ``CreateOpenXRDebugUtilsMessenger()`` creates and ``DestroyOpenXRDebugUtilsMessenger()`` destroys an ``XrDebugUtilsMessengerEXT``. ``OpenXRMessageCallbackFunction()`` is a called function specified at object creation, which is used to call when OpenXR raises an issue.

.. literalinclude:: ../Common/OpenXRDebugUtils.h
	:language: cpp

The header declares the functions and the cpp defines them.

OpenXRHelper
------------
:download:`OpenXRHelper.h <../Common/OpenXRHelper.h>`

A header for including all the neeeded header files and helper functions.

.. literalinclude:: ../Common/OpenXRHelper.h
	:language: cpp
	:start-at: // Define any
	:end-at: #include "openxr/openxr_platform.h"
	:emphasize-lines: 5

Here, we include the main OpenXR header file ``openxr.h`` and the OpenXR platform header file ``openxr_platform.h``.
For the OpenXR platform header file, note the comment about using the preceding ``XR_USE_PLATFORM_...`` and ``XR_USE_GRAPHICS_API_...`` macros. When enabled, we gain access to functionality that interact with the chosen graphics API and/or platform. These macros are automatically set by ``GraphicsAPI.h``

.. literalinclude:: ../Common/OpenXRHelper.h
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Helper_Functions0
	:end-before: XR_DOCS_TAG_END_Helper_Functions0

This defines the macro ``OPENXR_CHECK``. Many OpenXR functions return a ``XrResult``. This macro will check if the call has failed and logs a message to ``std::cerr``. This can be modified to suit your needs. There are two additional functions ``GetXRErrorString()`` and ``OpenXRDebugBreak()``, which are used to convert the ``XrResult`` to a string and as a breakpoint function respectively.


GraphicsAPI
-----------
This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. The drived class is based on your graphics API selection. Include both the header and cpp files for both ``GraphicsAPI`` and ``GraphicsAPI...``. ``GraphicsAPI.h`` includes the headers and macros needed to set up your platform and grapchis API.

.. literalinclude:: ../Common/GraphicsAPI.h
	:language: cpp
	:start-at: #include "HelperFunctions.h"
	:end-at: #include "OpenXRHelper.h"

:download:`GraphicsAPI.h <../Common/GraphicsAPI.h>`

:download:`GraphicsAPI.cpp <../Common/GraphicsAPI.cpp>`

.. container:: d3d11
	:name: d3d11-id-1

	.. rubric:: DirectX 11

	:download:`GraphicsAPI_D3D11.h <../Common/GraphicsAPI_D3D11.h>`

	:download:`GraphicsAPI_D3D11.cpp <../Common/GraphicsAPI_D3D11.cpp>`

.. container:: d3d12
	:name: d3d12-id-1

	.. rubric:: DirectX 12

	:download:`GraphicsAPI_D3D12.h <../Common/GraphicsAPI_D3D12.h>`

	:download:`GraphicsAPI_D3D12.cpp <../Common/GraphicsAPI_D3D12.cpp>`

.. container:: opengl
	:name: opengl-id-1

	.. rubric:: OpenGL

	:download:`GraphicsAPI_OpenGL.h <../Common/GraphicsAPI_OpenGL.h>`

	:download:`GraphicsAPI_OpenGL.cpp <../Common/GraphicsAPI_OpenGL.cpp>`

.. container:: opengles
	:name: opengles-id-1

	.. rubric:: OpenGL ES

	:download:`GraphicsAPI_OpenGL_ES.h <../Common/GraphicsAPI_OpenGL_ES.h>`

	:download:`GraphicsAPI_OpenGL_ES.cpp <../Common/GraphicsAPI_OpenGL_ES.cpp>`

.. container:: vulkan
	:name: vulkan-id-1

	.. rubric:: Vulkan

	:download:`GraphicsAPI_Vulkan.h <../Common/GraphicsAPI_Vulkan.h>`

	:download:`GraphicsAPI_Vulkan.cpp <../Common/GraphicsAPI_Vulkan.cpp>`

OpenXRTutorial and Main
=======================

We can now start adding code to our ``main.cpp`` file. First, we add ``DebugOutput.h``

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: #include "DebugOutput
	:end-at: .h"

Next, we add the ``GraphicsAPI_....h`` header to include the Graphics API code of your chosen graphics API. This will in turn include ``GraphicsAPI.h``, ``HelperFunctions.h``  and ``OpenXRHelper.h``. In this tutorial, we include all of them, though you would only be picking one.

.. container:: d3d11
	:name: d3d11-id-1

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 1

.. container:: d3d12
	:name: d3d12-id-1
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 2

.. container:: opengl
	:name: opengl-id-1

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 3

.. container:: opengles
	:name: opengles-id-1
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 4

.. container:: vulkan
	:name: vulkan-id-1
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_include_GraphicsAPIs
		:end-before: XR_DOCS_TAG_END_include_GraphicsAPIs
		:emphasize-lines: 5

You can also include ``OpenXRDebugUtils.h`` to help with set up of ``XrDebugUtilsMessengerEXT``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: #include "OpenXRDebugUtils
	:end-at: .h"

Now we will define the main class ``OpenXRTutorial`` of the application. It's just a stub for now, with an empty ``Run()`` method:

.. code-block:: cpp

	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type apiType) = default;
		~OpenXRTutorial() = default;

		void Run()
		{
		}

	private:
		 bool m_applicationRunning = true;
		 bool m_sessionRunning = false;
	};

Finally, let's add the main function for the application. It will look slightly different, depending on your
chosen platform. We first create a 'pseudo-main function' called ``OpenXRTutorial_Main()``, in which we create an instance of our ``OpenXRTutorial`` class, taking a ``GraphicsAPI_Type`` parameter, and call the ``Run()``method. ``GraphicsAPI_Type`` can be changed to suit the graphics API that you have chosen.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: void OpenXRTutorial_Main(GraphicsAPI_Type apiType)
	:end-at: }

Then, we create the actual platform specific main function (our entry point to the application), which will call ``OpenXRTutorial_Main()`` with our ``GraphicsAPI_Type`` parameter:

.. container:: windows linux
	:name: windows-linux-id-1

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_main_WIN32___linux__
		:end-before: XR_DOCS_TAG_END_main_WIN32___linux__

.. container:: android
	:name: android-id-1
	
	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_android_main___ANDROID__
		:end-before: XR_DOCS_TAG_END_android_main___ANDROID__

	Android requires a few extra additions to the class. Namely, ``android_app *``, ``AndroidAppState`` and ``AndroidAppHandleCmd``, which are used in getting updates from the Android Operating System and keep our application functioning.

	.. literalinclude:: ../Chapter2/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_Android_System_Functionality
		:end-before: XR_DOCS_TAG_END_Android_System_Functionality

	Before we can use OpenXR for Android, we need to initialise the loader based the application's context and virtual machine. We retrieve the function pointer to ``xrInitializeLoaderKHR``, and with the ``XrLoaderInitInfoAndroidKHR`` filled out call that function to initialise OpenXR for our use. At this point, we also attach the current thread to the Java Virtual Machine. We assign our ``AndroidAppState`` static member and our ``AndroidAppHandleCmd()`` static method to the ``android_app *`` and save it to a static member in the class.

Build and Run
=============

.. container:: windows
	:name: windows-id-1

	Now launch CMake GUI, and point the "Where is the source code" box to the root of your solution *workspace* directory,
	where your original ``CMakeLists.txt`` is located. Point the "Where to build the binaries" box to a subdirectory called ``build``,
	click Configure, "Yes" to create the build folder, and "OK" to accept the default Generator.

	You can now build and run your program. It should compile and link with no errors or warnings.

.. container:: linux
	:name: linux-id-1

	You now have the files and folders, laid out as follows:

	.. figure:: linux-vscode-initial-files.png
		:alt: Initial files in VS Code 

	Having installed the CMake extension for Visual Studio Code, you can now right-click on the main ``CMakeLists.txt`` file (the one in the root *workspace* folder). We can select “Configure and Build All” from the right-click menu of the main ``CMakeLists.txt`` file.

	.. figure:: linux-vscode-cmake-configure.png
		:alt: Select

	To enable debugging, select the Run/Debug panel in Visual Studio Code. You will now need to create a debugging configuration.
	Click the "Gear" icon to edit the file launch.json, and enter the following:

	.. code-block:: json

		{
			"version": "0.2.0",
			"configurations": [
				{
					"type": "cppdbg",
					"request": "launch",
					"name": "Chapter2",
					"program": "${workspaceFolder}/build/Chapter2/Chapter2",
					"cwd":"${workspaceFolder}/Chapter2",
					"externalConsole": true,
				}
			]
		}

.. container:: android
	:name: android-id-1

	With all the source and build systems set up, we can now build the Android project. In upper right of Android Studio, you should find the toolbar below. Click the green hammer icon to build the project, if all is successful you should see "BUILD SUCCESSFUL in [...]s" in the Build Output window.
	It is also recommended to sync the gradle files too.

	Next to the green hammer icon is the Run/Debug configuration dropdown menu. If that isn't populated, create a configuration called app.

	Turn on and connect your Android device. Set up any requirments for USB debugging and adb. You device should appear in the dropdown. Here, I am using a Oculus Quest 2.

	.. figure:: android-studio-build-run-toolbar.png
		:alt: Build/Run Toolbar
	
	To debug/run the application click the green bug icon.

Now that we have a basic application up and running with the OpenXR header files and libraries, we can start to set up the core aspects of OpenXR. As a modern Khronos API, the OpenXR is heavily influcencd by the Vulkan API. So those who are familiar with the style of the Vulkan API will find OpenXR easy to follow.

****************************************
2.1 Creating an XrInstance / xrGetSystem
****************************************

Firstly, add to the ``OpenXRTutorial`` class the methods: ``CreateInstance()``, ``GetInstanceProperties()``, ``GetSystemID()`` and ``DestroyInstance()``. Update ``OpenXRTutorial::Run()`` to call those methods in that order and add to the class in a private section the following members.

.. code-block:: cpp
	
	class OpenXRTutorial{
	public:
		// [...]
	
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

2.1.1 XrInstance
================

The ``XrInstance`` is the foundational object that we need to create first. The ``XrInstance`` encompasses the application setup state, OpenXR API version and any layers and extensions. So inside the ``CreateInstance()`` method, we will first look at the ``XrApplicationInfo``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrApplicationInfo
	:end-before: XR_DOCS_TAG_END_XrApplicationInfo
	:dedent: 8

This structure allows you specify both the name and the version for your application and engine. These members are solely for your use as the application developer. The main member here is the ``XrApplicationInfo::apiVersion``. Here we use the ``XR_CURRENT_API_VERSION`` macro to specific the OpenXR version that we want to run. Also note here the use of ``strncpy()`` to set the applicationName and engineName. If you look at ``XrApplicationInfo::applicationName`` and ``XrApplicationInfo::engineName`` members, they are of type ``char[]``, hence you must copy your string into that ``char[]`` and you must also by aware of the allowable length.

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

2.1.2 XrSystemId
================

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

*************************
2.2 Creating an XrSession
*************************

The next major component of OpenXR that needs to be created in an ``XrSession``. An ``XrSession`` encapulates the state of application from the perspective of OpenXR. When an ``XrSession`` is created, it starts in the ``XR_SESSION_STATE_IDLE``. It is upto the runtime to provide any updates to the ``XrSessionState`` and for the appliaction to query them and react to them. We will explore this in :doc:`Chapter 2.3. <2-Polling the Event Loop>`

For now, we are just going to create an ``XrSession``. At this point, you'll need to select which Graphics API you wish to use. Only one Graphics API can be used with an ``XrSession``. This tutorial demostrates how to use D3D11, D3D12, OpenGL, OpenGL ES and Vulkan in conjunction with OpenXR for the purpose of rendering graphics to the provided views. Ultimately, you will most likely be bringing your own rendering solution to this tutorial, therefore the code examples provided for the Graphics APIs are `placeholders` for you own code base; demostrating in this sub-chapter what objects are needed from your Graphics API in order to create an ``XrSession``. This tutorial uses polymorphic classes; ``GraphicsAPI_...`` derives from the base ``GraphicsAPI`` class. There are both compile and runtime checks to select the requested Graphics API, and we construct an apropriate derived classes throught the use of ``std::unique_ptr<>``. 

Update the Constructor and ``Run()`` method as shown and add the following members:
``CheckGraphicsAPI_TypeIsValidForPlatform()`` is declared in ``GraphicsAPI.h``.

.. code-block:: cpp

	class OpenXRTutorial {
	public:
		OpenXRTutorial(GraphicsAPI_Type api)
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

2.2.1 XrSession
===============

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateDestroySession
	:end-before: XR_DOCS_TAG_END_CreateDestroySession

Above is the code for creating and destroying an ``XrSession``. ``xrDestroySession()`` will destroy the ``XrSession`` when we have finished and shutting down the application. ``xrCreateSession()`` takes the ``XrInstance``, ``XrSessionCreateInfo`` and ``XrSession`` return object. If the function call was successful, ``xrCreateSession()`` will return ``XR_SUCCESS`` and ``XrSession`` will be non-null. The ``XrSessionCreateInfo`` structure is deceptively simple. ``XrSessionCreateInfo::createFlags`` and ``XrSessionCreateInfo::systemId`` are easily filled in, but we need to specify which Graphics APIs we wish to use. This is achieved via the use of the ``XrSessionCreateInfo::next`` void pointer. Following the Vulkan API's style of extensibility, structures for creating objects can be extended to enable extra functionality. In our case, the extension is required and thus ``XrSessionCreateInfo::next`` can not be a nullptr. That pointer must point to 'exactly one graphics API binding structure (a structure whose name begins with "XrGraphicsBinding")' (`XrSessionCreateInfo(3) Manual Page <https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrSessionCreateInfo.html>`_).

2.2.2 GraphicsAPI
=================

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

**************************
2.3 Polling the Event Loop
**************************

OpenXR uses an event based system to describes changes with the XR system. It's the application's responsibility to poll these events and react to them. The polling of events is done by the function ``xrPollEvent()``. The application should continually call this function throughout its lifetime. Within a single XR frame, the application should continuously call ``xrPollEvent()`` until the internal event queue is 'drained'; multiple events can occurs across the XR frame and the application needs to handle and respond to each accordingly.

Firstly, we will update the class to add the new methods and members.

.. code-block:: cpp

	class OpenXRTutorial {
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

2.3.1 xrPollEvent
=================

Next, we will define the ``PollEvents()`` method. Here, we use a do-while loop to the check the result of ``xrPollEvent()`` - whilst that function returns ``XR_SUCCESS``, there are events for us to process. ``xrPollEvent()`` will fill in the ``XrEventDataBuffer`` structure that we pass to the function call. ``xrPollEvent()`` will update the member variable ``type`` and from this we can use a switch statement to select the appropriate code path. Depending on the updated type, we can use a ``reinterpret_cast<>()`` to get the actual data that ``xrPollEvent()`` returned.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollEvents
	:end-before: XR_DOCS_TAG_END_PollEvents
	:dedent: 4

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

2.3.2 XrSessionState
====================

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

2.3.3 xrBeginSession and xrEndSession
=====================================

If the ``XrSessionState`` is ``XR_SESSION_STATE_READY``, the application can call ``xrBeginSession()``.
In the ``XrSessionBeginInfo`` structure, we assign to ``XrSessionBeginInfo::primaryViewConfigurationType`` the ``m_viewConfiguration`` from the class, which in our case is ``XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO``. This specifies the view configuration of the form factor's primary display - For Head Mounted Displays, it is two views (one per eye).

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
	:dedent: 16

If the ``XrSessionState`` is ``XR_SESSION_STATE_STOPPING``, the application should call ``xrEndSession()``.

.. literalinclude:: ../Chapter2/main.cpp
	:language: cpp
	:start-at: if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
	:end-before: if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
	:dedent: 16