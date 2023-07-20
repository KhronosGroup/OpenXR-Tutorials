#pragma once

// Define any XR_USE_PLATFORM_... / XR_USE_GRAPHICS_API_...before this header file.

// OpenXR Headers
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

// XR_DOCS_TAG_BEGIN_Helper_Functions0

#define OPENXR_CHECK1(x, y)                                                                                                                           \
    {                                                                                                                                                   \
        XrResult result = (x);                                                                                                                          \
        if (!XR_SUCCEEDED(result)) {                                                                                                                    \
            std::cout << "ERROR: OPENXR: " << int(result) << "(" << result << ") " << y << std::endl; \
        }                                                                                                                                               \
    }

#define OPENXR_CHECK(x, y)                                                                                                                              \
    {                                                                                                                                                   \
        XrResult result = (x);                                                                                                                          \
        if (!XR_SUCCEEDED(result)) {                                                                                                                    \
            std::cout << "ERROR: OPENXR: " << int(result) << "(" << (xrInstance ? GetXRErrorString(xrInstance, result) : "") << ") " << y << std::endl; \
        }                                                                                                                                               \
    }

inline const char* GetXRErrorString(XrInstance xr_instance, XrResult res) {
    static char str[XR_MAX_RESULT_STRING_SIZE];
    xrResultToString(xr_instance, res, str);
    return str;
}
// XR_DOCS_TAG_END_Helper_Functions0