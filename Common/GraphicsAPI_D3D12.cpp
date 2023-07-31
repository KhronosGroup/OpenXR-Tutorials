#include "GraphicsAPI_D3D12.h"

#if defined(XR_USE_GRAPHICS_API_D3D12)

#define D3D12_CHECK(x, y)                                                                         \
    {                                                                                             \
        HRESULT result = (x);                                                                     \
        if (FAILED(result)) {                                                                     \
            std::cout << "ERROR: D3D12: " << std::hex << "0x" << result << std::dec << std::endl; \
            std::cout << "ERROR: D3D12: " << y << std::endl;                                      \
        }                                                                                         \
    }

#define D3D12_SAFE_RELEASE(p) \
    {                         \
        if (p) {              \
            (p)->Release();   \
            (p) = nullptr;    \
        }                     \
    }

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12
GraphicsAPI_D3D12::GraphicsAPI_D3D12(XrInstance m_xrInstance, XrSystemId systemId) {
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetD3D12GraphicsRequirementsKHR", (PFN_xrVoidFunction *)&xrGetD3D12GraphicsRequirementsKHR), "Failed to get InstanceProcAddr.");
    XrGraphicsRequirementsD3D12KHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR};
    OPENXR_CHECK(xrGetD3D12GraphicsRequirementsKHR(m_xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for D3D12.");

    D3D12_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

    UINT i = 0;
    IDXGIAdapter1 *adapter = nullptr;
    DXGI_ADAPTER_DESC adapterDesc = {};
    while (factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
        adapter->GetDesc(&adapterDesc);
        if (memcmp(&graphicsRequirements.adapterLuid, &adapterDesc.AdapterLuid, sizeof(LUID)) == 0) {
            break;  // We have the matching adapter that OpenXR wants.
        }
    }

    D3D12_CHECK(D3D12CreateDevice(adapter, graphicsRequirements.minFeatureLevel, IID_PPV_ARGS(&device)), "Failed to create D3D12 Device.");

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Priority = 0;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.NodeMask = 0;
    D3D12_CHECK(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&queue)), "Failed to create D3D12 Command Queue.");
}

GraphicsAPI_D3D12 ::~GraphicsAPI_D3D12() {
    D3D12_SAFE_RELEASE(device);
    D3D12_SAFE_RELEASE(queue);
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D12

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_GetGraphicsBinding
void *GraphicsAPI_D3D12::GetGraphicsBinding() {
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};
    graphicsBinding.device = device;
    graphicsBinding.queue = queue;
    return &graphicsBinding;
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D12_GetGraphicsBinding

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_AllocateSwapchainImageData
XrSwapchainImageBaseHeader *GraphicsAPI_D3D12::AllocateSwapchainImageData(uint32_t count) {
    swapchainImages.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImages.data());
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D12_AllocateSwapchainImageData

void *GraphicsAPI_D3D12::CreateImage(const ImageCreateInfo &imageCI) {
    ID3D12Resource *texture = nullptr;
    D3D12_RESOURCE_DESC desc;
    desc.Dimension = D3D12_RESOURCE_DIMENSION(imageCI.dimension + 1);
    desc.Alignment = 0;
    desc.Width = imageCI.width;
    desc.Height = imageCI.height;
    desc.DepthOrArraySize = imageCI.dimension == 3 ? imageCI.depth : imageCI.arrayLayers;
    desc.MipLevels = imageCI.mipLevels;
    desc.Format = (DXGI_FORMAT)imageCI.format;
    desc.SampleDesc = {imageCI.sampleCount, 0};
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE | (imageCI.colorAttachment ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAGS(0)) | (imageCI.depthAttachment ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL : D3D12_RESOURCE_FLAGS(0));

    D3D12_CLEAR_VALUE clear = {};
    bool useClear = false;
    if (useClear = BitwiseCheck(desc.Flags, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)) {
        clear.Format = desc.Format;
        clear.Color[0] = 0.0f;
        clear.Color[1] = 0.0f;
        clear.Color[2] = 0.0f;
        clear.Color[3] = 0.0f;
    }
    if (useClear = BitwiseCheck(desc.Flags, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)) {
        clear.Format = desc.Format;
        clear.DepthStencil = {0.0f, 0};
    }

    ID3D12Heap *heap = nullptr;
    D3D12_RESOURCE_ALLOCATION_INFO allocInfo = device->GetResourceAllocationInfo(0, 1, &desc);
    D3D12_HEAP_DESC heapDesc;
    heapDesc.SizeInBytes = allocInfo.SizeInBytes;
    heapDesc.Properties = {D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0};
    heapDesc.Alignment = allocInfo.Alignment;
    heapDesc.Flags = D3D12_HEAP_FLAG_NONE;
    D3D12_CHECK(device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)), "Failed to create Heap.")

    D3D12_CHECK(device->CreatePlacedResource(heap, 0, &desc, D3D12_RESOURCE_STATE_COMMON, useClear ? &clear : nullptr, IID_PPV_ARGS(&texture)), "Failed to create Texture.");
    imageResources[texture] = heap;

    return texture;
}
void GraphicsAPI_D3D12::DestroyImage(void *&image) {
    ID3D12Resource *d3d12Image = reinterpret_cast<ID3D12Resource *>(image);
    ID3D12Heap *heap = imageResources[d3d12Image];
    imageResources.erase(d3d12Image);
    D3D12_SAFE_RELEASE(heap);
    D3D12_SAFE_RELEASE(d3d12Image);
    image = nullptr;
}

void *GraphicsAPI_D3D12::CreateImageView(const ImageViewCreateInfo &imageViewCI) {
    if (imageViewCI.type == ImageViewCreateInfo::Type::RTV) {
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = (DXGI_FORMAT)imageViewCI.format;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
            rtvDesc.Texture1D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = imageViewCI.baseMipLevel;
            rtvDesc.Texture2D.PlaneSlice = 0;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_3D: {
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
            rtvDesc.Texture3D.MipSlice = imageViewCI.baseMipLevel;
            rtvDesc.Texture3D.FirstWSlice = imageViewCI.baseArrayLayer;
            rtvDesc.Texture3D.WSize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvDesc.Texture1DArray.MipSlice = imageViewCI.baseMipLevel;
            rtvDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            rtvDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.MipSlice = imageViewCI.baseMipLevel;
            rtvDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            rtvDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            rtvDesc.Texture2DArray.PlaneSlice = 0;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D12: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = {};
        ID3D12DescriptorHeap *descHeap;
        D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
        descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descHeapDesc.NumDescriptors = 1;
        descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descHeapDesc.NodeMask = 0;
        D3D12_CHECK(device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap)), "Failed to create DescriptorHeap.");
        rtv = descHeap->GetCPUDescriptorHandleForHeapStart();
        device->CreateRenderTargetView((ID3D12Resource *)imageViewCI.image, &rtvDesc, rtv);
        imageViewResources[rtv.ptr] = descHeap;
        return (void *)rtv.ptr;
    } else if (imageViewCI.type == ImageViewCreateInfo::Type::DSV) {
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        dsvDesc.Format = (DXGI_FORMAT)imageViewCI.format;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
            dsvDesc.Texture1D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
            dsvDesc.Texture1DArray.MipSlice = imageViewCI.baseMipLevel;
            dsvDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            dsvDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvDesc.Texture2DArray.MipSlice = imageViewCI.baseMipLevel;
            dsvDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            dsvDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D12: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
        ID3D12DescriptorHeap *descHeap;
        D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
        descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        descHeapDesc.NumDescriptors = 1;
        descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descHeapDesc.NodeMask = 0;
        D3D12_CHECK(device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap)), "Failed to create DescriptorHeap.");
        dsv = descHeap->GetCPUDescriptorHandleForHeapStart();
        device->CreateDepthStencilView((ID3D12Resource *)imageViewCI.image, &dsvDesc, dsv);
        imageViewResources[dsv.ptr] = descHeap;
        return (void *)dsv.ptr;
    } else if (imageViewCI.type == ImageViewCreateInfo::Type::SRV) {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = (DXGI_FORMAT)imageViewCI.format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            srvDesc.Texture1D.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture1D.MipLevels = imageViewCI.levelCount;
            srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture2D.MipLevels = imageViewCI.levelCount;
            srvDesc.Texture2D.PlaneSlice = 0;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_3D: {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture3D.MipLevels = imageViewCI.levelCount;
            srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            srvDesc.Texture1DArray.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture1DArray.MipLevels = imageViewCI.levelCount;
            srvDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            srvDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            srvDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture2DArray.MipLevels = imageViewCI.levelCount;
            srvDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            srvDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            srvDesc.Texture2DArray.PlaneSlice = 0;
            srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D12: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        D3D12_CPU_DESCRIPTOR_HANDLE srv = {};
        ID3D12DescriptorHeap *descHeap;
        D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
        descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descHeapDesc.NumDescriptors = 1;
        descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descHeapDesc.NodeMask = 0;
        D3D12_CHECK(device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap)), "Failed to create DescriptorHeap.");
        device->CreateShaderResourceView((ID3D12Resource *)imageViewCI.image, &srvDesc, srv);
        srv = descHeap->GetCPUDescriptorHandleForHeapStart();
        imageViewResources[srv.ptr] = descHeap;
        return (void *)srv.ptr;
    } else if (imageViewCI.type == ImageViewCreateInfo::Type::UAV) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.Format = (DXGI_FORMAT)imageViewCI.format;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
            uavDesc.Texture1D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = imageViewCI.baseMipLevel;
            uavDesc.Texture2D.PlaneSlice = 0;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_3D: {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            uavDesc.Texture3D.MipSlice = imageViewCI.baseMipLevel;
            uavDesc.Texture3D.FirstWSlice = imageViewCI.baseArrayLayer;
            uavDesc.Texture3D.WSize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
            uavDesc.Texture1DArray.MipSlice = imageViewCI.baseMipLevel;
            uavDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            uavDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            uavDesc.Texture2DArray.MipSlice = imageViewCI.baseMipLevel;
            uavDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            uavDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            uavDesc.Texture2DArray.PlaneSlice = 0;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D12: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        D3D12_CPU_DESCRIPTOR_HANDLE uav = {};
        ID3D12DescriptorHeap *descHeap;
        D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
        descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descHeapDesc.NumDescriptors = 1;
        descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descHeapDesc.NodeMask = 0;
        D3D12_CHECK(device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap)), "Failed to create DescriptorHeap.");
        device->CreateUnorderedAccessView((ID3D12Resource *)imageViewCI.image, nullptr, &uavDesc, uav);
        uav = descHeap->GetCPUDescriptorHandleForHeapStart();
        imageViewResources[uav.ptr] = descHeap;
        return (void *)uav.ptr;
    } else {
        DEBUG_BREAK;
        std::cout << "ERROR: D3D12: Unknown ImageView Type." << std::endl;
        return nullptr;
    }
}

void GraphicsAPI_D3D12::DestroyImageView(void *&imageView) {
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12ImageView = {(SIZE_T)imageView};
    ID3D12DescriptorHeap *descHeap = imageViewResources[d3d12ImageView.ptr];
    imageViewResources.erase(d3d12ImageView.ptr);
    D3D12_SAFE_RELEASE(descHeap);
    imageView = nullptr;
}

void GraphicsAPI_D3D12::BeginRendering() {
    D3D12_CHECK(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)), "Failed to create CommandAllocator.");
    D3D12_CHECK(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList)), "Failed to create CommandList.");
}

void GraphicsAPI_D3D12::EndRendering() {
    D3D12_CHECK(cmdList->Close(), "Failed to close CommandList");
    queue->ExecuteCommandLists(1, (ID3D12CommandList **)&cmdList);
    D3D12_SAFE_RELEASE(cmdList);
    D3D12_SAFE_RELEASE(cmdAllocator);
}

void GraphicsAPI_D3D12::ClearColor(void *imageView, float r, float g, float b, float a) {
    const FLOAT clearColor[4] = {r, g, b, a};
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12ImageView = {(SIZE_T)imageView};
    cmdList->ClearRenderTargetView(d3d12ImageView, clearColor, 0, nullptr);
}

void GraphicsAPI_D3D12::ClearDepth(void *imageView, float d) {
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12ImageView = {(SIZE_T)imageView};
    cmdList->ClearDepthStencilView(d3d12ImageView, D3D12_CLEAR_FLAG_DEPTH, d, 0, 0, nullptr);
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_GetSupportedSwapchainFormats
const std::vector<int64_t> GraphicsAPI_D3D12::GetSupportedSwapchainFormats() {
    return {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB};
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D12_GetSupportedSwapchainFormats
#endif