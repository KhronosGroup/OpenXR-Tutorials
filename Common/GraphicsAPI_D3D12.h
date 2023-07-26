#pragma once
#include "GraphicsAPI.h"

#if defined(XR_USE_GRAPHICS_API_D3D12)
class GraphicsAPI_D3D12 : public GraphicsAPI {
public:
    GraphicsAPI_D3D12(XrInstance m_xrInstance, XrSystemId systemId);
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

    virtual void ClearColor(void* imageView, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* imageView, float d) override;

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