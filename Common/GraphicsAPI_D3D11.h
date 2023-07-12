#pragma once
#include "GraphicsAPI.h"

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