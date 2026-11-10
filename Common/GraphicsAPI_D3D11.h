// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#pragma once
#include <GraphicsAPI.h>

#if defined(XR_USE_GRAPHICS_API_D3D11)
class GraphicsAPI_D3D11 : public GraphicsAPI {
public:
    GraphicsAPI_D3D11();
    GraphicsAPI_D3D11(XrInstance m_xrInstance, XrSystemId systemId);
    ~GraphicsAPI_D3D11() override;

    void* CreateDesktopSwapchain(const SwapchainCreateInfo& swapchainCI) override;
    void DestroyDesktopSwapchain(void*& swapchain) override;
    void* GetDesktopSwapchainImage(void* swapchain, uint32_t index) override;
    void AcquireDesktopSwapchanImage(void* swapchain, uint32_t& index) override;
    void PresentDesktopSwapchainImage(void* swapchain, uint32_t index) override;

    // XR_DOCS_TAG_BEGIN_GetDepthFormat_D3D11
    int64_t GetDepthFormat() override { return (int64_t)DXGI_FORMAT_D32_FLOAT; }
    // XR_DOCS_TAG_END_GetDepthFormat_D3D11

    void* GetGraphicsBinding() override;
    XrSwapchainImageBaseHeader* AllocateSwapchainImageData(XrSwapchain swapchain, SwapchainType type, uint32_t count) override;
    void FreeSwapchainImageData(XrSwapchain swapchain) override {
        swapchainImagesMap[swapchain].second.clear();
        swapchainImagesMap.erase(swapchain);
    }
    XrSwapchainImageBaseHeader* GetSwapchainImageData(XrSwapchain swapchain, uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImagesMap[swapchain].second[index]; }
    // XR_DOCS_TAG_BEGIN_GetSwapchainImage_D3D11
    void* GetSwapchainImage(XrSwapchain swapchain, uint32_t index) override { return swapchainImagesMap[swapchain].second[index].texture; }
    // XR_DOCS_TAG_END_GetSwapchainImage_D3D11

    void* CreateImage(const ImageCreateInfo& imageCI) override;
    void DestroyImage(void*& image) override;

    void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    void DestroyImageView(void*& imageView) override;

    void* CreateSampler(const SamplerCreateInfo& samplerCI) override;
    void DestroySampler(void*& sampler) override;

    void* CreateBuffer(const BufferCreateInfo& bufferCI) override;
    void DestroyBuffer(void*& buffer) override;

    void* CreateShader(const ShaderCreateInfo& shaderCI) override;
    void DestroyShader(void*& shader) override;

    void* CreatePipeline(const PipelineCreateInfo& pipelineCI) override;
    void DestroyPipeline(void*& pipeline) override;

    void BeginRendering() override;
    void EndRendering() override;

    void SetBufferData(void* buffer, size_t offset, size_t size, void* data) override;

    void ClearColor(void* image, float r, float g, float b, float a) override;
    void ClearDepth(void* image, float d) override;

    void SetRenderAttachments(void** colorViews, size_t colorViewCount, void* depthStencilView, uint32_t width, uint32_t height, void* pipeline) override;
    void SetViewports(Viewport* viewports, size_t count) override;
    void SetScissors(Rect2D* scissors, size_t count) override;

    void SetPipeline(void* pipeline) override;
    void SetDescriptor(const DescriptorInfo& descriptorInfo) override;
    void UpdateDescriptors() override;
    void SetVertexBuffers(void** vertexBuffers, size_t count) override;
    void SetIndexBuffer(void* indexBuffer) override;
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) override;
    void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;

private:
    const std::vector<int64_t> GetSupportedColorSwapchainFormats() override;
    const std::vector<int64_t> GetSupportedDepthSwapchainFormats() override;

private:
    IDXGIFactory4* factory = nullptr;
    ID3D11Device* device = nullptr;

    ID3D11Debug* d3dDebug = nullptr;
    ID3D11InfoQueue* infoQueue = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;

    PFN_xrGetD3D11GraphicsRequirementsKHR xrGetD3D11GraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingD3D11KHR graphicsBinding{};

    std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageD3D11KHR>>> swapchainImagesMap{};

    std::unordered_map<ID3D11Buffer*, BufferCreateInfo> buffers;

    std::unordered_map<ID3D11DeviceChild*, std::vector<char>> shaderCompiledBinaries;
    std::unordered_map<UINT64, PipelineCreateInfo> pipelines;
    UINT64 setPipeline = 0;
};
#endif
