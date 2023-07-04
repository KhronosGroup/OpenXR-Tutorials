#pragma once
#include "HelperFunctions.h"
#include "OpenXRHelper.h"

XrBool32 OpenXRMessageCallbackFunction(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageType, const XrDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

XrDebugUtilsMessengerEXT CreateOpenXRDebugUtilsMessenger(XrInstance instance);
void DestroyOpenXRDebugUtilsMessenger(XrInstance instance, XrDebugUtilsMessengerEXT debugUtilsMessenger);