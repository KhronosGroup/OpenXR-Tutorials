#pragma once
#include "HelperFunctions.h"

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

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
#include "gfxwrapper_opengl.h"
#endif

#if defined(XR_USE_GRAPHICS_API_VULKAN)
#include "vulkan/vulkan.h"
#endif

// OpenXR
#include "OpenXRHelper.h"

enum GraphicsAPI_Type : uint8_t {
    UNKNOWN,
    D3D11,
    D3D12,
    OPENGL,
    OPENGL_ES,
    VULKAN
};

bool CheckGraphicsAPI_TypeIsValidForPlatform(GraphicsAPI_Type type);

const char* GetGraphicsAPIInstanceExtensionString(GraphicsAPI_Type type);

class GraphicsAPI {
public:
    struct ImageCreateInfo {
        uint32_t dimension;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        uint32_t sampleCount;
        int64_t format;
        bool cubemap;
        bool colorAttachment;
        bool depthAttachment;
        bool sampled;
    };
    struct ImageViewCreateInfo {
        void* image;
        enum class Type : uint8_t {
            RTV,
            DSV,
            SRV,
            UAV
        } type;
        enum class View : uint8_t {
            TYPE_1D,
            TYPE_2D,
            TYPE_3D,
            TYPE_CUBE,
            TYPE_1D_ARRAY,
            TYPE_2D_ARRAY,
            TYPE_CUBE_ARRAY,
        } view;
        int64_t format;
        enum class Aspect : uint8_t {
            COLOR_BIT = 0x01,
            DEPTH_BIT = 0x02,
            STENCIL_BIT = 0x04
        } aspect;
        uint32_t baseMipLevel;
        uint32_t levelCount;
        uint32_t baseArrayLayer;
        uint32_t layerCount;
    };

public:
    virtual ~GraphicsAPI() = default;

    int64_t SelectSwapchainFormat(const std::vector<int64_t>& formats);

    virtual int64_t GetDepthFormat() = 0;

    virtual void* GetGraphicsBinding() = 0;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) = 0;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) = 0;
    virtual void* GetSwapchainImage(uint32_t index) = 0;

    virtual void* CreateImage(const ImageCreateInfo& imageCI) = 0;
    virtual void DestroyImage(void*& image) = 0;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) = 0;
    virtual void DestroyImageView(void*& imageView) = 0;

    virtual void BeginRendering(){};
    virtual void EndRendering(){};

    virtual void ClearColor(void* imageView, float r, float g, float b, float a) = 0;
    virtual void ClearDepth(void* imageView, float d) = 0;

protected:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() = 0;
};