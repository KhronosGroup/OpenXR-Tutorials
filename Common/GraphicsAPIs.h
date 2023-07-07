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

#if defined(XR_USE_GRAPHICS_API_VULKAN)
#include "vulkan/vulkan.h"
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
#include "gfxwrapper_opengl.h"
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

#if defined(XR_USE_GRAPHICS_API_D3D11)
class GraphicsAPI_D3D11 : public GraphicsAPI {
public:
    GraphicsAPI_D3D11(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_D3D11();

    virtual int64_t GetDepthFormat() override { return (int64_t)DXGI_FORMAT_D32_FLOAT; }

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override { return swapchainImages[index].texture; }

    virtual void* CreateImage(const ImageCreateInfo& imageCI) override;
    virtual void DestroyImage(void*& image) override;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    virtual void DestroyImageView(void*& imageView) override;

    virtual void ClearColor(void* image, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* image, float d) override;

private:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    IDXGIFactory1* factory = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;

    PFN_xrGetD3D11GraphicsRequirementsKHR xrGetD3D11GraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingD3D11KHR graphicsBinding{};

    std::vector<XrSwapchainImageD3D11KHR> swapchainImages{};
};
#endif

#if defined(XR_USE_GRAPHICS_API_D3D12)
class GraphicsAPI_D3D12 : public GraphicsAPI {
public:
    GraphicsAPI_D3D12(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_D3D12();

    virtual int64_t GetDepthFormat() override { return (int64_t)DXGI_FORMAT_D32_FLOAT; }

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override { return swapchainImages[index].texture; }

    virtual void* CreateImage(const ImageCreateInfo& imageCI) override;
    virtual void DestroyImage(void*& image) override;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    virtual void DestroyImageView(void*& imageView) override;

    virtual void BeginRendering() override;
    virtual void EndRendering() override;

    virtual void ClearColor(void* image, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* image, float d) override;

private:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    IDXGIFactory4* factory = nullptr;
    ID3D12Device* device = nullptr;
    ID3D12CommandQueue* queue = nullptr;

    ID3D12CommandAllocator* cmdAllocator = nullptr;
    ID3D12GraphicsCommandList* cmdList = nullptr;

    PFN_xrGetD3D12GraphicsRequirementsKHR xrGetD3D12GraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingD3D12KHR graphicsBinding{};

    std::vector<XrSwapchainImageD3D12KHR> swapchainImages{};

    std::unordered_map<ID3D12Resource*, ID3D12Heap*> imageResources;
    std::unordered_map<SIZE_T, ID3D12DescriptorHeap*> imageViewResources;
};
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL)
class GraphicsAPI_OpenGL : public GraphicsAPI {
public:
    GraphicsAPI_OpenGL(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_OpenGL();

    virtual int64_t GetDepthFormat() override { return (int64_t)GL_DEPTH_COMPONENT32F; }

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override { return (void*)(uint64_t)swapchainImages[index].image; }

    virtual void* CreateImage(const ImageCreateInfo& imageCI) override;
    virtual void DestroyImage(void*& image) override;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    virtual void DestroyImageView(void*& imageView) override;

    virtual void ClearColor(void* image, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* image, float d) override;

private:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    ksGpuWindow window{};

    PFN_xrGetOpenGLGraphicsRequirementsKHR xrGetOpenGLGraphicsRequirementsKHR = nullptr;
#if defined(XR_USE_PLATFORM_WIN32)
    XrGraphicsBindingOpenGLWin32KHR graphicsBinding{};
#elif defined(XR_USE_PLATFORM_XLIB)
    XrGraphicsBindingOpenGLXlibKHR graphicsBinding{};
#elif defined(XR_USE_PLATFORM_XCB)
    XrGraphicsBindingOpenGLXcbKHR graphicsBinding{};
#elif defined(XR_USE_PLATFORM_WAYLAND)
    XrGraphicsBindingOpenGLWaylandKHR graphicsBinding{};
#endif

    std::vector<XrSwapchainImageOpenGLKHR> swapchainImages{};
};
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
class GraphicsAPI_OpenGL_ES : public GraphicsAPI {
public:
    GraphicsAPI_OpenGL_ES(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_OpenGL_ES();

    virtual int64_t GetDepthFormat() override { return (int64_t)GL_DEPTH_COMPONENT32F; }

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override { return (void*)(uint64_t)swapchainImages[index].image; }

    virtual void* CreateImage(const ImageCreateInfo& imageCI) override;
    virtual void DestroyImage(void*& image) override;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    virtual void DestroyImageView(void*& imageView) override;

    virtual void ClearColor(void* image, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* image, float d) override;

private:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    ksGpuWindow window{};

    PFN_xrGetOpenGLESGraphicsRequirementsKHR xrGetOpenGLESGraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingOpenGLESAndroidKHR graphicsBinding{};

    std::vector<XrSwapchainImageOpenGLESKHR> swapchainImages{};
};
#endif

#if defined(XR_USE_GRAPHICS_API_VULKAN)
class GraphicsAPI_Vulkan : public GraphicsAPI {
public:
    GraphicsAPI_Vulkan(XrInstance xrInstance, XrSystemId systemId);
    ~GraphicsAPI_Vulkan();

    virtual int64_t GetDepthFormat() override { return (int64_t)VK_FORMAT_D32_SFLOAT; }

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override { return (void*)swapchainImages[index].image; }

    virtual void* CreateImage(const ImageCreateInfo& imageCI) override;
    virtual void DestroyImage(void*& image) override;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    virtual void DestroyImageView(void*& imageView) override;

    virtual void BeginRendering() override;
    virtual void EndRendering() override;

    virtual void ClearColor(void* image, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* image, float d) override;

private:
    void LoadPFN_XrFunctions(XrInstance xrInstance);
    std::vector<std::string> GetInstanceExtensionsForOpenXR(XrInstance xrInstance, XrSystemId systemId);
    std::vector<std::string> GetDeviceExtensionsForOpenXR(XrInstance xrInstance, XrSystemId systemId);

    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    uint32_t queueFamilyIndex = 0;
    uint32_t queueIndex = 0;

    VkCommandPool cmdPool{};
    VkCommandBuffer cmdBuffer{};

    std::vector<const char*> activeInstanceExtensions{};
    std::vector<const char*> activeDeviceExtensions{};

    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR = nullptr;
    XrGraphicsBindingVulkanKHR graphicsBinding{};

    std::vector<XrSwapchainImageVulkanKHR> swapchainImages{};

    std::unordered_map<VkImage, VkDeviceMemory> imageResources;
    std::unordered_map<VkImageView, ImageViewCreateInfo> imageViewResources;
};
#endif