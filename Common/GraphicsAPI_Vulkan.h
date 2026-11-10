// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#pragma once
#include <GraphicsAPI.h>

#if defined(XR_USE_GRAPHICS_API_VULKAN)
class GraphicsAPI_Vulkan : public GraphicsAPI {
public:
    GraphicsAPI_Vulkan();
    GraphicsAPI_Vulkan(XrInstance m_xrInstance, XrSystemId systemId);
    ~GraphicsAPI_Vulkan();

    void* CreateDesktopSwapchain(const SwapchainCreateInfo& swapchainCI) override;
    void DestroyDesktopSwapchain(void*& swapchain) override;
    void* GetDesktopSwapchainImage(void* swapchain, uint32_t index) override;
    void AcquireDesktopSwapchanImage(void* swapchain, uint32_t& index) override;
    void PresentDesktopSwapchainImage(void* swapchain, uint32_t index) override;

    // XR_DOCS_TAG_BEGIN_GetDepthFormat_Vulkan
    int64_t GetDepthFormat() override { return (int64_t)VK_FORMAT_D32_SFLOAT; }
    // XR_DOCS_TAG_END_GetDepthFormat_Vulkan

    void* GetGraphicsBinding() override;
    XrSwapchainImageBaseHeader* AllocateSwapchainImageData(XrSwapchain swapchain, SwapchainType type, uint32_t count) override;
    void FreeSwapchainImageData(XrSwapchain swapchain) override {
        swapchainImagesMap[swapchain].second.clear();
        swapchainImagesMap.erase(swapchain);
    }
    XrSwapchainImageBaseHeader* GetSwapchainImageData(XrSwapchain swapchain, uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImagesMap[swapchain].second[index]; }
    // XR_DOCS_TAG_BEGIN_GetSwapchainImage_Vulkan
    void* GetSwapchainImage(XrSwapchain swapchain, uint32_t index) override {
        VkImage image = swapchainImagesMap[swapchain].second[index].image;
        VkImageLayout layout = swapchainImagesMap[swapchain].first == SwapchainType::COLOR ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        imageStates[image] = layout;
        return (void*)image;
    }
    // XR_DOCS_TAG_END_GetSwapchainImage_Vulkan

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
    void LoadPFN_XrFunctions(XrInstance m_xrInstance);
    std::vector<std::string> GetInstanceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId);
    std::vector<std::string> GetDeviceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId);

    const std::vector<int64_t> GetSupportedColorSwapchainFormats() override;
    const std::vector<int64_t> GetSupportedDepthSwapchainFormats() override;

private:
    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    uint32_t queueFamilyIndex = 0xFFFFFFFF;
    uint32_t queueIndex = 0xFFFFFFFF;
    VkQueue queue{};
    VkFence fence{};

    VkCommandPool cmdPool{};
    VkCommandBuffer cmdBuffer{};
    VkDescriptorPool descriptorPool;

    std::vector<const char*> activeInstanceLayers{};
    std::vector<const char*> activeInstanceExtensions{};
    std::vector<const char*> activeDeviceLayer{};
    std::vector<const char*> activeDeviceExtensions{};

    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR = nullptr;
    XrGraphicsBindingVulkanKHR graphicsBinding{};

    std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageVulkanKHR>>> swapchainImagesMap{};

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
    std::unordered_map<VkCommandBuffer, std::vector<VkDescriptorSet>> cmdBufferDescriptorSets;
    std::vector<std::tuple<VkWriteDescriptorSet, VkDescriptorBufferInfo, VkDescriptorImageInfo>> writeDescSets;
};
#endif
