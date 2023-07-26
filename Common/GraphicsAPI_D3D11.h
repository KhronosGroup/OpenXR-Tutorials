#pragma once
#include "GraphicsAPI.h"

#if defined(XR_USE_GRAPHICS_API_D3D11)
class GraphicsAPI_D3D11 : public GraphicsAPI {
public:
    GraphicsAPI_D3D11(XrInstance m_xrInstance, XrSystemId systemId);
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

    virtual void* CreateSampler(const SamplerCreateInfo& samplerCI) override;
    virtual void DestroySampler(void*& sampler) override;

    virtual void* CreateBuffer(const BufferCreateInfo& bufferCI) override;
    virtual void DestroyBuffer(void*& buffer) override;

    virtual void* CreateShader(const ShaderCreateInfo& shaderCI) override;
    virtual void DestroyShader(void*& shader) override;

    virtual void* CreatePipeline(const PipelineCreateInfo& pipelineCI) override;
    virtual void DestroyPipeline(void*& pipeline) override;

    virtual void BeginRendering();
    virtual void EndRendering();

    virtual void SetBufferData(void* buffer, size_t offset, size_t size, void* data) override;

    virtual void ClearColor(void* image, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* image, float d) override;

    virtual void SetRenderAttachments(void** colorViews, size_t colorViewCount, void* depthStencilView) override;
    virtual void SetViewports(Viewport* viewports, size_t count) override;
    virtual void SetScissors(Rect2D* scissors, size_t count) override;

    virtual void SetPipeline(void* pipeline) override;
    virtual void SetDescriptor(const DescriptorInfo& descriptorInfo) override;
    virtual void SetVertexBuffers(void** vertexBuffers, size_t count) override;
    virtual void SetIndexBuffer(void* indexBuffer) override;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) override;
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;

private:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    IDXGIFactory1* factory = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;

    PFN_xrGetD3D11GraphicsRequirementsKHR xrGetD3D11GraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingD3D11KHR graphicsBinding{};

    std::vector<XrSwapchainImageD3D11KHR> swapchainImages{};

    std::unordered_map<ID3D11Buffer*, BufferCreateInfo> buffers;

    std::unordered_map<ID3D11DeviceChild*, ID3DBlob*> shaderCompiledBinaries;
    std::unordered_map<UINT64, PipelineCreateInfo> pipelines;
    UINT64 setPipeline = 0;
};
#endif