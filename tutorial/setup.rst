#####
Setup
#####

Once again, in case you want to see instructions for a different platform, select it here:

.. raw:: html
   :file: platforms.html

.. container:: windows

    :name: windows-intro-1

	.. rubric:: Windows

	Showing instructions for Windows.

.. container:: linux

    :name: linux-intro-1

	.. rubric:: Linux

	Showing instructions for Linux.

.. container:: android

    :name: android-intro-1

	.. rubric:: Android

	Showing instructions for Android.

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

.. container:: android
    :name: android-id-1

	.. rubric:: Project Generation

	Here, I'll show how to hand build an Android Studio project that runs a C++ Native Activity.
	Open Android Studio, select New Project and choose an Empty Activity. Set the names and save location. The language can be ignored here as we are using C++, and we can set the Minimum SDK to API 24: Android 7.0(Nougat). Complete set up.

	.. image:: android-studio-newproject.png
	   :alt: Android Studio - New Project - Empty Activity.
	   :align: right

	.. rubric:: CMake

	With the Android Studio project now set up, we need to modify some of the files and folders so as to set up the project to support the C++ Native Activity.
	Under the ``app`` folder, you can delete the ``libs`` folder, and under the ``app/src`` you can also delete the ``androidTest`` and ``test`` folders. Finally under ``app/src/main``, delete the ``java`` folder and add a ``cpp`` folder. Under the ``app/src/main/res``, delete the ``values-night`` and ``xml`` folders. Under the ``values`` modify colors.xml and theme.xml as shown.

	.. code-block:: xml

		<!-- colors.xml -->
		<?xml version="1.0" encoding="utf-8"?>
		<resources>
		    <color name="colorPrimary">#008577</color>
		    <color name="colorPrimaryDark">#00574B</color>
		    <color name="colorAccent">#D81B60</color>
		</resources>

		<!-- theme.xml -->
		<resources>

		    <!-- Base application theme. -->
		    <style name="AppTheme" parent="Theme.AppCompat.Light.DarkActionBar">
		        <!-- Customize your theme here. -->
		        <item name="colorPrimary">@color/colorPrimary</item>
		        <item name="colorPrimaryDark">@color/colorPrimaryDark</item>
		        <item name="colorAccent">@color/colorAccent</item>
		    </style>

		</resources>

	Within the ``app/src/main/cpp`` folder, create a CMakeLists.txt. We will use this file to specific how our Native C++ code will be built. This CMakeList will be invoked by Android Studio's Gradle build system. 

	.. code-block:: cmake 

		# For more information about using CMake with Android Studio, read the
		# documentation: https://d.android.com/studio/projects/add-native-code.html

		cmake_minimum_required(VERSION 3.22.1)
		project("openxrtutorialch2")

		# native_app_glue
		add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
		target_include_directories(native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)

		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate") # export ANativeActivity_onCreate for java to call.
		add_library(openxrtutorialch2 SHARED ../../../../../Chapter2.1/main.cpp)

		# import openxr_loader
		add_library(openxr_loader SHARED IMPORTED)
		set_target_properties(openxr_loader PROPERTIES IMPORTED_LOCATION "../../../../../../thirdparty/openxr-sdk/android/libs/android.arm64-v8a/libopenxr_loader.so")
		target_include_directories(openxrtutorialch2 PUBLIC ../../../../../thirdparty/openxr-sdk/include)

		# vulkan - Found in the NDK
		find_library(vulkan-lib vulkan)
		target_include_directories(openxrtutorialch2 PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include)

		# log - Found in the NDK
		find_library(log-lib log)

		target_link_libraries(openxrtutorialch2
		        android
		        native_app_glue
		        openxr_loader
		        ${vulkan-lib}
		        ${log-lib})

	First, we set the minimum required cmake version, here we are using 3.22.1 and the project's name. Next, we need to add a static library called native_app_glue. The native_app_glue library is compiled from a single source file android_native_app_glue.c. This interfaces between the Java Virtual Machine and our C++ code. Ultimately, it allows us to use the ``void android_main(struct android_app*)`` entry point. We also include that directory as we need access to the android_native_app_glue.h header file. Next, we need to set the ``CMAKE_SHARED_LINKER_FLAGS`` so that ``ANativeActivity_onCreate()`` is exported for the Java Virtual Machine to call. Next, we add our shared library openxrtutorialch2 that houses our code. Here, I have a relative path to our single C++ file.

	Now, we import the openxr_loader library. We need to do this, because it's external to the NDK library, and won't be automatically picked up. We call ``set_target_properties()`` to specific the location of libopenxr_loader.so. We also include the directory to the OpenXR headers. Next, we find the Vulkan library in the NDK and include the directory to the Android Vulkan headers. At this time, we also find the log library. Finally we link the android, native_app_glue, openxr_loader, vulkan and log libraries to our openxrtutorialch2 library. Our libopenxrtutorialch2.so will packageed inside our apk along with any shared libraries that we have linked.

	.. rubric:: AndroidManifest.xml

	.. code-block:: xml

		<?xml version="1.0" encoding="utf-8"?>
		<manifest xmlns:android="http://schemas.android.com/apk/res/android"
		    package="com.simul.openxrtutorialch2"
		    android:versionCode="1"
		    android:versionName="1.0">

		    <application
		        android:allowBackup="false"
		        android:fullBackupContent="false"
		        android:icon="@mipmap/ic_launcher"
		        android:label="@string/app_name"
		        android:hasCode="false">
		        <activity
		            android:name="android.app.NativeActivity"
		            android:configChanges="orientation|keyboardHidden"
		            android:debuggable="true">
		            <meta-data
		                android:name="android.app.lib_name"
		                android:value="openxrtutorialch2" />

		            <intent-filter>
		                <action android:name="android.intent.action.MAIN" />
		                <category android:name="android.intent.category.LAUNCHER" />
		            </intent-filter>
		        </activity>
		    </application>
		</manifest>

	We now need to modify our AndroidManifest.xml file to tell Android to run a Native Activity. We set ``android:name`` to "android.app.NativeActivity" and update ``android:configChanges`` to "orientation|keyboardHidden" to not close the activity on those changes. Next under the meta-data section, we set these values: ``android:name`` to "android.app.lib_name" and ``android:value`` to "openxrtutorialch2", where ``android:value`` is name of the library we created in the CMakeLists, thus pointing our NativeActivity to the correct library.

	.. rubric:: Gradle

	.. code-block:: groovy

		apply plugin: 'com.android.application'

		android {
		    compileSdkVersion 29
		    ndkVersion '23.1.7779620'

		    defaultConfig {
		        applicationId "com.simul.openxrtutorialch2"
		        minSdkVersion 29
		        targetSdkVersion 29
		        versionCode 1
		        versionName "1.0"
		        ndk {
		            abiFilters 'arm64-v8a'
		        }
		    }
		    buildFeatures {
		        prefab true
		    }
		    buildTypes {
		        release {
		            minifyEnabled false
		            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
		        }
		        debug {
		            jniDebuggable true
		            debuggable true
		            renderscriptDebuggable true
		            minifyEnabled false
		        }
		    }
		    externalNativeBuild {
		        cmake {
		            version '3.22.1'
		            path 'src/main/cpp/CMakeLists.txt'
		        }
		    }
		}

		dependencies {
		    implementation fileTree(dir: 'libs', include: ['*.jar'])
		    implementation 'androidx.appcompat:appcompat:1.0.2'
		    implementation 'androidx.constraintlayout:constraintlayout:1.1.3'
		    implementation 'org.khronos.openxr:openxr_loader_for_android:1.0.27'
		}

	Now, we can config our build.gradle file in the ``app`` folder. First remove any references to Java, Kotlin and to testing. Next add in the ``externalNativeBuild`` section specifying CMake, its version and the location of the CMakeLists.txt that we created earlier. Also specify under the ``ndk`` section the ``abiFilters``. We will just be using arm64-v8a in this tutorial. ``ndkVersion`` should also be specified.

	.. code-block:: groovy

		// Top-level build file where you can add configuration options common to all sub-projects/modules.
		buildscript {
		    repositories {
		       google()
		       mavenCentral()
		    }
		    dependencies {
		        classpath 'com.android.tools.build:gradle:4.2.2'
		    }
		}

		allprojects {
		    repositories {
		        google()
		        mavenCentral()
		    }
		}

		task clean(type: Delete) {
		    delete rootProject.buildDir
		}

	Now, we can config our build.gradle file in the root folder of the project. This is a complete replacement the default one provided by Android Studio. This file stipulates the repositories and gradle version to be used.
	The settings.gradle can be reduce to just: ``include ':app'``, and in the gradle.properties we need to remove ``kotlin.code.style=official`` and ``android.nonTransitiveRClass=true``.

	With that completed, we should now be able to sync the Gradle file and build the project.

	Now we’ll create our source file. Create a new text file called “main.cpp” and put it in the Chapter2 directory. This file will be referenced in the CMakeLists file we created, so ensure the path is correct. In this file, place the following code:

.. highlight:: cpp
.. code-block:: cpp

	// C Headers
	#include <iostream>
	#include <sstream>
	#include <vector>

This is boilerplate for the various platforms. Now add the following:

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-at: // OpenXR Headers
	:end-at: #include "openxr/openxr_platform.h"
	:name: xr-headers
	:emphasize-lines: 9

Here we include the main OpenXR header, openxr.h; and the platform header openxr_platform.h.
What the latter does, depends on which of the preceding XR_USE\_ macros we enable. We will enable
one of these only, depending on your choice of graphics API.
Now add the following:

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-at: // Debugbreak
	:end-at: #endif

This defines the macro ``DEBUG_BREAK``, according to what platform we're building for. This macro will
stop execution of your program when an error occurs, so you can see where it happened and fix it.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Helper_Functions
	:end-before: XR_DOCS_TAG_END_Helper_Functions

This defines the macro ``OPENXR_CHECK``. Many OpenXR functions return a XrResult. This macro will check if the call has failed and logs a message to stdout. This can be modified to suit your needs. There are three additional functions ``GetXRErrorString()``, ``IsStringInVector()`` and ``BitwiseCheck()``, which are just simple wrappers over commonly used code.

Now we will define the main class of your application. It's just a stub for now, with an empty Run() method:

.. code-block:: cpp

	class OpenXRTutorial_Ch2_1 {
	public:
		OpenXRTutorial_Ch2_1() = default;
		~OpenXRTutorial_Ch2_1() = default;

		void Run()
		{
		}
	};

Finally, let's add the main function for your app. It looks slightly different, depending on your
choice of platform, but in each case, we define an instance of our OpenXRTutorial class, and call the Run()
method:

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-at: void OpenXRTutorial_Main()
	:end-at: #endif

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

1. SETUP (Basic structure ready)

Now that we have a basic application up and running with the OpenXR header files and libraries, we can start to set the core aspects of OpenXR. As a modern Khronos API, the OpenXR is heavily influcencd by the Vulkan API. So those who are familiar with the style of the Vulkan API will find OpenXR easy to follow.

Creating an XrInstance
----------------------
2.1. Creating an XrInstance / xrGetSystem (xrCreateInstance)

.. rubric:: XrInstance

Firstly, add to the ``OpenXRTutorial`` class the methods: ``CreateInstance()``, ``GetInstanceProperties()``, ``GetSystemID()`` and ``DestroyInstance()``. Update ``OpenXRTutorial::Run()`` to call those methods in that order and add to the class in a private section the following members.

.. code-block:: cpp
	
	class OpenXRTutorial_Ch2_1 {
	public:
		OpenXRTutorial() = default;
		~OpenXRTutorial() = default;
	
		Run()
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
		XrSystemId systemID = {};
	}

The ``XrInstance`` is the foundational object that we need to create first. The ``XrInstance`` encompasses the application setup state, OpenXR API version and any layers and extensions. So inside the ``CreateInstance()`` method, we will first look at the ``XrApplicationInfo``.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrApplicationInfo
	:end-before: XR_DOCS_TAG_END_XrApplicationInfo

This structure allows you specify both the name and the version for your application and engine. These members are solely for your use as the application developer. The main member here is the ``XrApplicationInfo::apiVersion``. Here we use the ``XR_CURRENT_API_VERSION`` macro to specific the OpenXR version that we want to run. Also note here the use of ``strcpy()`` to set the applicationName and engineName. If you look at ``XrApplicationInfo::applicationName`` and ``XrApplicationInfo::engineName`` members, they are of type ``char[]``, hence you must copy your string into that ``char[]`` and you must also by aware of the allowable length.

Similar to Vulkan, OpenXR allows applications to extend functionality past what is provided by the core specification. The functionality could be hardware/vendor specific. Most vital of course is which Graphics API to use with OpenXR. OpenXR supports D3D11, D3D12, Vulkan, OpenGL and OpenGL ES. Due the extensible nature of specification, it allows newer Graphics APIs and hardware functionality to be added with ease.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_instanceExtensions
	:end-before: XR_DOCS_TAG_END_instanceExtensions

Here, we store in a ``std::vector<std::string>`` the extension names that we would like to use. ``XR_EXT_DEBUG_UTILS_EXTENSION_NAME`` is a macro of a string defined in ``openxr.h``. The ``XR_EXT_debug_utils`` is extension that checks the validity of calls made to OpenXR, and can use a call back function to handle any raised errors. We will explore this extension more in Chapter 5.1. Depending on which ``XR_USE_GRAPHICS_API_...`` macro that you have defined, this code will add the relevant extension.

Not all API layers and extensions are available to use, so we much check which ones can use. We will use ``xrEnumerateApiLayerProperties()`` and ``xrEnumerateInstanceExtensionProperties()`` to check which ones the runtime can provide.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_find_apiLayer_extension
	:end-before: XR_DOCS_TAG_END_find_apiLayer_extension

These functions are called twice. The first time is to get the count of the API layers or extensions and the second is to fill out the array of structures. Before the second call, we need set ``XrApiLayerProperties::type`` or ``XrExtensionProperties::type`` to the correct value, so that the second call can correctly fill out the data. After we have enumerated the API layers and extensions, we use a nested loop to check to see whether an API layers or extensions is availble and add it to the activeAPILayers and/or activeInstanceExtensions respectively. Note the activeAPILayers and activeInstanceExtensions are of type ``std::vector<const char*>``. This will help us when fill out the next structure ``XrInstanceCreateInfo``.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_XrInstanceCreateInfo
	:end-before: XR_DOCS_TAG_END_XrInstanceCreateInfo

This section is fairly simple, as we now just collect data from before and assign them to members in the ``XrInstanceCreateInfo`` structure. Finally, we get to call ``xrCreateInstance()`` where we take pointers to thr stack ``XrInstanceCreateInfo`` and ``XrInstance`` objects. If the function succeeded, the result will be ``XR_SUCCESS`` and ``XrInstance`` will be non-null.

At the end of the program, we should destroy the ``XrInstance``. This is simple done with the function ``xrDestroyInstance()``.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-at: void DestroyInstance()
	:end-at: }

Whilst we have an ``XrInstance``, lets check its properties. We fill out the type and next members of the structure ``XrInstanceProperties`` and pass it along with the ``XrInstance`` to ``xrGetInstanceProperties()``. This function will fill out the rest of that structure for us to use. Here, we simply log to stdout the runtime's name, and with the use of the ``XR_VERSION_MAJOR``, ``XR_VERSION_MINOR`` and ``XR_VERSION_PATCH`` macros, we parse and log the runtime version.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-at: void GetInstanceProperties()
	:end-at: }

.. rubric:: XrSystemId

The next object that we want to get is the ``XrSystemId``. OpenXR 'separates the concept of physical systems of XR devices from the logical objects that applications interact with directly. A system represents a collection of related devices in the runtime, often made up of several individual hardware components working together to enable XR experiences'. 
`OpenXR Specification 5. System <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#system>`_. 

So, a ``XrSystemId`` could represent VR headset and a pair of contollers, or perhaps mobile device with video pass-through for AR. So we need to decide what type of ``XrFormFactor`` we are wanting to use, as some runtimes support multiple form factors. Here, we are selecting ``XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY`` for a Meta Quest or Pico Neo. OpenXR currently offers two option for the ``XrFormFactor``.

.. literalinclude:: ../thirdparty/openxr-sdk/include/openxr/openxr.h
	:language: cpp
	:start-at: typedef enum XrFormFactor {
	:end-at: } XrFormFactor;

We fill out the ``XrSystemGetInfo`` structure as desired and pass it as a pointer along with the ``XrInstance`` and a pointer to the ``XrSystemId`` to ``xrGetSystem()``. If successful, we should now have a non-null ``XrSystemId``.

.. literalinclude:: ../Chapter2.1/main.cpp
	:language: cpp
	:start-at: void GetSystemID()
	:end-at: }

We can now also get the system's properties. We partially fill out a ``XrSystemProperties`` structure and pass it as a pointer along with the ``XrInstance`` and the ``XrSystemId`` to ``xrGetSystemProperties()``. This function will fill out the rest of the ``XrSystemProperties`` structure; detailing the vendor's ID, system's name and the system's graphics and tracking properties.

.. literalinclude:: ../thirdparty/openxr-sdk/include/openxr/openxr.h
	:language: cpp
	:start-at: typedef struct XrSystemGraphicsProperties {
	:end-at: } XrSystemProperties;

Creating an XrSession
---------------------
2.2. Creating an XrSession (xrCreateSession, OpenGL based for code brevity)

Polling the Event Loop
----------------------
2.3. 2.3. Polling the Event Loop (xrPollEvent and Session States)