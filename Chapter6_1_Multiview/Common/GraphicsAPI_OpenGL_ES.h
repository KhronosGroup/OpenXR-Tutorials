// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#pragma once
#include <GraphicsAPI.h>

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
class GraphicsAPI_OpenGL_ES : public GraphicsAPI {
public:
    GraphicsAPI_OpenGL_ES();
    GraphicsAPI_OpenGL_ES(XrInstance m_xrInstance, XrSystemId systemId);
    ~GraphicsAPI_OpenGL_ES();

    void* CreateDesktopSwapchain(const SwapchainCreateInfo& swapchainCI) override;
    void DestroyDesktopSwapchain(void*& swapchain) override;
    void* GetDesktopSwapchainImage(void* swapchain, uint32_t index) override;
    void AcquireDesktopSwapchanImage(void* swapchain, uint32_t& index) override;
    void PresentDesktopSwapchainImage(void* swapchain, uint32_t index) override;

    // XR_DOCS_TAG_BEGIN_GetDepthFormat_OpenGL_ES
    int64_t GetDepthFormat() override { return (int64_t)GL_DEPTH_COMPONENT32F; }
    // XR_DOCS_TAG_END_GetDepthFormat_OpenGL_ES
    size_t AlignSizeForUniformBuffer(size_t size) override;

    void* GetGraphicsBinding() override;
    XrSwapchainImageBaseHeader* AllocateSwapchainImageData(XrSwapchain swapchain, SwapchainType type, uint32_t count) override;
    void FreeSwapchainImageData(XrSwapchain swapchain) override {
        swapchainImagesMap[swapchain].second.clear();
        swapchainImagesMap.erase(swapchain);
    }
    XrSwapchainImageBaseHeader* GetSwapchainImageData(XrSwapchain swapchain, uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImagesMap[swapchain].second[index]; }
    // XR_DOCS_TAG_BEGIN_GetSwapchainImage_OpenGL_ES
    void* GetSwapchainImage(XrSwapchain swapchain, uint32_t index) override { return (void*)(uint64_t)swapchainImagesMap[swapchain].second[index].image; }
    // XR_DOCS_TAG_END_GetSwapchainImage_OpenGL_ES

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

    void ClearColor(void* imageView, float r, float g, float b, float a) override;
    void ClearDepth(void* imageView, float d) override;

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
    ksGpuWindow window{};

    PFN_xrGetOpenGLESGraphicsRequirementsKHR xrGetOpenGLESGraphicsRequirementsKHR = nullptr;
    XrGraphicsBindingOpenGLESAndroidKHR graphicsBinding{};

    std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageOpenGLESKHR>>> swapchainImagesMap{};

    std::unordered_map<GLuint, BufferCreateInfo> buffers{};
    std::unordered_map<GLuint, ImageCreateInfo> images{};
    std::unordered_map<GLuint, ImageViewCreateInfo> imageViews{};

    GLuint setFramebuffer = 0;
    std::unordered_map<GLuint, PipelineCreateInfo> pipelines{};
    GLuint setPipeline = 0;
    GLuint vertexArray = 0;
    GLuint setIndexBuffer = 0;
};
#endif
