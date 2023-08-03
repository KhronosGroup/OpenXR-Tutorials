#define VK_USE_PLATFORM_WIN32_KHR
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

#if defined(__ANDROID__) && !defined(VK_API_MAKE_VERSION)
#define VK_MAKE_API_VERSION(variant, major, minor, patch) VK_MAKE_VERSION(major, minor, patch)
#endif

bool MemoryTypeFromProperties(VkPhysicalDeviceMemoryProperties memoryProperties, uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t *typeIndex) {
    // Search memory types to find first index with those properties
    for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
};

GraphicsAPI_Vulkan::GraphicsAPI_Vulkan() {
    // Instance
    VkApplicationInfo ai;
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pNext = nullptr;
    ai.pApplicationName = "OpenXR Tutorial - Vulkan";
    ai.applicationVersion = 1;
    ai.pEngineName = "OpenXR Tutorial - Vulkan Engine";
    ai.engineVersion = 1;
    ai.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    uint32_t instanceExtensionCount = 0;
    VULKAN_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");

    std::vector<VkExtensionProperties> instanceExtensionProperties;
    instanceExtensionProperties.resize(instanceExtensionCount);
    VULKAN_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");
    const std::vector<std::string> &instanceExtensionNames = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
    for (const std::string &requestExtension : instanceExtensionNames) {
        for (const VkExtensionProperties &extensionProperty : instanceExtensionProperties) {
            if (strcmp(requestExtension.c_str(), extensionProperty.extensionName))
                continue;
            else
                activeInstanceExtensions.push_back(requestExtension.c_str());
            break;
        }
    }

    activeInstanceLayers = {"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo instanceCI;
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pNext = nullptr;
    instanceCI.flags = 0;
    instanceCI.pApplicationInfo = &ai;
    instanceCI.enabledLayerCount = static_cast<uint32_t>(activeInstanceLayers.size());
    instanceCI.ppEnabledLayerNames = activeInstanceLayers.data();
    instanceCI.enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size());
    instanceCI.ppEnabledExtensionNames = activeInstanceExtensions.data();
    VULKAN_CHECK(vkCreateInstance(&instanceCI, nullptr, &instance), "Failed to create Vulkan Instance.");

    // Physical Device
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr), "Failed to enumerate PhysicalDevices.");
    physicalDevices.resize(physicalDeviceCount);
    VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()), "Failed to enumerate PhysicalDevices.");
    // Select the first available device.
    physicalDevice = physicalDevices[0];

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
    const std::vector<std::string> &deviceExtensionNames = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    for (const std::string &requestExtension : deviceExtensionNames) {
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
    ai.apiVersion = VK_MAKE_API_VERSION(0, XR_VERSION_MAJOR(graphicsRequirements.minApiVersionSupported), XR_VERSION_MINOR(graphicsRequirements.minApiVersionSupported), 0);

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

    activeInstanceLayers = {"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo instanceCI;
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pNext = nullptr;
    instanceCI.flags = 0;
    instanceCI.pApplicationInfo = &ai;
    instanceCI.enabledLayerCount = static_cast<uint32_t>(activeInstanceLayers.size());
    instanceCI.ppEnabledLayerNames = activeInstanceLayers.data();
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

void *GraphicsAPI_Vulkan::CreateDesktopSwapchain(const SwapchainCreateInfo &swapchainCI) {
    VkSurfaceKHR surface{};
    VkWin32SurfaceCreateInfoKHR surfaceCI;
    surfaceCI.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCI.pNext = nullptr;
    surfaceCI.flags = 0;
    surfaceCI.hinstance = GetModuleHandle(nullptr);
    surfaceCI.hwnd = (HWND)swapchainCI.windowHandle;
    VULKAN_CHECK(vkCreateWin32SurfaceKHR(instance, &surfaceCI, nullptr, &surface), "Failed to create Device.");

    VkBool32 surfaceSupport;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &surfaceSupport), "Failed to get PhysicalDeviceSurfaceSupport.");
    if (!surfaceSupport) {
        std::cout << "ERROR: VULKAN: Can not use the created surface." << std::endl;
    }

    VkSurfaceCapabilitiesKHR surfaceCapability;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapability), "Failed to get PhysicalDeviceSurfaceCapabilities.");

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;
    uint32_t surfaceFormatsCount = 0;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount, nullptr), "Failed to get PhysicalDeviceSurfaceFormats.");
    surfaceFormats.resize(surfaceFormatsCount);
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount, surfaceFormats.data()), "Failed to get PhysicalDeviceSurfaceFormats.");

    uint32_t presentModesCount = 0;
    VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr), "Failed to get PhysicalDeviceSurfacePresentModes.");
    presentModes.resize(presentModesCount);
    VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, presentModes.data()), "Failed to get PhysicalDeviceSurfacePresentModes.");

    if (surfaceFormats.empty() || presentModes.empty()) {
        std::cout << "ERROR: VULKAN: Could not find suitable surface formats and/or present modes for the created surface" << std::endl;
    }

    VkSurfaceFormatKHR surfaceFormat = {(VkFormat)swapchainCI.format, VK_COLORSPACE_SRGB_NONLINEAR_KHR};
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkSurfaceFormatKHR chosenSurfaceFormat = surfaceFormats[0];  // Default
    for (auto &_surfaceFormat : surfaceFormats) {
        if (_surfaceFormat.format == surfaceFormat.format && _surfaceFormat.colorSpace == surfaceFormat.colorSpace) {
            chosenSurfaceFormat = _surfaceFormat;
            break;
        } else
            continue;
    }
    VkPresentModeKHR chosenPresentMode = presentModes[0];  // Default

    VkSwapchainKHR swapchain{};
    VkSwapchainCreateInfoKHR vkSwapchainCI;
    vkSwapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkSwapchainCI.pNext = nullptr;
    vkSwapchainCI.flags = 0;
    vkSwapchainCI.surface = surface;
    vkSwapchainCI.minImageCount = swapchainCI.count;
    vkSwapchainCI.imageFormat = chosenSurfaceFormat.format;
    vkSwapchainCI.imageColorSpace = chosenSurfaceFormat.colorSpace;
    vkSwapchainCI.imageExtent = surfaceCapability.currentExtent;
    vkSwapchainCI.imageArrayLayers = 1;
    vkSwapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    vkSwapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkSwapchainCI.queueFamilyIndexCount = 0;
    vkSwapchainCI.pQueueFamilyIndices = nullptr;
    vkSwapchainCI.preTransform = surfaceCapability.currentTransform;
    vkSwapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    vkSwapchainCI.presentMode = swapchainCI.vsync ? chosenPresentMode : VK_PRESENT_MODE_IMMEDIATE_KHR;  // Use VK_PRESENT_MODE_IMMEDIATE_KHR for no v-sync.
    vkSwapchainCI.clipped = VK_TRUE;
    vkSwapchainCI.oldSwapchain = VK_NULL_HANDLE;

    VULKAN_CHECK(vkCreateSwapchainKHR(device, &vkSwapchainCI, nullptr, &swapchain), "ERROR: VULKAN: Failed to create Swapchain");

    surfaces[swapchain] = surface;

    VkSemaphoreCreateInfo semaphoreCI;
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.pNext = nullptr;
    semaphoreCI.flags = 0;
    VULKAN_CHECK(vkCreateSemaphore(device, &semaphoreCI, nullptr, &acquireSemaphore), "Failed to create Acquire Semaphore");
    VULKAN_CHECK(vkCreateSemaphore(device, &semaphoreCI, nullptr, &submitSemaphore), "Failed to create Submit Semaphore");

    return (void *)swapchain;
}

void GraphicsAPI_Vulkan::DestroyDesktopSwapchain(void *&swapchain) {
    VkSwapchainKHR vkSwapchain = (VkSwapchainKHR)swapchain;
    VkSurfaceKHR surface = surfaces[vkSwapchain];
    vkDestroySwapchainKHR(device, vkSwapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    surfaces.erase(vkSwapchain);
    swapchain = nullptr;
}

void *GraphicsAPI_Vulkan::GetDesktopSwapchainImage(void *swapchain, uint32_t index) {
    VkSwapchainKHR vkSwapchain = (VkSwapchainKHR)swapchain;
    std::vector<VkImage> swapchainImages;
    uint32_t swapchainImagesCount = 0;
    VULKAN_CHECK(vkGetSwapchainImagesKHR(device, vkSwapchain, &swapchainImagesCount, 0), "Failed to get Swapchain Images");
    swapchainImages.resize(swapchainImagesCount);
    VULKAN_CHECK(vkGetSwapchainImagesKHR(device, vkSwapchain, &swapchainImagesCount, swapchainImages.data()), "Failed to get Swapchain Images");

    return (void *)swapchainImages[index];
}
void GraphicsAPI_Vulkan::AcquireDesktopSwapchanImage(void *swapchain, uint32_t &index) {
    VULKAN_CHECK(vkAcquireNextImageKHR(device, (VkSwapchainKHR)swapchain, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, &index), "Failed to acquire next Image from Swapchain.");

    currentDesktopSwapchainImage = (VkImage)GetDesktopSwapchainImage(swapchain, index);
    imageStates[currentDesktopSwapchainImage] = VK_IMAGE_LAYOUT_UNDEFINED;
}

void GraphicsAPI_Vulkan::PresentDesktopSwapchainImage(void *swapchain, uint32_t index) {
    imageStates.erase(currentDesktopSwapchainImage);
    currentDesktopSwapchainImage = VK_NULL_HANDLE;

    VkQueue queue{};
    vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);

    VkSwapchainKHR vkSwapchain = (VkSwapchainKHR)swapchain;
    VkPresentInfoKHR pi;
    pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    pi.pNext = nullptr;
    pi.waitSemaphoreCount = 1;
    pi.pWaitSemaphores = &submitSemaphore;
    pi.swapchainCount = 1;
    pi.pSwapchains = &vkSwapchain;
    pi.pImageIndices = &index;
    pi.pResults = nullptr;

    VULKAN_CHECK(vkQueuePresentKHR(queue, &pi), "Failed to present the Image from Swapchain.");
}

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
    vkImageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | (imageCI.colorAttachment ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0) | (imageCI.depthAttachment ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0);
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

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
    MemoryTypeFromProperties(physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocateInfo.memoryTypeIndex);

    VULKAN_CHECK(vkAllocateMemory(device, &allocateInfo, nullptr, &memory), "Failed to allocate Memory.");
    VULKAN_CHECK(vkBindImageMemory(device, image, memory, 0), "Failed to bind Memory to Image.");

    imageResources[image] = memory;
    imageStates[image] = vkImageCI.initialLayout;

    return (void *)image;
}

void GraphicsAPI_Vulkan::DestroyImage(void *&image) {
    VkImage vkImage = (VkImage)image;
    VkDeviceMemory memory = imageResources[vkImage];
    vkFreeMemory(device, memory, nullptr);
    vkDestroyImage(device, vkImage, nullptr);
    imageResources.erase(vkImage);
    imageStates.erase(vkImage);
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
    VkImageView vkImageView = (VkImageView)imageView;
    vkDestroyImageView(device, vkImageView, nullptr);
    imageViewResources.erase(vkImageView);
    imageView = nullptr;
}

void *GraphicsAPI_Vulkan::CreateSampler(const SamplerCreateInfo &samplerCI) {
    VkSampler sampler{};
    VkSamplerCreateInfo vkSamplerCI;
    vkSamplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    vkSamplerCI.pNext = nullptr;
    vkSamplerCI.flags = 0;
    vkSamplerCI.magFilter = static_cast<VkFilter>(samplerCI.magFilter);
    vkSamplerCI.minFilter = static_cast<VkFilter>(samplerCI.minFilter);
    vkSamplerCI.mipmapMode = static_cast<VkSamplerMipmapMode>(samplerCI.mipmapMode);
    vkSamplerCI.addressModeU = static_cast<VkSamplerAddressMode>(samplerCI.addressModeS);
    vkSamplerCI.addressModeV = static_cast<VkSamplerAddressMode>(samplerCI.addressModeT);
    vkSamplerCI.addressModeW = static_cast<VkSamplerAddressMode>(samplerCI.addressModeR);
    vkSamplerCI.mipLodBias = samplerCI.mipLodBias;
    vkSamplerCI.anisotropyEnable = false;
    vkSamplerCI.maxAnisotropy = 0.0f;
    vkSamplerCI.compareEnable = samplerCI.compareEnable;
    vkSamplerCI.compareOp = static_cast<VkCompareOp>(samplerCI.compareOp);
    vkSamplerCI.minLod = samplerCI.minLod;
    vkSamplerCI.maxLod = samplerCI.maxLod;
    bool opaque = samplerCI.borderColor[3] == 1.0f;
    bool white = samplerCI.borderColor[0] == 1.0f && samplerCI.borderColor[1] == 1.0f && samplerCI.borderColor[2] == 1.0f && opaque;
    vkSamplerCI.borderColor = static_cast<VkBorderColor>((white ? 4 : opaque ? 2
                                                                             : 0));
    vkSamplerCI.unnormalizedCoordinates = false;

    VULKAN_CHECK(vkCreateSampler(device, &vkSamplerCI, nullptr, &sampler), "Failed to create Sampler.");
    return (void *)sampler;
}

void GraphicsAPI_Vulkan::DestroySampler(void *&sampler) {
    vkDestroySampler(device, (VkSampler)sampler, nullptr);
    sampler = nullptr;
}

void *GraphicsAPI_Vulkan::CreateBuffer(const BufferCreateInfo &bufferCI) {
    VkBuffer buffer{};
    VkBufferCreateInfo vkBufferCI;
    vkBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkBufferCI.pNext = nullptr;
    vkBufferCI.flags = 0;
    vkBufferCI.size = static_cast<VkDeviceSize>(bufferCI.size);
    vkBufferCI.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | (bufferCI.type == BufferCreateInfo::Type::VERTEX ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : 0) 
        | (bufferCI.type == BufferCreateInfo::Type::INDEX ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : 0) | (bufferCI.type == BufferCreateInfo::Type::UNIFORM ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : 0);
    vkBufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkBufferCI.queueFamilyIndexCount = 0;
    vkBufferCI.pQueueFamilyIndices = nullptr;
    vkCreateBuffer(device, &vkBufferCI, nullptr, &buffer);

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    VkDeviceMemory memory{};
    VkMemoryAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
    MemoryTypeFromProperties(physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocateInfo.memoryTypeIndex);

    VULKAN_CHECK(vkAllocateMemory(device, &allocateInfo, nullptr, &memory), "Failed to allocate Memory.");
    VULKAN_CHECK(vkBindBufferMemory(device, buffer, memory, 0), "Failed to bind Memory to Buffer.");

    SetBufferData(buffer, 0, bufferCI.size, bufferCI.data);
    bufferResources[buffer] = {memory, bufferCI};

    return buffer;
}
void GraphicsAPI_Vulkan::DestroyBuffer(void *&buffer) {
    VkBuffer vkBuffer = (VkBuffer) buffer;
    VkDeviceMemory memory = bufferResources[vkBuffer].first;
    vkFreeMemory(device, memory, nullptr);
    vkDestroyBuffer(device, vkBuffer, nullptr);
    bufferResources.erase(vkBuffer);
    buffer = nullptr;
}
void *GraphicsAPI_Vulkan::CreateShader(const ShaderCreateInfo &shaderCI) { return nullptr; }
void GraphicsAPI_Vulkan::DestroyShader(void *&shader) {}
void *GraphicsAPI_Vulkan::CreatePipeline(const PipelineCreateInfo &pipelineCI) { return nullptr; }
void GraphicsAPI_Vulkan::DestroyPipeline(void *&pipeline) {}

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

    if (currentDesktopSwapchainImage) {
        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        barrier.srcAccessMask = VkAccessFlagBits(0);
        barrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.oldLayout = imageStates[currentDesktopSwapchainImage];
        barrier.newLayout = imageStates[currentDesktopSwapchainImage] = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = currentDesktopSwapchainImage;
        barrier.subresourceRange = {VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkDependencyFlags(0),
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
    }
}

void GraphicsAPI_Vulkan::EndRendering() {
    if (currentDesktopSwapchainImage) {
        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        barrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VkAccessFlagBits(0);
        barrier.oldLayout = imageStates[currentDesktopSwapchainImage];
        barrier.newLayout = imageStates[currentDesktopSwapchainImage] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = currentDesktopSwapchainImage;
        barrier.subresourceRange = {VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        vkCmdPipelineBarrier(cmdBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkDependencyFlags(0),
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
    }

    VULKAN_CHECK(vkEndCommandBuffer(cmdBuffer), "Failed to end CommandBuffer.");

    VkQueue queue{};
    vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);

    VkPipelineStageFlags waitDstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = acquireSemaphore ? 1 : 0;
    submitInfo.pWaitSemaphores = acquireSemaphore ? &acquireSemaphore : nullptr;
    submitInfo.pWaitDstStageMask = acquireSemaphore ? &waitDstStageMask : nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.signalSemaphoreCount = submitSemaphore ? 1 : 0;
    submitInfo.pSignalSemaphores = submitSemaphore ? &submitSemaphore : nullptr;

    VkFence fence{};
    VkFenceCreateInfo fenceCI{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.pNext = nullptr;
    fenceCI.flags = 0;
    VULKAN_CHECK(vkCreateFence(device, &fenceCI, nullptr, &fence), "Failed to create Fence.")

    VULKAN_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence), "Failed to submit to Queue.");

    VULKAN_CHECK(vkWaitForFences(device, 1, &fence, true, UINT64_MAX), "Failed to wait for Fence");
    vkDestroyFence(device, fence, nullptr);

    vkFreeCommandBuffers(device, cmdPool, 1, &cmdBuffer);
    vkDestroyCommandPool(device, cmdPool, nullptr);
}

void GraphicsAPI_Vulkan::SetBufferData(void *buffer, size_t offset, size_t size, void *data) {
    VkBuffer vkBuffer = (VkBuffer)buffer;
    VkDeviceMemory memory = bufferResources[vkBuffer].first;
    void *mappedData;
    VULKAN_CHECK(vkMapMemory(device, memory, offset, size, 0, &mappedData), "Can not map Buffer.");
    memcpy(mappedData, data, size);
    //Because the VkDeviceMemory use a heap with properties (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    //We don't need to use vkFlushMappedMemoryRanges() or vkInvalidateMappedMemoryRanges()
    vkUnmapMemory(device, memory);
};

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
    imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = vkImage;
    imageBarrier.subresourceRange = range;
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VkDependencyFlagBits(0), 0, nullptr, 0, nullptr, 1, &imageBarrier);

    vkCmdClearColorImage(cmdBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);

    imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = vkImage;
    imageBarrier.subresourceRange = range;
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkDependencyFlagBits(0), 0, nullptr, 0, nullptr, 1, &imageBarrier);
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
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VkDependencyFlagBits(0), 0, nullptr, 0, nullptr, 1, &imageBarrier);

    vkCmdClearDepthStencilImage(cmdBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepth, 1, &range);
}

void GraphicsAPI_Vulkan::SetRenderAttachments(void **colorViews, size_t colorViewCount, void *depthStencilView) {
}

void GraphicsAPI_Vulkan::SetViewports(Viewport *viewports, size_t count) {
    std::vector<VkViewport> vkViewports;
    vkViewports.reserve(count);
    for (size_t i = 0; i < count; i++) {
        const Viewport &viewport = viewports[i];
        vkViewports.push_back({viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth});
    }

    vkCmdSetViewport(cmdBuffer, 0, static_cast<uint32_t>(vkViewports.size()), vkViewports.data());
}
void GraphicsAPI_Vulkan::SetScissors(Rect2D *scissors, size_t count) {
    std::vector<VkRect2D> vkRect2D;
    vkRect2D.reserve(count);
    for (size_t i = 0; i < count; i++) {
        const Rect2D &scissor = scissors[i];
        vkRect2D.push_back({{scissor.offset.x, scissor.offset.y}, {scissor.extent.width, scissor.extent.height}});
    }

    vkCmdSetScissor(cmdBuffer, 0, static_cast<uint32_t>(vkRect2D.size()), vkRect2D.data());
}
void GraphicsAPI_Vulkan::SetPipeline(void *pipeline) {
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)pipeline);
}

void GraphicsAPI_Vulkan::SetDescriptor(const DescriptorInfo &descriptorInfo) {
}

void GraphicsAPI_Vulkan::UpdateDescriptors() {
}

void GraphicsAPI_Vulkan::SetVertexBuffers(void **vertexBuffers, size_t count) {
    std::vector<VkBuffer> vkBuffers;
    std::vector<VkDeviceSize> offsets;
    for (size_t i = 0; i < count; i++) {
        vkBuffers.push_back((VkBuffer)vertexBuffers[i]);
        offsets.push_back(0);
    }

    vkCmdBindVertexBuffers(cmdBuffer, 0, static_cast<uint32_t>(vkBuffers.size()), vkBuffers.data(), offsets.data());
}

void GraphicsAPI_Vulkan::SetIndexBuffer(void *indexBuffer) {
    const BufferCreateInfo &bufferCI = bufferResources[(VkBuffer)indexBuffer].second;
    VkIndexType type = bufferCI.stride == 4 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
    vkCmdBindIndexBuffer(cmdBuffer, (VkBuffer)indexBuffer, 0, type);
}

void GraphicsAPI_Vulkan::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void GraphicsAPI_Vulkan::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    vkCmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
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