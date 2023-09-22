// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: MIT

// OpenXR Tutorial for Khronos Group

#pragma once
#include <GraphicsAPI.h>

#if defined(XR_USE_GRAPHICS_API_VULKAN)
class GraphicsAPI_Vulkan : public GraphicsAPI {
public:
    GraphicsAPI_Vulkan();
    GraphicsAPI_Vulkan(XrInstance m_xrInstance, XrSystemId systemId);
    ~GraphicsAPI_Vulkan();

    virtual void* CreateDesktopSwapchain(const SwapchainCreateInfo& swapchainCI) override;
    virtual void DestroyDesktopSwapchain(void*& swapchain) override;
    virtual void* GetDesktopSwapchainImage(void* swapchain, uint32_t index) override;
    virtual void AcquireDesktopSwapchanImage(void* swapchain, uint32_t& index) override;
    virtual void PresentDesktopSwapchainImage(void* swapchain, uint32_t index) override;

    virtual int64_t GetDepthFormat() override { return (int64_t)VK_FORMAT_D32_SFLOAT; }

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override {
        VkImage image = swapchainImages[index].image;
        imageStates[image] = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        return (void *)image;
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

    virtual void SetBufferData(void* buffer, size_t offset, size_t size, void* data) override;

    virtual void ClearColor(void* imageView, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* imageView, float d) override;

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
    void LoadPFN_XrFunctions(XrInstance m_xrInstance);
    std::vector<std::string> GetInstanceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId);
    std::vector<std::string> GetDeviceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId);

    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() override;

private:
    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    uint32_t queueFamilyIndex = 0xFFFFFFFF;
    uint32_t queueIndex = 0xFFFFFFFF;

    VkCommandPool cmdPool{};
    VkCommandBuffer cmdBuffer{};

    std::vector<const char*> activeInstanceLayers{};
    std::vector<const char*> activeInstanceExtensions{};
    std::vector<const char*> activeDeviceLayer{};
    std::vector<const char*> activeDeviceExtensions{};

    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR = nullptr;
    XrGraphicsBindingVulkanKHR graphicsBinding{};

    std::vector<XrSwapchainImageVulkanKHR> swapchainImages{};

    VkImage currentDesktopSwapchainImage = VK_NULL_HANDLE;

    std::unordered_map<VkSwapchainKHR, VkSurfaceKHR> surfaces;
    VkSemaphore acquireSemaphore{};
    VkSemaphore submitSemaphore{};

    std::unordered_map<VkImage, VkImageLayout> imageStates;
    std::unordered_map<VkImage, std::pair<VkDeviceMemory, ImageCreateInfo>> imageResources;
    std::unordered_map<VkImageView, ImageViewCreateInfo> imageViewResources;
    
    std::unordered_map<VkBuffer, std::pair<VkDeviceMemory, BufferCreateInfo>> bufferResources;

    std::unordered_map<VkShaderModule, ShaderCreateInfo> shaderResources;
    std::unordered_map<VkPipeline, std::tuple<VkPipelineLayout, VkDescriptorSetLayout, VkRenderPass, PipelineCreateInfo>> pipelineResources;

    std::unordered_map<VkCommandBuffer, std::vector<VkFramebuffer>> cmdBufferFramebuffers;
    bool inRenderPass = false;

    VkPipeline setPipeline = VK_NULL_HANDLE;
    std::unordered_map<VkCommandBuffer, std::vector<std::pair<VkDescriptorPool, VkDescriptorSet>>> cmdBufferDescriptorSets;
    std::vector<std::tuple<VkWriteDescriptorSet, VkDescriptorBufferInfo, VkDescriptorImageInfo>> writeDescSets;

};
#endif