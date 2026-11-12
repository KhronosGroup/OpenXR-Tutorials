// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#pragma once
#include <GraphicsAPI.h>

#if defined(XR_USE_GRAPHICS_API_D3D12)
class GraphicsAPI_D3D12 : public GraphicsAPI {
public:
    GraphicsAPI_D3D12();
    GraphicsAPI_D3D12(XrInstance m_xrInstance, XrSystemId systemId);
    ~GraphicsAPI_D3D12() override;

    // XR_DOCS_TAG_BEGIN_GetDepthFormat_D3D12
    int64_t GetDepthFormat() override { return (int64_t)DXGI_FORMAT_D32_FLOAT; }
    // XR_DOCS_TAG_END_GetDepthFormat_D3D12
    size_t AlignSizeForUniformBuffer(size_t size) override { return Align<size_t>(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT); }

    void* CreateDesktopSwapchain(const SwapchainCreateInfo& swapchainCI) override;
    void DestroyDesktopSwapchain(void*& swapchain) override;
    void* GetDesktopSwapchainImage(void* swapchain, uint32_t index) override;
    void AcquireDesktopSwapchanImage(void* swapchain, uint32_t& index) override;
    void PresentDesktopSwapchainImage(void* swapchain, uint32_t index) override;

    void* GetGraphicsBinding() override;
    XrSwapchainImageBaseHeader* AllocateSwapchainImageData(XrSwapchain swapchain, SwapchainType type, uint32_t count) override;
    void FreeSwapchainImageData(XrSwapchain swapchain) override {
        swapchainImagesMap[swapchain].second.clear();
        swapchainImagesMap.erase(swapchain);
    }
    XrSwapchainImageBaseHeader* GetSwapchainImageData(XrSwapchain swapchain, uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImagesMap[swapchain].second[index]; }
    // XR_DOCS_TAG_BEGIN_GetSwapchainImage_D3D12
    void* GetSwapchainImage(XrSwapchain swapchain, uint32_t index) override {
        ID3D12Resource* image = swapchainImagesMap[swapchain].second[index].texture;
        D3D12_RESOURCE_STATES state = swapchainImagesMap[swapchain].first == SwapchainType::COLOR ? D3D12_RESOURCE_STATE_RENDER_TARGET : D3D12_RESOURCE_STATE_DEPTH_WRITE;
        imageStates[image] = state;
        return image;
    }
    // XR_DOCS_TAG_END_GetSwapchainImage_D3D12

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

    void ClearColor(void* imageView, float r, float g, float b, float a) override;
    void ClearDepth(void* imageView, float d) override;

    void SetBufferData(void* buffer, size_t offset, size_t size, void* data) override;

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
    ID3D12Debug* debug = nullptr;
    IDXGIFactory4* factory = nullptr;
    ID3D12Device* device = nullptr;
    ID3D12CommandQueue* queue = nullptr;

    ID3D12CommandAllocator* cmdAllocator = nullptr;
    ID3D12GraphicsCommandList* cmdList = nullptr;

    PFN_xrGetD3D12GraphicsRequirementsKHR xrGetD3D12GraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingD3D12KHR graphicsBinding{};

    std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageD3D12KHR>>> swapchainImagesMap{};

    ID3D12Resource* currentDesktopSwapchainImage = nullptr;

    std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> imageStates;

    std::unordered_map<ID3D12Resource*, ID3D12Heap*> imageResources;
    std::unordered_map<SIZE_T, std::pair<ID3D12DescriptorHeap*, ID3D12Resource*>> imageViewResources;
    std::unordered_map<SIZE_T, ID3D12DescriptorHeap*> samplerResources;

    std::unordered_map<ID3D12Resource*, std::pair<ID3D12Heap*, BufferCreateInfo>> bufferResources;

    std::unordered_map<D3D12_SHADER_BYTECODE*, std::pair<std::vector<char>, ShaderCreateInfo>> shaders;

    std::vector<DescriptorInfo> descriptorInfos = {};
    ID3D12DescriptorHeap* CBV_SRV_UAV_DescriptorHeap = nullptr;
    ID3D12DescriptorHeap* SAMPLER_DescriptorHeap = nullptr;
    UINT CBV_SRV_UAV_DescriptorOffset = 0;
    UINT SAMPLER_DescriptorOffset = 0;
    bool setDescriptorHeap = true;

    std::unordered_map<ID3D12PipelineState*, std::pair<ID3D12RootSignature*, PipelineCreateInfo>> pipelineResources;
    ID3D12PipelineState* setPipeline = nullptr;
};
#endif
