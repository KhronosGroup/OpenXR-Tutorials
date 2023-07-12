#pragma once
#include "GraphicsAPI.h"

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