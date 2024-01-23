// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#include <GraphicsAPI_D3D12.h>

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

static D3D12_FILTER ToD3D12Filter(GraphicsAPI::SamplerCreateInfo::Filter magFilter, GraphicsAPI::SamplerCreateInfo::Filter minFilter, GraphicsAPI::SamplerCreateInfo::MipmapMode mipmapMode, bool anisotropic) {
    if (anisotropic)
        return D3D12_FILTER_ANISOTROPIC;

    uint32_t res = 0;
    if (mipmapMode == GraphicsAPI::SamplerCreateInfo::MipmapMode::LINEAR)
        res += 1;
    if (magFilter == GraphicsAPI::SamplerCreateInfo::Filter::LINEAR)
        res += 4;
    if (minFilter == GraphicsAPI::SamplerCreateInfo::Filter::LINEAR)
        res += 16;

    return static_cast<D3D12_FILTER>(res);
}

static DXGI_FORMAT ToDXGI_FORMAT(GraphicsAPI::VertexType type) {
    switch (type) {
    case GraphicsAPI::VertexType::FLOAT:
        return DXGI_FORMAT_R32_FLOAT;
    case GraphicsAPI::VertexType::VEC2:
        return DXGI_FORMAT_R32G32_FLOAT;
    case GraphicsAPI::VertexType::VEC3:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case GraphicsAPI::VertexType::VEC4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case GraphicsAPI::VertexType::INT:
        return DXGI_FORMAT_R32_SINT;
    case GraphicsAPI::VertexType::IVEC2:
        return DXGI_FORMAT_R32G32_SINT;
    case GraphicsAPI::VertexType::IVEC3:
        return DXGI_FORMAT_R32G32B32_SINT;
    case GraphicsAPI::VertexType::IVEC4:
        return DXGI_FORMAT_R32G32B32A32_SINT;
    case GraphicsAPI::VertexType::UINT:
        return DXGI_FORMAT_R32_UINT;
    case GraphicsAPI::VertexType::UVEC2:
        return DXGI_FORMAT_R32G32_UINT;
    case GraphicsAPI::VertexType::UVEC3:
        return DXGI_FORMAT_R32G32B32_UINT;
    case GraphicsAPI::VertexType::UVEC4:
        return DXGI_FORMAT_R32G32B32A32_UINT;
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

static D3D12_BLEND ToD3D12_BLEND(GraphicsAPI::BlendFactor blend) {
    switch (blend) {
    default:
    case GraphicsAPI::BlendFactor::ZERO:
        return D3D12_BLEND_ZERO;
    case GraphicsAPI::BlendFactor::ONE:
        return D3D12_BLEND_ONE;
    case GraphicsAPI::BlendFactor::SRC_COLOR:
        return D3D12_BLEND_SRC_COLOR;
    case GraphicsAPI::BlendFactor::ONE_MINUS_SRC_COLOR:
        return D3D12_BLEND_INV_SRC_COLOR;
    case GraphicsAPI::BlendFactor::DST_COLOR:
        return D3D12_BLEND_DEST_COLOR;
    case GraphicsAPI::BlendFactor::ONE_MINUS_DST_COLOR:
        return D3D12_BLEND_INV_DEST_COLOR;
    case GraphicsAPI::BlendFactor::SRC_ALPHA:
        return D3D12_BLEND_SRC_ALPHA;
    case GraphicsAPI::BlendFactor::ONE_MINUS_SRC_ALPHA:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case GraphicsAPI::BlendFactor::DST_ALPHA:
        return D3D12_BLEND_DEST_ALPHA;
    case GraphicsAPI::BlendFactor::ONE_MINUS_DST_ALPHA:
        return D3D12_BLEND_INV_DEST_ALPHA;
    }
}

static D3D12_LOGIC_OP ToD3D12_LOGIC_OP(GraphicsAPI::LogicOp logic) {
    switch (logic) {
    default:
    case GraphicsAPI::LogicOp::CLEAR:
        return D3D12_LOGIC_OP_CLEAR;
    case GraphicsAPI::LogicOp::AND:
        return D3D12_LOGIC_OP_AND;
    case GraphicsAPI::LogicOp::AND_REVERSE:
        return D3D12_LOGIC_OP_AND_REVERSE;
    case GraphicsAPI::LogicOp::COPY:
        return D3D12_LOGIC_OP_COPY;
    case GraphicsAPI::LogicOp::AND_INVERTED:
        return D3D12_LOGIC_OP_AND_INVERTED;
    case GraphicsAPI::LogicOp::NO_OP:
        return D3D12_LOGIC_OP_NOOP;
    case GraphicsAPI::LogicOp::XOR:
        return D3D12_LOGIC_OP_XOR;
    case GraphicsAPI::LogicOp::OR:
        return D3D12_LOGIC_OP_OR;
    case GraphicsAPI::LogicOp::NOR:
        return D3D12_LOGIC_OP_NOR;
    case GraphicsAPI::LogicOp::EQUIVALENT:
        return D3D12_LOGIC_OP_EQUIV;
    case GraphicsAPI::LogicOp::INVERT:
        return D3D12_LOGIC_OP_INVERT;
    case GraphicsAPI::LogicOp::OR_REVERSE:
        return D3D12_LOGIC_OP_OR_REVERSE;
    case GraphicsAPI::LogicOp::COPY_INVERTED:
        return D3D12_LOGIC_OP_COPY_INVERTED;
    case GraphicsAPI::LogicOp::OR_INVERTED:
        return D3D12_LOGIC_OP_OR_INVERTED;
    case GraphicsAPI::LogicOp::NAND:
        return D3D12_LOGIC_OP_NAND;
    case GraphicsAPI::LogicOp::SET:
        return D3D12_LOGIC_OP_SET;
    }
}

static D3D12_PRIMITIVE_TOPOLOGY ToD3D12_PRIMITIVE_TOPOLOGY(GraphicsAPI::PrimitiveTopology topology) {
    switch (topology) {
    case GraphicsAPI::PrimitiveTopology::POINT_LIST:
        return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case GraphicsAPI::PrimitiveTopology::LINE_LIST:
        return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case GraphicsAPI::PrimitiveTopology::LINE_STRIP:
        return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case GraphicsAPI::PrimitiveTopology::TRIANGLE_STRIP:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case GraphicsAPI::PrimitiveTopology::TRIANGLE_FAN:
        return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    default:
        return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

GraphicsAPI_D3D12::GraphicsAPI_D3D12() {
    /*D3D12_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)), "Failed to get DebugInterface.");
    debug->EnableDebugLayer();
    reinterpret_cast<ID3D12Debug1 *>(debug)->SetEnableGPUBasedValidation(true);*/

    D3D12_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

    IDXGIAdapter *adapter = nullptr;
    DXGI_ADAPTER_DESC adapterDesc = {};
    for(UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        adapter->GetDesc(&adapterDesc);
        break;
    }

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    D3D12_CHECK(D3D12CreateDevice(adapter, featureLevel, IID_PPV_ARGS(&device)), "Failed to create D3D12 Device.");

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Priority = 0;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.NodeMask = 0;
    D3D12_CHECK(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&queue)), "Failed to create D3D12 Command Queue.");

    D3D12_FEATURE_DATA_D3D12_OPTIONS d3d12Options{};
    D3D12_CHECK(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &d3d12Options, sizeof(d3d12Options)), "Unable to CheckFeatureSupport for D3D12_FEATURE_D3D12_OPTIONS.");

    struct ResourceBindingCapabilities {
        uint32_t maxDescriptorCount;
        uint32_t maxCBVsPerStage;
        uint32_t maxSRVsPerStage;
        uint32_t maxUAVsPerStage;
        uint32_t maxSamplersPerStage;
        uint32_t maxSamplerCount = 2048;
    } resourceBindingCapabilities;

    switch (d3d12Options.ResourceBindingTier) {
    case D3D12_RESOURCE_BINDING_TIER_3: {
        resourceBindingCapabilities = {1000000, 1000000, 1000000, 1000000, 1000000, 2048};
        break;
    }
    case D3D12_RESOURCE_BINDING_TIER_2: {
        resourceBindingCapabilities = {1000000, 14, 1000000, 64, 1000000, 2048};
        break;
    }
    case D3D12_RESOURCE_BINDING_TIER_1:
    default: {
        uint32_t maxUAVsPerStage = (featureLevel == D3D_FEATURE_LEVEL_11_0) ? 8 : 64;
        resourceBindingCapabilities = {1000000, 14, 128, maxUAVsPerStage, 16, 2048};
        break;
    }
    }

    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.NumDescriptors = resourceBindingCapabilities.maxDescriptorCount;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DescriptorHeapDesc.NodeMask = 0;
    device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&CBV_SRV_UAV_DescriptorHeap));
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    DescriptorHeapDesc.NumDescriptors = resourceBindingCapabilities.maxSamplerCount;
    device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&SAMPLER_DescriptorHeap));
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12
GraphicsAPI_D3D12::GraphicsAPI_D3D12(XrInstance m_xrInstance, XrSystemId systemId) {
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetD3D12GraphicsRequirementsKHR", (PFN_xrVoidFunction *)&xrGetD3D12GraphicsRequirementsKHR), "Failed to get InstanceProcAddr for xrGetD3D12GraphicsRequirementsKHR.");
    XrGraphicsRequirementsD3D12KHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR};
    OPENXR_CHECK(xrGetD3D12GraphicsRequirementsKHR(m_xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for D3D12.");
    if (debugAPI) {
        D3D12_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)), "Failed to get DebugInterface.");
        debug->EnableDebugLayer();
        reinterpret_cast<ID3D12Debug1 *>(debug)->SetEnableGPUBasedValidation(true);
    }
    D3D12_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

    IDXGIAdapter *adapter = nullptr;
    DXGI_ADAPTER_DESC adapterDesc = {};
    for(UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        adapter->GetDesc(&adapterDesc);
        if (memcmp(&graphicsRequirements.adapterLuid, &adapterDesc.AdapterLuid, sizeof(LUID)) == 0) {
            break;  // We have the matching adapter that OpenXR wants.
        }
        // If we don't get a match reset adapter to nullptr to force a throw.
        adapter = nullptr;
    }
    OPENXR_CHECK(adapter != nullptr ? XR_SUCCESS : XR_ERROR_VALIDATION_FAILURE, "Failed to find matching graphics adapter from xrGetD3D12GraphicsRequirementsKHR.");

    D3D12_CHECK(D3D12CreateDevice(adapter, graphicsRequirements.minFeatureLevel, IID_PPV_ARGS(&device)), "Failed to create D3D12 Device.");

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Priority = 0;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.NodeMask = 0;
    D3D12_CHECK(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&queue)), "Failed to create D3D12 Command Queue.");

    D3D12_FEATURE_DATA_D3D12_OPTIONS d3d12Options{};
    D3D12_CHECK(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &d3d12Options, sizeof(d3d12Options)), "Unable to CheckFeatureSupport for D3D12_FEATURE_D3D12_OPTIONS.");

    struct ResourceBindingCapabilities {
        uint32_t maxDescriptorCount;
        uint32_t maxCBVsPerStage;
        uint32_t maxSRVsPerStage;
        uint32_t maxUAVsPerStage;
        uint32_t maxSamplersPerStage;
        uint32_t maxSamplerCount = 2048;
    } resourceBindingCapabilities;

    switch (d3d12Options.ResourceBindingTier) {
    case D3D12_RESOURCE_BINDING_TIER_3: {
        resourceBindingCapabilities = {1000000, 1000000, 1000000, 1000000, 1000000, 2048};
        break;
    }
    case D3D12_RESOURCE_BINDING_TIER_2: {
        resourceBindingCapabilities = {1000000, 14, 1000000, 64, 1000000, 2048};
        break;
    }
    case D3D12_RESOURCE_BINDING_TIER_1:
    default: {
        uint32_t maxUAVsPerStage = (graphicsRequirements.minFeatureLevel == D3D_FEATURE_LEVEL_11_0) ? 8 : 64;
        resourceBindingCapabilities = {1000000, 14, 128, maxUAVsPerStage, 16, 2048};
        break;
    }
    }

    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc;
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    DescriptorHeapDesc.NumDescriptors = resourceBindingCapabilities.maxDescriptorCount;
    DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DescriptorHeapDesc.NodeMask = 0;
    device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&CBV_SRV_UAV_DescriptorHeap));
    DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    DescriptorHeapDesc.NumDescriptors = resourceBindingCapabilities.maxSamplerCount;
    device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&SAMPLER_DescriptorHeap));
}

GraphicsAPI_D3D12 ::~GraphicsAPI_D3D12() {
    D3D12_SAFE_RELEASE(SAMPLER_DescriptorHeap);
    D3D12_SAFE_RELEASE(CBV_SRV_UAV_DescriptorHeap);
    D3D12_SAFE_RELEASE(queue);
    D3D12_SAFE_RELEASE(device);
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D12

static bool DesktopSwapchainVsync = false;
void *GraphicsAPI_D3D12::CreateDesktopSwapchain(const SwapchainCreateInfo &swapchainCI) {
    DesktopSwapchainVsync = swapchainCI.vsync;
    IDXGISwapChain1 *swapchain = nullptr;
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc;
    swapchainDesc.Width = swapchainCI.width;
    swapchainDesc.Height = swapchainCI.height;
    swapchainDesc.Format = (DXGI_FORMAT)swapchainCI.format;
    swapchainDesc.Stereo = false;
    swapchainDesc.SampleDesc = {1, 0};
    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.BufferCount = swapchainCI.count;  // One of these is locked by Windows
    swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapchainDesc.Flags = swapchainCI.vsync == false ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    D3D12_CHECK(factory->CreateSwapChainForHwnd(queue, (HWND)swapchainCI.windowHandle, &swapchainDesc, nullptr, nullptr, &swapchain), "Failed to create Swapchain.");
    return swapchain;
}

void GraphicsAPI_D3D12::DestroyDesktopSwapchain(void *&swapchain) {
    IDXGISwapChain *d3d12Swapchain = reinterpret_cast<IDXGISwapChain *>(swapchain);
    D3D12_SAFE_RELEASE(d3d12Swapchain);
    swapchain = nullptr;
}

void *GraphicsAPI_D3D12::GetDesktopSwapchainImage(void *swapchain, uint32_t index) {
    IDXGISwapChain *d3d12Swapchain = reinterpret_cast<IDXGISwapChain *>(swapchain);
    ID3D12Resource *texture = nullptr;
    D3D12_CHECK(d3d12Swapchain->GetBuffer(index, IID_PPV_ARGS(&texture)), "Failed to get Swapchain Image.");
    return texture;
}

void GraphicsAPI_D3D12::AcquireDesktopSwapchanImage(void *swapchain, uint32_t &index) {
    IDXGISwapChain3 *d3d12Swapchain = reinterpret_cast<IDXGISwapChain3 *>(swapchain);
    index = d3d12Swapchain->GetCurrentBackBufferIndex();
    currentDesktopSwapchainImage = (ID3D12Resource *)GetDesktopSwapchainImage(swapchain, index);
    imageStates[currentDesktopSwapchainImage] = D3D12_RESOURCE_STATE_COMMON;
}

void GraphicsAPI_D3D12::PresentDesktopSwapchainImage(void *swapchain, uint32_t index) {
    imageStates.erase(currentDesktopSwapchainImage);
    currentDesktopSwapchainImage = nullptr;
    IDXGISwapChain *d3d12Swapchain = reinterpret_cast<IDXGISwapChain *>(swapchain);
    if (DesktopSwapchainVsync) {
        D3D12_CHECK(d3d12Swapchain->Present(1, 0), "Failed to present the Image from Swapchain.");
    } else {
        D3D12_CHECK(d3d12Swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING), "Failed to present the Image from Swapchain.");
    }
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_GetGraphicsBinding
void *GraphicsAPI_D3D12::GetGraphicsBinding() {
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};
    graphicsBinding.device = device;
    graphicsBinding.queue = queue;
    return &graphicsBinding;
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D12_GetGraphicsBinding

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_AllocateSwapchainImageData
XrSwapchainImageBaseHeader *GraphicsAPI_D3D12::AllocateSwapchainImageData(XrSwapchain swapchain, SwapchainType type, uint32_t count) {
    swapchainImagesMap[swapchain].first = type;
    swapchainImagesMap[swapchain].second.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImagesMap[swapchain].second.data());
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
    if (BitwiseCheck(desc.Flags, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)) {
        useClear = true;
        clear.Format = desc.Format;
        clear.Color[0] = 0.0f;
        clear.Color[1] = 0.0f;
        clear.Color[2] = 0.0f;
        clear.Color[3] = 0.0f;
    }
    if (BitwiseCheck(desc.Flags, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)) {
        useClear = true;
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
    imageStates[texture] = D3D12_RESOURCE_STATE_COMMON;

    return texture;
}

void GraphicsAPI_D3D12::DestroyImage(void *&image) {
    ID3D12Resource *d3d12Image = reinterpret_cast<ID3D12Resource *>(image);
    ID3D12Heap *heap = imageResources[d3d12Image];
    imageResources.erase(d3d12Image);
    imageStates.erase(d3d12Image);
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
        imageViewResources[rtv.ptr] = {descHeap, (ID3D12Resource *)imageViewCI.image};
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
        imageViewResources[dsv.ptr] = {descHeap, (ID3D12Resource *)imageViewCI.image};
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
        srv = descHeap->GetCPUDescriptorHandleForHeapStart();
        device->CreateShaderResourceView((ID3D12Resource *)imageViewCI.image, &srvDesc, srv);
        imageViewResources[srv.ptr] = {descHeap, (ID3D12Resource *)imageViewCI.image};
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
        uav = descHeap->GetCPUDescriptorHandleForHeapStart();
        device->CreateUnorderedAccessView((ID3D12Resource *)imageViewCI.image, nullptr, &uavDesc, uav);
        imageViewResources[uav.ptr] = {descHeap, (ID3D12Resource *)imageViewCI.image};
        return (void *)uav.ptr;
    } else {
        DEBUG_BREAK;
        std::cout << "ERROR: D3D12: Unknown ImageView Type." << std::endl;
        return nullptr;
    }
}

void GraphicsAPI_D3D12::DestroyImageView(void *&imageView) {
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12ImageView = {(SIZE_T)imageView};
    ID3D12DescriptorHeap *descHeap = imageViewResources[d3d12ImageView.ptr].first;
    imageViewResources.erase(d3d12ImageView.ptr);
    D3D12_SAFE_RELEASE(descHeap);
    imageView = nullptr;
}

void *GraphicsAPI_D3D12::CreateSampler(const SamplerCreateInfo &samplerCI) {
    D3D12_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = ToD3D12Filter(samplerCI.magFilter, samplerCI.minFilter, samplerCI.mipmapMode, false);
    samplerDesc.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(static_cast<uint32_t>(samplerCI.addressModeS) + 1);
    samplerDesc.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(static_cast<uint32_t>(samplerCI.addressModeT) + 1);
    samplerDesc.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(static_cast<uint32_t>(samplerCI.addressModeR) + 1);
    samplerDesc.MipLODBias = samplerCI.mipLodBias;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(static_cast<uint32_t>(samplerCI.compareOp) + 1);
    samplerDesc.BorderColor[0] = samplerCI.borderColor[0];
    samplerDesc.BorderColor[1] = samplerCI.borderColor[0];
    samplerDesc.BorderColor[2] = samplerCI.borderColor[0];
    samplerDesc.BorderColor[3] = samplerCI.borderColor[0];
    samplerDesc.MinLOD = samplerCI.minLod;
    samplerDesc.MaxLOD = samplerCI.maxLod;

    D3D12_CPU_DESCRIPTOR_HANDLE sampler = {};
    ID3D12DescriptorHeap *descHeap;
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    descHeapDesc.NumDescriptors = 1;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    descHeapDesc.NodeMask = 0;
    D3D12_CHECK(device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap)), "Failed to create DescriptorHeap.");
    sampler = descHeap->GetCPUDescriptorHandleForHeapStart();
    device->CreateSampler(&samplerDesc, sampler);
    samplerResources[sampler.ptr] = descHeap;
    return (void *)sampler.ptr;
}

void GraphicsAPI_D3D12::DestroySampler(void *&sampler) {
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12Sampler = {(SIZE_T)sampler};
    ID3D12DescriptorHeap *descHeap = samplerResources[d3d12Sampler.ptr];
    samplerResources.erase(d3d12Sampler.ptr);
    D3D12_SAFE_RELEASE(descHeap);
    sampler = nullptr;
}

void *GraphicsAPI_D3D12::CreateBuffer(const BufferCreateInfo &bufferCI) {
    ID3D12Resource *buffer = nullptr;

    size_t size = bufferCI.size;
    if (bufferCI.type == BufferCreateInfo::Type::UNIFORM) {
        size = Align<size_t>(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
    }

    D3D12_RESOURCE_DESC desc;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc = {1, 0};
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_CLEAR_VALUE *clear = nullptr;

    ID3D12Heap *heap = nullptr;
    D3D12_RESOURCE_ALLOCATION_INFO allocInfo = device->GetResourceAllocationInfo(0, 1, &desc);
    D3D12_HEAP_DESC heapDesc;
    heapDesc.SizeInBytes = allocInfo.SizeInBytes;
    heapDesc.Properties = {D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0};
    heapDesc.Alignment = allocInfo.Alignment;
    heapDesc.Flags = D3D12_HEAP_FLAG_NONE;
    D3D12_CHECK(device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)), "Failed to create Heap.")

    D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_GENERIC_READ;
    if (heapDesc.Properties.Type == D3D12_HEAP_TYPE_DEFAULT) {
        if (bufferCI.type == BufferCreateInfo::Type::VERTEX) {
            initState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        } else if (bufferCI.type == BufferCreateInfo::Type::INDEX) {
            initState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
        } else if (bufferCI.type == BufferCreateInfo::Type::UNIFORM) {
            initState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        } else {
            std::cout << "ERROR: D3D12: Unknown Buffer Type." << std::endl;
        }
    }

    D3D12_CHECK(device->CreatePlacedResource(heap, 0, &desc, initState, clear, IID_PPV_ARGS(&buffer)), "Failed to create Buffer.");

    SetBufferData(buffer, 0, bufferCI.size, bufferCI.data);
    bufferResources[buffer] = {heap, bufferCI};

    return buffer;
}

void GraphicsAPI_D3D12::DestroyBuffer(void *&buffer) {
    ID3D12Resource *d3d12Buffer = reinterpret_cast<ID3D12Resource *>(buffer);
    ID3D12Heap *heap = bufferResources[d3d12Buffer].first;
    bufferResources.erase(d3d12Buffer);
    D3D12_SAFE_RELEASE(heap);
    D3D12_SAFE_RELEASE(d3d12Buffer);
    buffer = nullptr;
}

void *GraphicsAPI_D3D12::CreateShader(const ShaderCreateInfo &shaderCI) {
    D3D12_SHADER_BYTECODE *byteCode = new D3D12_SHADER_BYTECODE();

    shaders[byteCode].first.resize(shaderCI.sourceSize);
    memcpy(shaders[byteCode].first.data(), shaderCI.sourceData, shaderCI.sourceSize);
    shaders[byteCode].second = shaderCI;

    byteCode->pShaderBytecode = shaders[byteCode].first.data();
    byteCode->BytecodeLength = shaders[byteCode].first.size();

    return byteCode;
}

void GraphicsAPI_D3D12::DestroyShader(void *&shader) {
    D3D12_SHADER_BYTECODE *byteCode = reinterpret_cast<D3D12_SHADER_BYTECODE *>(shader);
    shaders.erase(byteCode);
    delete byteCode;
    shader = nullptr;
}

void *GraphicsAPI_D3D12::CreatePipeline(const PipelineCreateInfo &pipelineCI) {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GPSD = {};

    // ShaderStages
    for (void *shader : pipelineCI.shaders) {
        D3D12_SHADER_BYTECODE *byteCode = reinterpret_cast<D3D12_SHADER_BYTECODE *>(shader);

        const ShaderCreateInfo &shaderCreateInfo = shaders[byteCode].second;
        switch (shaderCreateInfo.type) {
        case ShaderCreateInfo::Type::VERTEX:
            GPSD.VS = *byteCode;
            continue;
        case ShaderCreateInfo::Type::FRAGMENT:
            GPSD.PS = *byteCode;
            continue;
        case ShaderCreateInfo::Type::TESSELLATION_EVALUATION:
            GPSD.DS = *byteCode;
            continue;
        case ShaderCreateInfo::Type::TESSELLATION_CONTROL:
            GPSD.HS = *byteCode;
            continue;
        case ShaderCreateInfo::Type::GEOMETRY:
            GPSD.GS = *byteCode;
            continue;
        default:
            continue;
        }
    }

    // VertexInput
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
    for (auto &attrib : pipelineCI.vertexInputState.attributes) {
        D3D12_INPUT_ELEMENT_DESC il;
        il.SemanticName = attrib.semanticName;
        il.SemanticIndex = attrib.attribIndex;
        il.Format = ToDXGI_FORMAT(attrib.vertexType);
        il.InputSlot = attrib.bindingIndex;
        il.AlignedByteOffset = attrib.offset;
        il.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        il.InstanceDataStepRate = 0;
        inputLayout.push_back(il);
    }
    GPSD.InputLayout = {inputLayout.data(), (UINT)inputLayout.size()};

    // InputAssembly
    switch (pipelineCI.inputAssemblyState.topology) {
    case GraphicsAPI::PrimitiveTopology::POINT_LIST:
        GPSD.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        break;
    case GraphicsAPI::PrimitiveTopology::LINE_LIST:
    case GraphicsAPI::PrimitiveTopology::LINE_STRIP:
        GPSD.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        break;
    case GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST:
    case GraphicsAPI::PrimitiveTopology::TRIANGLE_STRIP:
    case GraphicsAPI::PrimitiveTopology::TRIANGLE_FAN:
        GPSD.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        break;
    default:
        GPSD.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        break;
    }

    // Tessellation
    GPSD.StreamOutput = {};

    // Rasterisation
    GPSD.RasterizerState.FillMode = pipelineCI.rasterisationState.polygonMode == GraphicsAPI::PolygonMode::LINE ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
    GPSD.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(static_cast<uint32_t>(pipelineCI.rasterisationState.cullMode) % 3 + 1);  //%3 because d3d12 has no FRONT_AND_BACK
    GPSD.RasterizerState.FrontCounterClockwise = !static_cast<bool>(pipelineCI.rasterisationState.frontFace);
    GPSD.RasterizerState.DepthBias = static_cast<INT>(pipelineCI.rasterisationState.depthBiasConstantFactor);
    GPSD.RasterizerState.DepthBiasClamp = pipelineCI.rasterisationState.depthBiasClamp;
    GPSD.RasterizerState.SlopeScaledDepthBias = pipelineCI.rasterisationState.depthBiasSlopeFactor;
    GPSD.RasterizerState.DepthClipEnable = pipelineCI.rasterisationState.depthClampEnable;
    GPSD.RasterizerState.MultisampleEnable = pipelineCI.multisampleState.rasterisationSamples > 1;      // Sets AA algorithm
    GPSD.RasterizerState.AntialiasedLineEnable = pipelineCI.multisampleState.rasterisationSamples > 1;  // Sets AA algorithm
    GPSD.RasterizerState.ForcedSampleCount = 0;
    GPSD.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // Multisample
    GPSD.SampleDesc.Count = static_cast<UINT>(pipelineCI.multisampleState.rasterisationSamples);
    GPSD.SampleDesc.Quality = 0;
    GPSD.SampleMask = static_cast<UINT>(pipelineCI.multisampleState.sampleMask);

    // DepthStencil
    GPSD.DepthStencilState.DepthEnable = pipelineCI.depthStencilState.depthTestEnable;
    GPSD.DepthStencilState.DepthWriteMask = static_cast<D3D12_DEPTH_WRITE_MASK>(pipelineCI.depthStencilState.depthWriteEnable);
    GPSD.DepthStencilState.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(static_cast<uint32_t>(pipelineCI.depthStencilState.depthCompareOp) + 1);
    GPSD.DepthStencilState.StencilEnable = pipelineCI.depthStencilState.stencilTestEnable;
    GPSD.DepthStencilState.StencilReadMask = static_cast<UINT8>(GPSD.RasterizerState.CullMode == D3D12_CULL_MODE_FRONT ? pipelineCI.depthStencilState.back.compareMask : pipelineCI.depthStencilState.front.compareMask);
    GPSD.DepthStencilState.StencilWriteMask = static_cast<UINT8>(GPSD.RasterizerState.CullMode == D3D12_CULL_MODE_FRONT ? pipelineCI.depthStencilState.back.writeMask : pipelineCI.depthStencilState.front.writeMask);
    GPSD.DepthStencilState.FrontFace.StencilFailOp = static_cast<D3D12_STENCIL_OP>(static_cast<uint32_t>(pipelineCI.depthStencilState.front.failOp) + 1);
    GPSD.DepthStencilState.FrontFace.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(static_cast<uint32_t>(pipelineCI.depthStencilState.front.depthFailOp) + 1);
    GPSD.DepthStencilState.FrontFace.StencilPassOp = static_cast<D3D12_STENCIL_OP>(static_cast<uint32_t>(pipelineCI.depthStencilState.front.passOp) + 1);
    GPSD.DepthStencilState.FrontFace.StencilFunc = static_cast<D3D12_COMPARISON_FUNC>(static_cast<uint32_t>(pipelineCI.depthStencilState.front.compareOp) + 1);
    GPSD.DepthStencilState.BackFace.StencilFailOp = static_cast<D3D12_STENCIL_OP>(static_cast<uint32_t>(pipelineCI.depthStencilState.back.failOp) + 1);
    GPSD.DepthStencilState.BackFace.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(static_cast<uint32_t>(pipelineCI.depthStencilState.back.depthFailOp) + 1);
    GPSD.DepthStencilState.BackFace.StencilPassOp = static_cast<D3D12_STENCIL_OP>(static_cast<uint32_t>(pipelineCI.depthStencilState.back.passOp) + 1);
    GPSD.DepthStencilState.BackFace.StencilFunc = static_cast<D3D12_COMPARISON_FUNC>(static_cast<uint32_t>(pipelineCI.depthStencilState.back.compareOp) + 1);

    // ColorBlend
    GPSD.BlendState.AlphaToCoverageEnable = pipelineCI.multisampleState.alphaToCoverageEnable;
    GPSD.BlendState.IndependentBlendEnable = true;
    size_t i = 0;
    for (auto &blend : pipelineCI.colorBlendState.attachments) {
        GPSD.BlendState.RenderTarget[i].BlendEnable = blend.blendEnable;
        GPSD.BlendState.RenderTarget[i].LogicOpEnable = pipelineCI.colorBlendState.logicOpEnable;
        GPSD.BlendState.RenderTarget[i].SrcBlend = ToD3D12_BLEND(blend.srcColorBlendFactor);
        GPSD.BlendState.RenderTarget[i].DestBlend = ToD3D12_BLEND(blend.dstColorBlendFactor);
        GPSD.BlendState.RenderTarget[i].BlendOp = static_cast<D3D12_BLEND_OP>(static_cast<uint32_t>(blend.colorBlendOp) + 1);
        GPSD.BlendState.RenderTarget[i].SrcBlendAlpha = ToD3D12_BLEND(blend.srcAlphaBlendFactor);
        GPSD.BlendState.RenderTarget[i].DestBlendAlpha = ToD3D12_BLEND(blend.dstAlphaBlendFactor);
        GPSD.BlendState.RenderTarget[i].BlendOpAlpha = static_cast<D3D12_BLEND_OP>(static_cast<uint32_t>(blend.alphaBlendOp) + 1);
        GPSD.BlendState.RenderTarget[i].LogicOp = ToD3D12_LOGIC_OP(pipelineCI.colorBlendState.logicOp);
        GPSD.BlendState.RenderTarget[i].RenderTargetWriteMask = static_cast<UINT8>(blend.colorWriteMask);

        i++;
        if (i >= 8)
            break;
    }

    // RTV and DSV
    size_t j = 0;
    for (auto &colorFormat : pipelineCI.colorFormats) {
        GPSD.RTVFormats[j] = (DXGI_FORMAT)colorFormat;

        j++;
        if (j >= 8)
            break;
    }
    GPSD.NumRenderTargets = static_cast<UINT>(j);
    GPSD.DSVFormat = (DXGI_FORMAT)pipelineCI.depthFormat;

    if (GPSD.DSVFormat == DXGI_FORMAT_UNKNOWN) {  // If no DSV, then DepthStencilState must be null.
        GPSD.DepthStencilState = {};
    }

    // RootSignature
    ID3D12RootSignature *rootSignature = nullptr;
    ID3DBlob *serializedRootSignature = nullptr;
    ID3DBlob *serializedRootSignatureError = nullptr;
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

    std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
    for (const DescriptorInfo &descInfo : pipelineCI.layout) {
        D3D12_DESCRIPTOR_RANGE descriptorRange = {};

        switch (descInfo.type) {
        case DescriptorInfo::Type::BUFFER: {
            if (descInfo.readWrite) {
                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            } else {
                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            }
            break;
        }
        case DescriptorInfo::Type::IMAGE: {
            if (descInfo.readWrite) {
                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            } else {
                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            }
            break;
        }
        case DescriptorInfo::Type::SAMPLER: {
            descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            break;
        }
        }

        descriptorRange.NumDescriptors = 1;
        descriptorRange.BaseShaderRegister = descInfo.bindingIndex;
        descriptorRange.RegisterSpace = 0;
        descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        descriptorRanges.push_back(descriptorRange);
    }

    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    std::vector<D3D12_ROOT_DESCRIPTOR_TABLE> descriptorTables;
    for (const D3D12_DESCRIPTOR_RANGE &descRange : descriptorRanges) {
        D3D12_ROOT_DESCRIPTOR_TABLE descTable = {1, &descRange};
        descriptorTables.push_back(descTable);

        D3D12_ROOT_PARAMETER rootParameter;
        rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameter.DescriptorTable = descTable;
        rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters.push_back(rootParameter);
    }

    rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
    rootSignatureDesc.pParameters = rootParameters.data();
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    HRESULT res = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializedRootSignature, &serializedRootSignatureError);
    if (serializedRootSignatureError) {
        std::string errorStr = std::string((char *)serializedRootSignatureError->GetBufferPointer());
        std::cout << "Error in serialising RootSignature: " + errorStr << std::endl;
    }
    D3D12_CHECK(res, "Failed to serialise RootSignature.");
    D3D12_CHECK(device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "Failed to create RootSignature.");

    GPSD.pRootSignature = rootSignature;
    GPSD.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    GPSD.NodeMask = 0;
    GPSD.CachedPSO = {};
    GPSD.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    ID3D12PipelineState *pipeline = nullptr;
    D3D12_CHECK(device->CreateGraphicsPipelineState(&GPSD, IID_PPV_ARGS(&pipeline)), "Failed to create Pipeline");

    D3D12_SAFE_RELEASE(serializedRootSignature);
    D3D12_SAFE_RELEASE(serializedRootSignatureError);

    pipelineResources[pipeline] = {rootSignature, pipelineCI};

    return pipeline;
}

void GraphicsAPI_D3D12::DestroyPipeline(void *&pipeline) {
    ID3D12PipelineState *d3d12Pipeline = reinterpret_cast<ID3D12PipelineState *>(pipeline);
    ID3D12RootSignature *rootSignature = pipelineResources[d3d12Pipeline].first;
    pipelineResources.erase(d3d12Pipeline);
    D3D12_SAFE_RELEASE(d3d12Pipeline);
    D3D12_SAFE_RELEASE(rootSignature);
    pipeline = nullptr;
}

void GraphicsAPI_D3D12::BeginRendering() {
    D3D12_CHECK(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)), "Failed to create CommandAllocator.");
    D3D12_CHECK(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList)), "Failed to create CommandList.");

    setDescriptorHeap = true;
    CBV_SRV_UAV_DescriptorOffset = 0;
    SAMPLER_DescriptorOffset = 0;

    if (currentDesktopSwapchainImage) {
        D3D12_RESOURCE_BARRIER swapchainImageBarrier;
        swapchainImageBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        swapchainImageBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        swapchainImageBarrier.Transition.pResource = currentDesktopSwapchainImage;
        swapchainImageBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        swapchainImageBarrier.Transition.StateBefore = imageStates[currentDesktopSwapchainImage];
        swapchainImageBarrier.Transition.StateAfter = imageStates[currentDesktopSwapchainImage] = D3D12_RESOURCE_STATE_RENDER_TARGET;
        cmdList->ResourceBarrier(1, &swapchainImageBarrier);
    }
}

void GraphicsAPI_D3D12::EndRendering() {
    if (currentDesktopSwapchainImage) {
        D3D12_RESOURCE_BARRIER swapchainImageBarrier;
        swapchainImageBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        swapchainImageBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        swapchainImageBarrier.Transition.pResource = currentDesktopSwapchainImage;
        swapchainImageBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        swapchainImageBarrier.Transition.StateBefore = imageStates[currentDesktopSwapchainImage];
        swapchainImageBarrier.Transition.StateAfter = imageStates[currentDesktopSwapchainImage] = D3D12_RESOURCE_STATE_COMMON;
        cmdList->ResourceBarrier(1, &swapchainImageBarrier);
    }

    D3D12_CHECK(cmdList->Close(), "Failed to close CommandList");
    queue->ExecuteCommandLists(1, (ID3D12CommandList **)&cmdList);

    ID3D12Fence *fence = nullptr;
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    queue->Signal(fence, 64);
    while (fence->GetCompletedValue() != 64) {
    }
    D3D12_SAFE_RELEASE(fence);

    D3D12_SAFE_RELEASE(cmdList);
    D3D12_SAFE_RELEASE(cmdAllocator);
}

void GraphicsAPI_D3D12::ClearColor(void *imageView, float r, float g, float b, float a) {
    ID3D12Resource *image = imageViewResources[(SIZE_T)imageView].second;
    if (imageStates[image] != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        D3D12_RESOURCE_BARRIER barrier;
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = image;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = imageStates[currentDesktopSwapchainImage];
        barrier.Transition.StateAfter = imageStates[currentDesktopSwapchainImage] = D3D12_RESOURCE_STATE_RENDER_TARGET;
        cmdList->ResourceBarrier(1, &barrier);
    }

    const FLOAT clearColor[4] = {r, g, b, a};
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12ImageView = {(SIZE_T)imageView};
    cmdList->ClearRenderTargetView(d3d12ImageView, clearColor, 0, nullptr);
}

void GraphicsAPI_D3D12::ClearDepth(void *imageView, float d) {
    ID3D12Resource *image = imageViewResources[(SIZE_T)imageView].second;
    if (imageStates[image] != D3D12_RESOURCE_STATE_DEPTH_WRITE) {
        D3D12_RESOURCE_BARRIER barrier;
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = image;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = imageStates[image];
        barrier.Transition.StateAfter = imageStates[image] = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        cmdList->ResourceBarrier(1, &barrier);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE d3d12ImageView = {(SIZE_T)imageView};
    cmdList->ClearDepthStencilView(d3d12ImageView, D3D12_CLEAR_FLAG_DEPTH, d, 0, 0, nullptr);
}

void GraphicsAPI_D3D12::SetBufferData(void *buffer, size_t offset, size_t size, void *data) {
    ID3D12Resource *d3d12Buffer = (ID3D12Resource *)buffer;
    void *mappedData = nullptr;
    D3D12_RANGE readRange = {0, 0};
    D3D12_CHECK(d3d12Buffer->Map(0, &readRange, &mappedData), "Failed to map Resource.");
    if (mappedData && data)
        memcpy((char *)mappedData + offset, data, size);
    d3d12Buffer->Unmap(0, nullptr);
}

void GraphicsAPI_D3D12::SetRenderAttachments(void **colorViews, size_t colorViewCount, void *depthStencilView, uint32_t width, uint32_t height, void *pipeline) {
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> d3d12RTVs;
    d3d12RTVs.reserve(colorViewCount);
    for (size_t i = 0; i < colorViewCount; i++) {
        d3d12RTVs.push_back({(SIZE_T)colorViews[i]});
    }
    D3D12_CPU_DESCRIPTOR_HANDLE d3d12DSV = {(SIZE_T)depthStencilView};

    cmdList->OMSetRenderTargets((UINT)colorViewCount, d3d12RTVs.data(), false, &d3d12DSV);
}

void GraphicsAPI_D3D12::SetViewports(Viewport *viewports, size_t count) {
    std::vector<D3D12_VIEWPORT> d3d12Viewports;
    d3d12Viewports.reserve(count);
    for (size_t i = 0; i < count; i++) {
        const Viewport &viewport = viewports[i];
        d3d12Viewports.push_back({viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth});
    }

    cmdList->RSSetViewports(static_cast<UINT>(d3d12Viewports.size()), d3d12Viewports.data());
}

void GraphicsAPI_D3D12::SetScissors(Rect2D *scissors, size_t count) {
    std::vector<D3D12_RECT> d3d12Scissors;
    d3d12Scissors.reserve(count);
    for (size_t i = 0; i < count; i++) {
        const Rect2D &scissor = scissors[i];
        d3d12Scissors.push_back({static_cast<LONG>(scissor.offset.x), static_cast<LONG>(scissor.offset.y), static_cast<LONG>(scissor.extent.width), static_cast<LONG>(scissor.extent.height)});
    }

    cmdList->RSSetScissorRects(static_cast<UINT>(d3d12Scissors.size()), d3d12Scissors.data());
}

void GraphicsAPI_D3D12::SetPipeline(void *pipeline) {
    ID3D12PipelineState *d3d12Pipeline = reinterpret_cast<ID3D12PipelineState *>(pipeline);
    setPipeline = d3d12Pipeline;

    const PipelineCreateInfo &pipelineCI = pipelineResources[d3d12Pipeline].second;

    cmdList->SetPipelineState(d3d12Pipeline);
    cmdList->SetGraphicsRootSignature(pipelineResources[d3d12Pipeline].first);
    cmdList->IASetPrimitiveTopology(ToD3D12_PRIMITIVE_TOPOLOGY(pipelineCI.inputAssemblyState.topology));
}

void GraphicsAPI_D3D12::SetDescriptor(const DescriptorInfo &descriptorInfo) {
    if (setDescriptorHeap) {
        ID3D12DescriptorHeap *heaps[2] = {CBV_SRV_UAV_DescriptorHeap, SAMPLER_DescriptorHeap};
        cmdList->SetDescriptorHeaps(2, heaps);
        setDescriptorHeap = false;
    }
    descriptorInfos.push_back(descriptorInfo);
}

void GraphicsAPI_D3D12::UpdateDescriptors() {
    UINT CBV_SRV_UAV_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    UINT SAMPLER_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    UINT Current_CBV_SRV_UAV_DescriptorOffset = CBV_SRV_UAV_DescriptorOffset;
    UINT Current_SAMPLER_DescriptorOffset = SAMPLER_DescriptorOffset;

    size_t rootParameterIndex = 0;
    for (const DescriptorInfo &descriptorInfo : descriptorInfos) {
        switch (descriptorInfo.type) {
        case DescriptorInfo::Type::BUFFER: {
            D3D12_CPU_DESCRIPTOR_HANDLE destCpuHandle = {};
            destCpuHandle.ptr = CBV_SRV_UAV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + Current_CBV_SRV_UAV_DescriptorOffset;
            D3D12_GPU_DESCRIPTOR_HANDLE destGpuHandle = {};
            destGpuHandle.ptr = CBV_SRV_UAV_DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + Current_CBV_SRV_UAV_DescriptorOffset;

            ID3D12Resource *d3d12Buffer = reinterpret_cast<ID3D12Resource *>(descriptorInfo.resource);
            const BufferCreateInfo &bufferCI = bufferResources[d3d12Buffer].second;

            if (descriptorInfo.readWrite) {
                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
                uavDesc.Format = d3d12Buffer->GetDesc().Format;
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
                uavDesc.Buffer.FirstElement = 0;
                uavDesc.Buffer.NumElements = static_cast<UINT>(descriptorInfo.bufferSize / bufferCI.stride);
                uavDesc.Buffer.StructureByteStride = static_cast<UINT>(bufferCI.stride);
                uavDesc.Buffer.CounterOffsetInBytes = descriptorInfo.bufferOffset;
                uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
                device->CreateUnorderedAccessView(d3d12Buffer, nullptr, &uavDesc, destCpuHandle);
            } else {
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
                cbvDesc.BufferLocation = d3d12Buffer->GetGPUVirtualAddress() + descriptorInfo.bufferOffset;
                cbvDesc.SizeInBytes = Align<UINT>(static_cast<UINT>(descriptorInfo.bufferSize), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
                device->CreateConstantBufferView(&cbvDesc, destCpuHandle);
            }

            cmdList->SetGraphicsRootDescriptorTable(rootParameterIndex, destGpuHandle);
            Current_CBV_SRV_UAV_DescriptorOffset += 1 * CBV_SRV_UAV_DescriptorSize;
            break;
        }
        case DescriptorInfo::Type::IMAGE: {
            D3D12_CPU_DESCRIPTOR_HANDLE srcCpuHandle = {(SIZE_T)descriptorInfo.resource};

            D3D12_CPU_DESCRIPTOR_HANDLE destCpuHandle = {};
            destCpuHandle.ptr = CBV_SRV_UAV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + Current_CBV_SRV_UAV_DescriptorOffset;
            D3D12_GPU_DESCRIPTOR_HANDLE destGpuHandle = {};
            destGpuHandle.ptr = CBV_SRV_UAV_DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + Current_CBV_SRV_UAV_DescriptorOffset;

            device->CopyDescriptorsSimple(1, destCpuHandle, srcCpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

            cmdList->SetGraphicsRootDescriptorTable(rootParameterIndex, destGpuHandle);
            Current_CBV_SRV_UAV_DescriptorOffset += 1 * CBV_SRV_UAV_DescriptorSize;
            break;
        }
        case DescriptorInfo::Type::SAMPLER: {
            D3D12_CPU_DESCRIPTOR_HANDLE srcCpuHandle = {(SIZE_T)descriptorInfo.resource};
            D3D12_CPU_DESCRIPTOR_HANDLE destCpuHandle = {};
            destCpuHandle.ptr = SAMPLER_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + Current_SAMPLER_DescriptorOffset;
            D3D12_GPU_DESCRIPTOR_HANDLE destGpuHandle = {};
            destGpuHandle.ptr = SAMPLER_DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + Current_SAMPLER_DescriptorOffset;

            device->CopyDescriptorsSimple(1, destCpuHandle, srcCpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

            cmdList->SetGraphicsRootDescriptorTable(rootParameterIndex, destGpuHandle);
            Current_SAMPLER_DescriptorOffset += 1 * SAMPLER_DescriptorSize;
            break;
        }
        default: {
            continue;
        }
        }

        rootParameterIndex++;
    }

    CBV_SRV_UAV_DescriptorOffset = Current_CBV_SRV_UAV_DescriptorOffset;
    SAMPLER_DescriptorOffset = Current_SAMPLER_DescriptorOffset;

    descriptorInfos.clear();
}

void GraphicsAPI_D3D12::SetVertexBuffers(void **vertexBuffers, size_t count) {
    std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
    vertexBufferViews.reserve(count);
    for (size_t i = 0; i < count; i++) {
        const PipelineCreateInfo &pipelineCI = pipelineResources[setPipeline].second;
        for (const VertexInputBinding &vertexBinding : pipelineCI.vertexInputState.bindings) {
            if (vertexBinding.bindingIndex == (uint32_t)i) {
                D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
                ID3D12Resource *d3d12VertexBuffer = reinterpret_cast<ID3D12Resource *>(vertexBuffers[i]);
                vertexBufferView.BufferLocation = d3d12VertexBuffer->GetGPUVirtualAddress();
                vertexBufferView.SizeInBytes = d3d12VertexBuffer->GetDesc().Width;
                vertexBufferView.StrideInBytes = vertexBinding.stride;
                vertexBufferViews.push_back(vertexBufferView);
            }
        }
    }
    cmdList->IASetVertexBuffers(0, static_cast<UINT>(vertexBufferViews.size()), vertexBufferViews.data());
}

void GraphicsAPI_D3D12::SetIndexBuffer(void *indexBuffer) {
    ID3D12Resource *d3d12IndexBuffer = reinterpret_cast<ID3D12Resource *>(indexBuffer);
    const BufferCreateInfo &bufferCI = bufferResources[d3d12IndexBuffer].second;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    indexBufferView.BufferLocation = d3d12IndexBuffer->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = d3d12IndexBuffer->GetDesc().Width;
    indexBufferView.Format = bufferCI.stride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
    cmdList->IASetIndexBuffer(&indexBufferView);
}

void GraphicsAPI_D3D12::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    cmdList->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void GraphicsAPI_D3D12::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    cmdList->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D12_GetSupportedSwapchainFormats
const std::vector<int64_t> GraphicsAPI_D3D12::GetSupportedColorSwapchainFormats() {
    return {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB};
}
const std::vector<int64_t> GraphicsAPI_D3D12::GetSupportedDepthSwapchainFormats() {
    return {
        DXGI_FORMAT_D32_FLOAT,
        DXGI_FORMAT_D16_UNORM};
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D12_GetSupportedSwapchainFormats
#endif
