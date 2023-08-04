#include "GraphicsAPI.h"

bool CheckGraphicsAPI_TypeIsValidForPlatform(GraphicsAPI_Type type) {
#if defined(XR_USE_PLATFORM_WIN32)
    return (type == D3D11) || (type == D3D12) || (type == OPENGL) || (type == VULKAN);
#endif
#if defined(XR_USE_PLATFORM_XLIB) || defined(XR_USE_PLATFORM_XCB) || defined(XR_USE_PLATFORM_WAYLAND)
    return (type == OPENGL) || (type == VULKAN);
#endif
#if defined(XR_USE_PLATFORM_ANDROID) || defined(XR_USE_PLATFORM_XCB) || defined(XR_USE_PLATFORM_WAYLAND)
    return (type == OPENGL_ES) || (type == VULKAN);
#endif
	return false;
}

const char *GetGraphicsAPIInstanceExtensionString(GraphicsAPI_Type type) {
    if (type == D3D11) {
#if defined(XR_USE_GRAPHICS_API_D3D11)
        return XR_KHR_D3D11_ENABLE_EXTENSION_NAME;
#endif
    } else if (type == D3D12) {
#if defined(XR_USE_GRAPHICS_API_D3D12)
        return XR_KHR_D3D12_ENABLE_EXTENSION_NAME;
#endif
    } else if (type == OPENGL) {
#if defined(XR_USE_GRAPHICS_API_OPENGL)
        return XR_KHR_OPENGL_ENABLE_EXTENSION_NAME;
#endif
    } else if (type == OPENGL_ES) {
#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
        return XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME;
#endif
    } else if (type == VULKAN) {
#if defined(XR_USE_GRAPHICS_API_VULKAN)
        return XR_KHR_VULKAN_ENABLE_EXTENSION_NAME;
#endif
    } else {
        std::cout << "ERROR: Unknown Graphics API." << std::endl;
        DEBUG_BREAK;
        return nullptr;
    }
    return nullptr;
}

// GraphicsAPI

// XR_DOCS_TAG_BEGIN_GraphicsAPI_SelectSwapchainFormat
int64_t GraphicsAPI::SelectSwapchainFormat(const std::vector<int64_t> &formats) {
    const std::vector<int64_t> &supportSwapchainFormats = GetSupportedSwapchainFormats();

    const std::vector<int64_t>::const_iterator &swapchainFormatIt = std::find_first_of(formats.begin(), formats.end(),
                                                                                       std::begin(supportSwapchainFormats), std::end(supportSwapchainFormats));
    if (swapchainFormatIt == formats.end()) {
        std::cout << "ERROR: Unable to find supported Swapchain Format" << std::endl;
        DEBUG_BREAK;
        return 0;
    }

    return *swapchainFormatIt;
}
// XR_DOCS_TAG_END_GraphicsAPI_SelectSwapchainFormat