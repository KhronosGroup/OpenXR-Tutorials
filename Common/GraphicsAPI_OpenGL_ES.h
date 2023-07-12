#pragma once
#include "GraphicsAPI.h"

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