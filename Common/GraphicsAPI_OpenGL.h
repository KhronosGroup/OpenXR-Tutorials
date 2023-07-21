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

    virtual void ClearColor(void* imageView, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* imageView, float d) override;

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