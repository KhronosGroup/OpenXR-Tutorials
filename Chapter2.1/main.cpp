#include <iostream>
#include "openxr/openxr.h"

int main(int argc, char** argv)
{
	std::cout << "Hello from OpenXR Tutorial Chapter 2.1." << std::endl;

	XrInstance instance;

	XrApplicationInfo AI;
	strcpy_s(AI.applicationName, "OpenXR Tutorial Chapter 2.1");
	AI.applicationVersion = 1;
	strcpy_s(AI.engineName, "Teleport VR OpenXR Engine");
	AI.engineVersion = 1;
	AI.apiVersion = XR_CURRENT_API_VERSION;

	XrInstanceCreateInfo instanceCI;
	instanceCI.type = XR_TYPE_INSTANCE_CREATE_INFO;
	instanceCI.next = nullptr;
	instanceCI.createFlags = 0;
	instanceCI.applicationInfo = AI;
	instanceCI.enabledApiLayerCount = 0;
	instanceCI.enabledApiLayerNames = nullptr;
	instanceCI.enabledExtensionCount = 0;
	instanceCI.enabledExtensionNames = nullptr;

	XrResult res = xrCreateInstance(&instanceCI, &instance);
	if (res == XR_SUCCESS)
		std::cout << "Created OpenXR Instance." << std::endl;
	else
		std::cout << "Failed to Create OpenXR Instance." << std::endl;

	res = xrDestroyInstance(instance);
	if (res == XR_SUCCESS)
		std::cout << "Destroyed OpenXR Instance." << std::endl;
	else
		std::cout << "Failed to Destroy OpenXR Instance." << std::endl;
}