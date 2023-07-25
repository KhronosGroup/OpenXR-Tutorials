// OpenXRHelper.h

// OpenXR Headers
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

#if defined(__ANDROID__)
#include "android_native_app_glue.h"
#define XR_USE_PLATFORM_ANDROID
#endif

#define OPENXR_CHECK1(x, y)          \
    {                                \
        XrResult result = (x);       \
        if (!XR_SUCCEEDED(result)) { \
            std::cout << "ERROR: OPENXR: " << int(result) << "(" << result << ") " << y << std::endl; \
        }                            \
    }

#define OPENXR_CHECK(x, y)           \
    {                                \
        XrResult result = (x);       \
        if (!XR_SUCCEEDED(result)) { \
            std::cout << "ERROR: OPENXR: " << int(result) << "(" << (/*WRS:xrInstance*/instance ? GetXRErrorString(/*WRS:xrInstance*/instance, result) : "") << ") " << y << std::endl; \
        }                            \
    }

inline const char* GetXRErrorString(XrInstance xr_instance, XrResult res) {
    static char str[XR_MAX_RESULT_STRING_SIZE];
    xrResultToString(xr_instance, res, str);
    return str;
}

