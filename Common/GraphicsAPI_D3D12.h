#pragma once
#include <GraphicsAPI.h>

#if defined(XR_USE_GRAPHICS_API_D3D12)
class GraphicsAPI_D3D12 : public GraphicsAPI {
public:
    GraphicsAPI_D3D12();
    GraphicsAPI_D3D12(XrInstance m_xrInstance, XrSystemId systemId);
    ~GraphicsAPI_D3D12();

    virtual int64_t GetDepthFormat() override { return (int64_t)DXGI_FORMAT_D32_FLOAT; }

    virtual void* CreateDesktopSwapchain(const SwapchainCreateInfo& swapchainCI) override;
    virtual void DestroyDesktopSwapchain(void*& swapchain) override;
    virtual void* GetDesktopSwapchainImage(void* swapchain, uint32_t index) override;
    virtual void AcquireDesktopSwapchanImage(void* swapchain, uint32_t& index) override;
    virtual void PresentDesktopSwapchainImage(void* swapchain, uint32_t index) override;

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override { 
        ID3D12Resource* image = swapchainImages[index].texture;
        imageStates[image] = D3D12_RESOURCE_STATE_RENDER_TARGET;
        return image;
    }

    virtual void* CreateImage(const ImageCreateInfo& imageCI) override;
    virtual void DestroyImage(void*& image) override;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    virtual void DestroyImageView(void*& imageView) override;

    virtual void* CreateSampler(const SamplerCreateInfo& samplerCI) override;
    virtual void DestroySampler(void*& sampler) override;

    virtual void* CreateBuffer(const BufferCreateInfo& bufferCI) override;
    virtual void DestroyBuffer(void*& buffer) override;

    virtual void* CreateShader(const ShaderCreateInfo& shaderCI) override;
    virtual void DestroyShader(void*& shader) override;

    virtual void* CreatePipeline(const PipelineCreateInfo& pipelineCI) override;
    virtual void DestroyPipeline(void*& pipeline) override;

    virtual void BeginRendering() override;
    virtual void EndRendering() override;

    virtual void ClearColor(void* imageView, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* imageView, float d) override;

    virtual void SetBufferData(void* buffer, size_t offset, size_t size, void* data) override;

    virtual void SetRenderAttachments(void** colorViews, size_t colorViewCount, void* depthStencilView, uint32_t width, uint32_t height, void* pipeline) override;
    virtual void SetViewports(Viewport* viewports, size_t count) override;
    virtual void SetScissors(Rect2D* scissors, size_t count) override;

    virtual void SetPipeline(void* pipeline) override;
    virtual void SetDescriptor(const DescriptorInfo& descriptorInfo) override;
    virtual void UpdateDescriptors() override;
    virtual void SetVertexBuffers(void** vertexBuffers, size_t count) override;
    virtual void SetIndexBuffer(void* indexBuffer) override;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) override;
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;

private:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    ID3D12Debug* debug = nullptr;
    IDXGIFactory4* factory = nullptr;
    ID3D12Device* device = nullptr;
    ID3D12CommandQueue* queue = nullptr;

    ID3D12CommandAllocator* cmdAllocator = nullptr;
    ID3D12GraphicsCommandList* cmdList = nullptr;

    PFN_xrGetD3D12GraphicsRequirementsKHR xrGetD3D12GraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingD3D12KHR graphicsBinding{};

    std::vector<XrSwapchainImageD3D12KHR> swapchainImages{};

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