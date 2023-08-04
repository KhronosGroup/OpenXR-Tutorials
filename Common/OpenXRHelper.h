#pragma once

// XR_DOCS_TAG_BEGIN_include_Linux
#ifdef XR_USE_PLATFORM_WAYLAND
#include "wayland-client.h"
#endif

#ifdef XR_USE_PLATFORM_XLIB
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef XR_USE_PLATFORM_XCB
#include <xcb/xcb.h>
#endif
// XR_DOCS_TAG_END_include_Linux

// Define any XR_USE_PLATFORM_... / XR_USE_GRAPHICS_API_... before this header file.

// OpenXR Headers
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

// XR_DOCS_TAG_BEGIN_Helper_Functions0
inline void OpenXRDebugBreak() {
    std::cerr << "Breakpoint here to debug.\n";
}

inline const char* GetXRErrorString(XrInstance xrInstance, XrResult result) {
    static char string[XR_MAX_RESULT_STRING_SIZE];
    xrResultToString(xrInstance, result, string);
    return string;
}

#define OPENXR_CHECK(x, y)                                                                                                                                  \
    {                                                                                                                                                       \
        XrResult result = (x);                                                                                                                              \
        if (!XR_SUCCEEDED(result)) {                                                                                                                        \
            std::cerr << "ERROR: OPENXR: " << int(result) << "(" << (m_xrInstance ? GetXRErrorString(m_xrInstance, result) : "") << ") " << y << std::endl; \
            OpenXRDebugBreak();                                                                                                                             \
        }                                                                                                                                                   \
    }

// XR_DOCS_TAG_END_Helper_Functions0