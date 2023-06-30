#pragma once

#include <iostream>
#include <sstream>
#include <vector>

// Platform headers
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "unknwn.h"
#define XR_USE_PLATFORM_WIN32

#define XR_USE_GRAPHICS_API_D3D11
#define XR_USE_GRAPHICS_API_D3D12
#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_GRAPHICS_API_VULKAN
#endif

#if defined(__LINIX__)
#define XR_USE_PLATFORM_XLIB
#define XR_USE_PLATFORM_XCB
#define XR_USE_PLATFORM_WAYLAND

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_GRAPHICS_API_VULKAN
#endif

#if defined(__ANDROID__)
#include "android_native_app_glue.h"
#define XR_USE_PLATFORM_ANDROID

#define XR_USE_GRAPHICS_API_OPENGL_ES
#define XR_USE_GRAPHICS_API_VULKAN
#endif

// Graphic APIs headers
#if defined(XR_USE_GRAPHICS_API_D3D11)
#include <d3d11.h>
#endif

#if defined(XR_USE_GRAPHICS_API_D3D12)
#include <d3d12.h>
#include <dxgi1_6.h>
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL)
// gfxwrapper will redefine these macros
#undef XR_USE_PLATFORM_WIN32
#undef XR_USE_PLATFORM_XLIB
#undef XR_USE_PLATFORM_XCB
#undef XR_USE_PLATFORM_WAYLAND
#include "gfxwrapper_opengl.h"
#endif

#if defined(XR_USE_GRAPHICS_API_VULKAN)
#include "vulkan/vulkan.h"
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
#include "gfxwrapper_opengl.h"
#endif

// OpenXR
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

enum GraphicsAPI_Type : uint8_t {
    UNKNOWN,
    D3D11,
    D3D12,
    OPENGL,
    OPENGL_ES,
    VULKAN
};

bool CheckGraphicsAPI_TypeIsValidForPlatform(GraphicsAPI_Type type);

#if defined(XR_USE_GRAPHICS_API_D3D11)
class GraphicsAPI_D3D11 {
public:
    GraphicsAPI_D3D11(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_D3D11();

public:
    IDXGIFactory1* factory = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;

private:
    PFN_xrGetD3D11GraphicsRequirementsKHR xrGetD3D11GraphicsRequirementsKHR = nullptr;
};
#endif

#if defined(XR_USE_GRAPHICS_API_D3D12)
class GraphicsAPI_D3D12 {
public:
    GraphicsAPI_D3D12(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_D3D12();

public:
    IDXGIFactory4* factory = nullptr;
    ID3D12Device* device = nullptr;
    ID3D12CommandQueue* queue = nullptr;

private:
    PFN_xrGetD3D12GraphicsRequirementsKHR xrGetD3D12GraphicsRequirementsKHR = nullptr;
};
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL)
class GraphicsAPI_OpenGL {
public:
    GraphicsAPI_OpenGL(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_OpenGL();

    ksGpuWindow window{};

private:
    PFN_xrGetOpenGLGraphicsRequirementsKHR xrGetOpenGLGraphicsRequirementsKHR = nullptr;
};
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
class GraphicsAPI_OpenGL_ES {
public:
    GraphicsAPI_OpenGL_ES(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_OpenGL_ES();

    ksGpuWindow window{};

private:
    PFN_xrGetOpenGLESGraphicsRequirementsKHR xrGetOpenGLESGraphicsRequirementsKHR = nullptr;
};
#endif

#if defined(XR_USE_GRAPHICS_API_VULKAN)
class GraphicsAPI_Vulkan {
public:
    GraphicsAPI_Vulkan(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_Vulkan();

private:
    void LoadPFN_XrFunctions(XrInstance xrInstance);
    std::vector<std::string> GetInstanceExtensionsForOpenXR(XrInstance xrInstance, XrSystemId systemId);
    std::vector<std::string> GetDeviceExtensionsForOpenXR(XrInstance xrInstance, XrSystemId systemId);

public:
    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    uint32_t queueFamilyIndex = 0;
    uint32_t queueIndex = 0;

    std::vector<const char*> activeInstanceExtensions{};
    std::vector<const char*> activeDeviceExtensions{};

private:
    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR = nullptr;
};
#endif

// XR_DOCS_TAG_BEGIN_Helper_Functions0
#define OPENXR_CHECK(x, y)                                                       \
    {                                                                            \
        XrResult result = (x);                                                   \
        if (!XR_SUCCEEDED(result)) {                                             \
            std::cout << "ERROR: OPENXR: (" << result << ") " << y << std::endl; \
        }                                                                        \
    }
// XR_DOCS_TAG_END_Helper_Functions0