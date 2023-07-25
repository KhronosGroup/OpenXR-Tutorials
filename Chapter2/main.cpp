// main.cpp -- In OpenXR Tutorial section 2.1 (through part of the "XrInstance" section)
//
// WRS: There were references to an undefined "xrInstance", plus the
// 	"XrInstance" type name was sometimes used in places other than when
// 	a type was required -- I changed both of these uses to "instance"
// 	as that seems to be the intent.
//
// WRS:	The name "apiType" is used, but never defined, so the line that uses
// 	it will not compile.
//
// WRS:	Running the executable does not connect with the OpenXR runtime.
// 	Why not?

#include <iostream>	// Needed for "cout" stream
#include <vector>	// Needed for vector (array) operations
#include <cstring>	// WRS: added for "strcpy()"

//#include "Helperfunctions.h"	// WRS: added -- but not needed yet
#include "OpenXRHelper.h"	// WRS: added

// WRS: I assume the following goes here, the instructions didn't say
class OpenXRTutorialChapter2 {
public:
        OpenXRTutorialChapter2() = default;
        ~OpenXRTutorialChapter2() = default;

        void Run()
        {
		CreateInstance();		// 2.1: added
		printf("XR Instance created.\n");	// WRS: added for debugging

		GetInstanceProperties();	// 2.1: added
		GetSystemID();			// 2.1: added
		printf("Got SystemID.\n");	// WRS: added for debugging

		DestroyInstance();		// 2.1: added
        }

private:
// 2.1: entire "private" section is new
 	void CreateInstance()
	{
		XrApplicationInfo AI;
		strcpy(AI.applicationName, "OpenXR Tutorial Chapter 2");	// WRS: better to use strncpy()
		AI.applicationVersion = 1;
		strcpy(AI.engineName, "OpenXR Engine");	// WRS: When would this not be "OpenXR Engine"?	// WRS: better to use strncpy()
		AI.engineVersion = 1;
		AI.apiVersion = XR_CURRENT_API_VERSION;

		/* Add additional instance layers/extensions */
		instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#if 0 /* WRS: "apiType" isn't defined, so this lines doesn't compile. */
		instanceExtensions.push_back(GetGraphicsAPIInstanceExtensionString(apiType));	// WRS: "apiType" is a private variable -- is this an extension or a layer?
#endif
	}

 	void DestroyInstance()
	{
		OPENXR_CHECK(xrDestroyInstance(/*WRS:XrInstance*/instance), "Failed to destroy Instance.");	// WRS: I feel like this OPENXR_CHECK() macro usage can wiat until later
	}

 	void GetInstanceProperties()
	{
		XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
		OPENXR_CHECK(xrGetInstanceProperties(/*WRS:XrInstance*/instance, &instanceProperties), "Failed to get InstanceProperties.");

		std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - ";
		std::cout << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << ".";
		std::cout << XR_VERSION_PATCH(instanceProperties.runtimeVersion) << std::endl;
	}

 	void GetSystemID()
	{
		XrSystemGetInfo systemGI{XR_TYPE_SYSTEM_GET_INFO};
		systemGI.formFactor = formFactor;
		OPENXR_CHECK(xrGetSystem(/*WRS:XrInstance*/instance, &systemGI, &systemID), "Failed to get SystemID.");

		XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
		OPENXR_CHECK(xrGetSystemProperties(/*WRS:XrInstance*/instance, systemID, &systemProperties), "Failed to get SystemProperties.");
	}

private:
// 2.1: also new variable fields
	XrInstance instance = {};	// WRS: what is this?  A handle?
	std::vector<const char *> activteAPILayers = {};	// WRS: need comments! -- why are two "const char *", and next are "string"?
	std::vector<const char *> activteInstanceExtensions = {};	// WRS: need comments! -- why are two "const char *", and next are "string"?
	std::vector<std::string> apiLayers = {};
	std::vector<std::string> instanceExtensions = {};

	XrFormFactor formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	XrSystemId systemID = {};
};


void OpenXRTutorial_Main() {
    //DebugOutput debugOutput;	// WRS: commented out -- not defined anywhere
    std::cout << "OpenXR Tutorial Chapter 2." << std::endl;
    OpenXRTutorialChapter2 app;	// WRS: had to remove "()" after "app" & the "VULKAN" inside the "()"
    app.Run();
}


int main(int argc, char **argv) {
    OpenXRTutorial_Main();
}

