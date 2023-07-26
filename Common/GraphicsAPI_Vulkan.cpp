#include "GraphicsAPI_Vulkan.h"

#if defined(XR_USE_GRAPHICS_API_VULKAN)

#define VULKAN_CHECK(x, y)                                                                         \
    {                                                                                              \
        VkResult result = (x);                                                                     \
        if (result != VK_SUCCESS) {                                                                \
            std::cout << "ERROR: VULKAN: " << std::hex << "0x" << result << std::dec << std::endl; \
            std::cout << "ERROR: VULKAN: " << y << std::endl;                                      \
        }                                                                                          \
    }

// XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan
GraphicsAPI_Vulkan::GraphicsAPI_Vulkan(XrInstance m_xrInstance, XrSystemId systemId) {
    // Instance
    LoadPFN_XrFunctions(m_xrInstance);

    XrGraphicsRequirementsVulkanKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};
    OPENXR_CHECK(xrGetVulkanGraphicsRequirementsKHR(m_xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for Vulkan.");

    VkApplicationInfo ai;
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pNext = nullptr;
    ai.pApplicationName = "OpenXR Tutorial - Vulkan";
    ai.applicationVersion = 1;
    ai.pEngineName = "OpenXR Tutorial - Vulkan Engine";
    ai.engineVersion = 1;
    ai.apiVersion = graphicsRequirements.minApiVersionSupported;

    uint32_t instanceExtensionCount = 0;
    VULKAN_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");

    std::vector<VkExtensionProperties> instanceExtensionProperties;
    instanceExtensionProperties.resize(instanceExtensionCount);
    VULKAN_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");
    const std::vector<std::string> &openXrInstanceExtensionNames = GetInstanceExtensionsForOpenXR(m_xrInstance, systemId);
    for (const std::string &requestExtension : openXrInstanceExtensionNames) {
        for (const VkExtensionProperties &extensionProperty : instanceExtensionProperties) {
            if (strcmp(requestExtension.c_str(), extensionProperty.extensionName))
                continue;
            else
                activeInstanceExtensions.push_back(requestExtension.c_str());
            break;
        }
    }

    VkInstanceCreateInfo instanceCI;
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pNext = nullptr;
    instanceCI.flags = 0;
    instanceCI.pApplicationInfo = &ai;
    instanceCI.enabledLayerCount = 0;
    instanceCI.ppEnabledLayerNames = nullptr;
    instanceCI.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
    instanceCI.ppEnabledExtensionNames = activeInstanceExtensions.data();
    VULKAN_CHECK(vkCreateInstance(&instanceCI, nullptr, &instance), "Failed to create Vulkan Instance.");

    // Physical Device
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr), "Failed to enumerate PhysicalDevices.");
    physicalDevices.resize(physicalDeviceCount);
    VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()), "Failed to enumerate PhysicalDevices.");

    VkPhysicalDevice physicalDeviceFromXR;
    OPENXR_CHECK(xrGetVulkanGraphicsDeviceKHR(m_xrInstance, systemId, instance, &physicalDeviceFromXR), "Failed to get Graphics Device for Vulkan.");
    auto physicalDeviceFromXR_it = std::find(physicalDevices.begin(), physicalDevices.end(), physicalDeviceFromXR);
    if (physicalDeviceFromXR_it != physicalDevices.end()) {
        physicalDevice = *physicalDeviceFromXR_it;
    } else {
        std::cout << "ERROR: Vulkan: Failed to find PhysicalDevice for OpenXR." << std::endl;
        // Select the first available device.
        physicalDevice = physicalDevices[0];
    }

    // Device
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);
    queueFamilyProperties.resize(queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

    std::vector<VkDeviceQueueCreateInfo> deviceQueueCIs;
    std::vector<std::vector<float>> queuePriorities;
    queuePriorities.resize(queueFamilyProperties.size());
    deviceQueueCIs.resize(queueFamilyProperties.size());
    for (size_t i = 0; i < deviceQueueCIs.size(); i++) {
        for (size_t j = 0; j < queueFamilyProperties[i].queueCount; j++)
            queuePriorities[i].push_back(1.0f);

        deviceQueueCIs[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCIs[i].pNext = nullptr;
        deviceQueueCIs[i].flags = 0;
        deviceQueueCIs[i].queueFamilyIndex = static_cast<uint32_t>(i);
        deviceQueueCIs[i].queueCount = queueFamilyProperties[i].queueCount;
        deviceQueueCIs[i].pQueuePriorities = queuePriorities[i].data();

        if (BitwiseCheck(queueFamilyProperties[i].queueFlags, VkQueueFlags(VK_QUEUE_GRAPHICS_BIT)) && queueFamilyIndex == 0xFFFFFFFF && queueIndex == 0xFFFFFFFF) {
            queueFamilyIndex = static_cast<uint32_t>(i);
            queueIndex = 0;
        }
    }

    uint32_t deviceExtensionCount = 0;
    VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, 0, &deviceExtensionCount, 0), "Failed to enumerate DeviceExtensionProperties.");
    std::vector<VkExtensionProperties> deviceExtensionProperties;
    deviceExtensionProperties.resize(deviceExtensionCount);

    VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(physicalDevice, 0, &deviceExtensionCount, deviceExtensionProperties.data()), "Failed to enumerate DeviceExtensionProperties.");
    const std::vector<std::string> &openXrDeviceExtensionNames = GetDeviceExtensionsForOpenXR(m_xrInstance, systemId);
    for (const std::string &requestExtension : openXrDeviceExtensionNames) {
        for (const VkExtensionProperties &extensionProperty : deviceExtensionProperties) {
            if (strcmp(requestExtension.c_str(), extensionProperty.extensionName))
                continue;
            else
                activeDeviceExtensions.push_back(requestExtension.c_str());
            break;
        }
    }

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);

    VkDeviceCreateInfo deviceCI;
    deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCI.pNext = nullptr;
    deviceCI.flags = 0;
    deviceCI.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCIs.size());
    deviceCI.pQueueCreateInfos = deviceQueueCIs.data();
    deviceCI.enabledLayerCount = 0;
    deviceCI.ppEnabledLayerNames = nullptr;
    deviceCI.enabledExtensionCount = static_cast<uint32_t>(activeDeviceExtensions.size());
    deviceCI.ppEnabledExtensionNames = activeDeviceExtensions.data();
    deviceCI.pEnabledFeatures = &features;
    VULKAN_CHECK(vkCreateDevice(physicalDevice, &deviceCI, nullptr, &device), "Failed to create Device.");
}

GraphicsAPI_Vulkan::~GraphicsAPI_Vulkan() {
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}
// XR_DOCS_TAG_END_GraphicsAPI_Vulkan

// XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetGraphicsBinding
void *GraphicsAPI_Vulkan::GetGraphicsBinding() {
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR};
    graphicsBinding.instance = instance;
    graphicsBinding.physicalDevice = physicalDevice;
    graphicsBinding.device = device;
    graphicsBinding.queueFamilyIndex = queueFamilyIndex;
    graphicsBinding.queueIndex = queueIndex;
    return &graphicsBinding;
}
// XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetGraphicsBinding

// XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_AllocateSwapchainImageData
XrSwapchainImageBaseHeader *GraphicsAPI_Vulkan::AllocateSwapchainImageData(uint32_t count) {
    swapchainImages.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImages.data());
}
// XR_DOCS_TAG_END_GraphicsAPI_Vulkan_AllocateSwapchainImageData

void *GraphicsAPI_Vulkan::CreateImage(const ImageCreateInfo &imageCI) {
    VkImage image{};
    VkImageCreateInfo vkImageCI;
    vkImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    vkImageCI.pNext = nullptr;
    vkImageCI.flags = (imageCI.cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0) | (imageCI.dimension == 3 ? VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT : 0);
    vkImageCI.imageType = VkImageType(imageCI.dimension - 1);
    vkImageCI.format = (VkFormat)imageCI.format;
    vkImageCI.extent = {imageCI.width, imageCI.height, imageCI.depth};
    vkImageCI.mipLevels = imageCI.mipLevels;
    vkImageCI.arrayLayers = imageCI.arrayLayers;
    vkImageCI.samples = VkSampleCountFlagBits(imageCI.sampleCount);
    vkImageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
    vkImageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | (imageCI.colorAttachment ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0) | (imageCI.depthAttachment ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0);
    vkImageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkImageCI.queueFamilyIndexCount = 0;
    vkImageCI.pQueueFamilyIndices = nullptr;
    vkImageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VULKAN_CHECK(vkCreateImage(device, &vkImageCI, nullptr, &image), "Failed to create Image");

    VkMemoryRequirements memoryRequirements{};
    vkGetImageMemoryRequirements(device, image, &memoryRequirements);

    VkDeviceMemory memory{};
    VkMemoryAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;

    auto MemoryTypeFromProperties = [](VkPhysicalDeviceMemoryProperties memory_properties, uint32_t typeBits, VkMemoryPropertyFlags requirements_mask, uint32_t *typeIndex) -> bool {
        // Search memtypes to find first index with those properties
        for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
            if ((typeBits & 1) == 1) {
                // Type is available, does it match user properties?
                if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                    *typeIndex = i;
                    return true;
                }
            }
            typeBits >>= 1;
        }
        // No memory types matched, return failure
        return false;
    };

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
    MemoryTypeFromProperties(physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocateInfo.memoryTypeIndex);

    VULKAN_CHECK(vkAllocateMemory(device, &allocateInfo, nullptr, &memory), "Failed to allocate Memory.");
    VULKAN_CHECK(vkBindImageMemory(device, image, memory, 0), "Failed to bind Memory to Image.");

    imageResources[image] = memory;

    return (void *)image;
}

void GraphicsAPI_Vulkan::DestroyImage(void *&image) {
    VkImage vkImage = (VkImage)image;
    VkDeviceMemory memory = imageResources[vkImage];
    imageResources.erase(vkImage);

    vkFreeMemory(device, memory, nullptr);
    vkDestroyImage(device, vkImage, nullptr);
    image = nullptr;
}

void *GraphicsAPI_Vulkan::CreateImageView(const ImageViewCreateInfo &imageViewCI) {
    VkImageView imageView{};
    VkImageViewCreateInfo vkImageViewCI;
    vkImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vkImageViewCI.pNext = nullptr;
    vkImageViewCI.flags = 0;
    vkImageViewCI.image = (VkImage)imageViewCI.image;
    vkImageViewCI.viewType = VkImageViewType(imageViewCI.view);
    vkImageViewCI.format = (VkFormat)imageViewCI.format;
    vkImageViewCI.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    vkImageViewCI.subresourceRange.aspectMask = VkImageAspectFlagBits(imageViewCI.aspect);
    vkImageViewCI.subresourceRange.baseMipLevel = imageViewCI.baseMipLevel;
    vkImageViewCI.subresourceRange.levelCount = imageViewCI.levelCount;
    vkImageViewCI.subresourceRange.baseArrayLayer = imageViewCI.baseArrayLayer;
    vkImageViewCI.subresourceRange.layerCount = imageViewCI.layerCount;
    VULKAN_CHECK(vkCreateImageView(device, &vkImageViewCI, nullptr, &imageView), "Failed to create ImageView.");

    imageViewResources[imageView] = imageViewCI;
    return (void *)imageView;
}

void GraphicsAPI_Vulkan::DestroyImageView(void *&imageView) {
    vkDestroyImageView(device, (VkImageView)imageView, nullptr);
}

void GraphicsAPI_Vulkan::BeginRendering() {
    VkCommandPoolCreateInfo cmdPoolCI;
    cmdPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCI.pNext = nullptr;
    cmdPoolCI.flags = 0;
    cmdPoolCI.queueFamilyIndex = queueFamilyIndex;
    VULKAN_CHECK(vkCreateCommandPool(device, &cmdPoolCI, nullptr, &cmdPool), "Failed to create CommandPool.");

    VkCommandBufferAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = cmdPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;
    VULKAN_CHECK(vkAllocateCommandBuffers(device, &allocateInfo, &cmdBuffer), "Failed to allocate CommandBuffers.");

    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    VULKAN_CHECK(vkBeginCommandBuffer(cmdBuffer, &beginInfo), "Failed to begin CommandBuffers.");
}

void GraphicsAPI_Vulkan::EndRendering() {
    VULKAN_CHECK(vkEndCommandBuffer(cmdBuffer), "Failed to end CommandBuffer.");

    VkQueue queue{};
    vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);

    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkFence fence{};
    VkFenceCreateInfo fenceCI{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    VULKAN_CHECK(vkCreateFence(device, &fenceCI, nullptr, &fence), "Failed to create Fence.")
    VULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence), "Failed to submit to Queue.");

    VULKAN_CHECK(vkWaitForFences(device, 1, &fence, true, UINT64_MAX), "Failed to wait for Fence");
    vkDestroyFence(device, fence, nullptr);
}

void GraphicsAPI_Vulkan::ClearColor(void *imageView, float r, float g, float b, float a) {
    const ImageViewCreateInfo &imageViewCI = imageViewResources[(VkImageView)imageView];

    VkClearColorValue clearColor;
    clearColor.float32[0] = r;
    clearColor.float32[1] = g;
    clearColor.float32[2] = b;
    clearColor.float32[3] = a;

    VkImageSubresourceRange range;
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = imageViewCI.baseMipLevel;
    range.levelCount = imageViewCI.levelCount;
    range.baseArrayLayer = imageViewCI.baseArrayLayer;
    range.layerCount = imageViewCI.layerCount;

    VkImage vkImage = (VkImage)(imageViewCI.image);

    VkImageMemoryBarrier imageBarrier;
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.pNext = nullptr;
    imageBarrier.srcAccessMask = VkAccessFlagBits(0);
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = vkImage;
    imageBarrier.subresourceRange = range;
    vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits(0), VK_PIPELINE_STAGE_TRANSFER_BIT, VkDependencyFlagBits(0), 0, nullptr, 0, nullptr, 1, &imageBarrier);

    vkCmdClearColorImage(cmdBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);

    imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = vkImage;
    imageBarrier.subresourceRange = range;
    vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits(0), VK_PIPELINE_STAGE_TRANSFER_BIT, VkDependencyFlagBits(0), 0, nullptr, 0, nullptr, 1, &imageBarrier);
}

void GraphicsAPI_Vulkan::ClearDepth(void *imageView, float d) {
    const ImageViewCreateInfo &imageViewCI = imageViewResources[(VkImageView)imageView];

    VkClearDepthStencilValue clearDepth;
    clearDepth.depth = d;
    clearDepth.stencil = 0;

    VkImageSubresourceRange range;
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    range.baseMipLevel = imageViewCI.baseMipLevel;
    range.levelCount = imageViewCI.levelCount;
    range.baseArrayLayer = imageViewCI.baseArrayLayer;
    range.layerCount = imageViewCI.layerCount;

    VkImage vkImage = (VkImage)(imageViewCI.image);

    VkImageMemoryBarrier imageBarrier;
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.pNext = nullptr;
    imageBarrier.srcAccessMask = VkAccessFlagBits(0);
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = vkImage;
    imageBarrier.subresourceRange = range;
    vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits(0), VK_PIPELINE_STAGE_TRANSFER_BIT, VkDependencyFlagBits(0), 0, nullptr, 0, nullptr, 1, &imageBarrier);

    vkCmdClearDepthStencilImage(cmdBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepth, 1, &range);
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_LoadPFN_XrFunctions
void GraphicsAPI_Vulkan::LoadPFN_XrFunctions(XrInstance m_xrInstance) {
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction *)&xrGetVulkanGraphicsRequirementsKHR), "Failed to get InstanceProcAddr.");
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction *)&xrGetVulkanInstanceExtensionsKHR), "Failed to get InstanceProcAddr.");
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction *)&xrGetVulkanDeviceExtensionsKHR), "Failed to get InstanceProcAddr.");
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction *)&xrGetVulkanGraphicsDeviceKHR), "Failed to get InstanceProcAddr.");
}
// XR_DOCS_TAG_END_GraphicsAPI_Vulkan_LoadPFN_XrFunctions

// XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR
std::vector<std::string> GraphicsAPI_Vulkan::GetInstanceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId) {
    uint32_t extensionNamesSize = 0;
    OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Instance Extensions.");

    std::vector<char> extensionNames(extensionNamesSize);
    OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Instance Extensions.");

    std::stringstream streamData(extensionNames.data());
    std::vector<std::string> extensions;
    std::string extension;
    while (std::getline(streamData, extension, ' ')) {
        extensions.push_back(extension);
    }
    return extensions;
}
// XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR

// XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR
std::vector<std::string> GraphicsAPI_Vulkan::GetDeviceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId) {
    uint32_t extensionNamesSize = 0;
    OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(m_xrInstance, systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Device Extensions.");

    std::vector<char> extensionNames(extensionNamesSize);
    OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(m_xrInstance, systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Device Extensions.");

    std::stringstream streamData(extensionNames.data());
    std::vector<std::string> extensions;
    std::string extension;
    while (std::getline(streamData, extension, ' ')) {
        extensions.push_back(extension);
    }
    return extensions;
}
// XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetDeviceExtensionsForOpenXR

// XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetSupportedSwapchainFormats
const std::vector<int64_t> GraphicsAPI_Vulkan::GetSupportedSwapchainFormats() {
    return {
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_R8G8B8A8_UNORM};
}
// XR_DOCS_TAG_END_GraphicsAPI_Vulkan_GetSupportedSwapchainFormats
#endif