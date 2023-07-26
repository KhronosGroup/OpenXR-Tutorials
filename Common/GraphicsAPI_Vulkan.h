#pragma once
#include "GraphicsAPI.h"

#if defined(XR_USE_GRAPHICS_API_VULKAN)
class GraphicsAPI_Vulkan : public GraphicsAPI {
public:
    GraphicsAPI_Vulkan(XrInstance m_xrInstance, XrSystemId systemId);
    ~GraphicsAPI_Vulkan();

    virtual int64_t GetDepthFormat() override { return (int64_t)VK_FORMAT_D32_SFLOAT; }

    virtual void* GetGraphicsBinding() override;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) override;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) override { return (XrSwapchainImageBaseHeader*)&swapchainImages[index]; }
    virtual void* GetSwapchainImage(uint32_t index) override { return (void*)swapchainImages[index].image; }

    virtual void* CreateImage(const ImageCreateInfo& imageCI) override;
    virtual void DestroyImage(void*& image) override;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) override;
    virtual void DestroyImageView(void*& imageView) override;

    virtual void BeginRendering() override;
    virtual void EndRendering() override;

    virtual void ClearColor(void* imageView, float r, float g, float b, float a) override;
    virtual void ClearDepth(void* imageView, float d) override;

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

    std::vector<const char*> activeInstanceExtensions{};
    std::vector<const char*> activeDeviceExtensions{};

    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR = nullptr;
    XrGraphicsBindingVulkanKHR graphicsBinding{};

    std::vector<XrSwapchainImageVulkanKHR> swapchainImages{};

    std::unordered_map<VkImage, VkDeviceMemory> imageResources;
    std::unordered_map<VkImageView, ImageViewCreateInfo> imageViewResources;
};
#endif