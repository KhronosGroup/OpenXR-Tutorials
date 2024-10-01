// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#include <GraphicsAPI_D3D11.h>

#if defined(XR_USE_GRAPHICS_API_D3D11)

#define D3D11_CHECK(x, y)                                                                         \
    {                                                                                             \
        HRESULT result = (x);                                                                     \
        if (FAILED(result)) {                                                                     \
            std::cout << "ERROR: D3D11: " << std::hex << "0x" << result << std::dec << std::endl; \
            std::cout << "ERROR: D3D11: " << y << std::endl;                                      \
        }                                                                                         \
    }

#define D3D11_SAFE_RELEASE(p) \
    {                         \
        if (p) {              \
            (p)->Release();   \
            (p) = nullptr;    \
        }                     \
    }

static D3D11_BIND_FLAG ToD3D11BindFlag(GraphicsAPI::BufferCreateInfo::Type type) {
    switch (type) {
    case GraphicsAPI::BufferCreateInfo::Type::VERTEX: {
        return D3D11_BIND_VERTEX_BUFFER;
    }
    case GraphicsAPI::BufferCreateInfo::Type::INDEX: {
        return D3D11_BIND_INDEX_BUFFER;
    }
    case GraphicsAPI::BufferCreateInfo::Type::UNIFORM:
    default: {
        return D3D11_BIND_CONSTANT_BUFFER;
    }
    };
}

static D3D11_TEXTURE_ADDRESS_MODE ToD3D11TextureAddressMode(GraphicsAPI::SamplerCreateInfo::AddressMode addressMode) {
    if (addressMode == GraphicsAPI::SamplerCreateInfo::AddressMode::CLAMP_TO_BORDER) {
        if (addressMode == GraphicsAPI::SamplerCreateInfo::AddressMode::CLAMP_TO_EDGE) {
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        }
    }
    if (addressMode == GraphicsAPI::SamplerCreateInfo::AddressMode::REPEAT) {
        return D3D11_TEXTURE_ADDRESS_WRAP;
    }
    if (addressMode == GraphicsAPI::SamplerCreateInfo::AddressMode::MIRROR_CLAMP_TO_EDGE) {
        if (addressMode == GraphicsAPI::SamplerCreateInfo::AddressMode::MIRRORED_REPEAT) {
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        }
    }
    return D3D11_TEXTURE_ADDRESS_WRAP;
}

static D3D11_FILTER ToD3D11Filter(GraphicsAPI::SamplerCreateInfo::Filter filter) {
    if (filter == GraphicsAPI::SamplerCreateInfo::Filter::LINEAR)
        return D3D11_FILTER_ANISOTROPIC;
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

static D3D11_COMPARISON_FUNC ToD3D11Comparison(GraphicsAPI::CompareOp compareOp) {
    switch (compareOp) {
    case GraphicsAPI::CompareOp::ALWAYS: {
        return D3D11_COMPARISON_ALWAYS;
    }
    case GraphicsAPI::CompareOp::LESS: {
        return D3D11_COMPARISON_LESS;
    }
    case GraphicsAPI::CompareOp::EQUAL: {
        return D3D11_COMPARISON_EQUAL;
    }
    case GraphicsAPI::CompareOp::LESS_OR_EQUAL: {
        return D3D11_COMPARISON_LESS_EQUAL;
    }
    case GraphicsAPI::CompareOp::GREATER: {
        return D3D11_COMPARISON_GREATER;
    }
    case GraphicsAPI::CompareOp::NOT_EQUAL: {
        return D3D11_COMPARISON_NOT_EQUAL;
    }
    case GraphicsAPI::CompareOp::GREATER_OR_EQUAL: {
        return D3D11_COMPARISON_GREATER_EQUAL;
    }
    default:
        break;
    };
    return D3D11_COMPARISON_LESS;
}

static D3D11_STENCIL_OP ToD3D11StencilOp(GraphicsAPI::StencilOp stencilOp) {
    switch (stencilOp) {
    case GraphicsAPI::StencilOp::KEEP: {
        return D3D11_STENCIL_OP_KEEP;
    }
    case GraphicsAPI::StencilOp::ZERO: {
        return D3D11_STENCIL_OP_ZERO;
    }
    case GraphicsAPI::StencilOp::REPLACE: {
        return D3D11_STENCIL_OP_REPLACE;
    }
    case GraphicsAPI::StencilOp::INCREMENT_AND_CLAMP: {
        return D3D11_STENCIL_OP_INCR_SAT;
    }
    case GraphicsAPI::StencilOp::DECREMENT_AND_CLAMP: {
        return D3D11_STENCIL_OP_DECR_SAT;
    }
    case GraphicsAPI::StencilOp::INVERT: {
        return D3D11_STENCIL_OP_INVERT;
    }
    case GraphicsAPI::StencilOp::INCREMENT_AND_WRAP: {
        return D3D11_STENCIL_OP_INCR;
    }
    case GraphicsAPI::StencilOp::DECREMENT_AND_WRAP: {
        return D3D11_STENCIL_OP_DECR;
    }
    default:
        break;
    };
    return D3D11_STENCIL_OP_KEEP;
}

static D3D11_BLEND ToD3D11_BLEND(GraphicsAPI::BlendFactor blend) {
    switch (blend) {
    default:
    case GraphicsAPI::BlendFactor::ZERO:
        return D3D11_BLEND_ZERO;
    case GraphicsAPI::BlendFactor::ONE:
        return D3D11_BLEND_ONE;
    case GraphicsAPI::BlendFactor::SRC_COLOR:
        return D3D11_BLEND_SRC_COLOR;
    case GraphicsAPI::BlendFactor::ONE_MINUS_SRC_COLOR:
        return D3D11_BLEND_INV_SRC_COLOR;
    case GraphicsAPI::BlendFactor::DST_COLOR:
        return D3D11_BLEND_DEST_COLOR;
    case GraphicsAPI::BlendFactor::ONE_MINUS_DST_COLOR:
        return D3D11_BLEND_INV_DEST_COLOR;
    case GraphicsAPI::BlendFactor::SRC_ALPHA:
        return D3D11_BLEND_SRC_ALPHA;
    case GraphicsAPI::BlendFactor::ONE_MINUS_SRC_ALPHA:
        return D3D11_BLEND_INV_SRC_ALPHA;
    case GraphicsAPI::BlendFactor::DST_ALPHA:
        return D3D11_BLEND_DEST_ALPHA;
    case GraphicsAPI::BlendFactor::ONE_MINUS_DST_ALPHA:
        return D3D11_BLEND_INV_DEST_ALPHA;
    }
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

GraphicsAPI_D3D11::GraphicsAPI_D3D11() {
    D3D11_CHECK(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

    IDXGIAdapter *adapter = nullptr;
    DXGI_ADAPTER_DESC adapterDesc = {};
    for(UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        adapter->GetDesc(&adapterDesc);
        break;
    }
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    D3D11_CHECK(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, 0, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION, &device, nullptr, &immediateContext), "Failed to create D3D11 Device.");
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11
GraphicsAPI_D3D11::GraphicsAPI_D3D11(XrInstance m_xrInstance, XrSystemId systemId) {
    OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction *)&xrGetD3D11GraphicsRequirementsKHR), "Failed to get InstanceProcAddr xrGetD3D11GraphicsRequirementsKHR.");
    XrGraphicsRequirementsD3D11KHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
    OPENXR_CHECK(xrGetD3D11GraphicsRequirementsKHR(m_xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for D3D11.");

    D3D11_CHECK(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

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
    OPENXR_CHECK(adapter != nullptr ? XR_SUCCESS : XR_ERROR_VALIDATION_FAILURE, "Failed to find matching graphics adapter from xrGetD3D11GraphicsRequirementsKHR.");

    D3D11_CHECK(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, 0, D3D11_CREATE_DEVICE_DEBUG, &graphicsRequirements.minFeatureLevel, 1, D3D11_SDK_VERSION, &device, nullptr, &immediateContext), "Failed to create D3D11 Device.");

	device->QueryInterface( __uuidof(ID3D11Debug), (void**)&d3dDebug );
	d3dDebug->QueryInterface( __uuidof(ID3D11InfoQueue), (void**)&infoQueue );
	infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
	infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
}

GraphicsAPI_D3D11::~GraphicsAPI_D3D11() {
    D3D11_SAFE_RELEASE(immediateContext);
    D3D11_SAFE_RELEASE(device);
    D3D11_SAFE_RELEASE(factory);
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D11
static bool DesktopSwapchainVsync = false;
void *GraphicsAPI_D3D11::CreateDesktopSwapchain(const SwapchainCreateInfo &swapchainCI) {
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
    D3D11_CHECK(factory->CreateSwapChainForHwnd(device, (HWND)swapchainCI.windowHandle, &swapchainDesc, nullptr, nullptr, &swapchain), "Failed to create Swapchain.");
    return swapchain;
}

void GraphicsAPI_D3D11::DestroyDesktopSwapchain(void *&swapchain) {
    IDXGISwapChain *d3d11Swapchain = reinterpret_cast<IDXGISwapChain *>(swapchain);
    D3D11_SAFE_RELEASE(d3d11Swapchain);
    swapchain = nullptr;
}

void *GraphicsAPI_D3D11::GetDesktopSwapchainImage(void *swapchain, uint32_t index) {
    IDXGISwapChain *d3d11Swapchain = reinterpret_cast<IDXGISwapChain *>(swapchain);
    ID3D11Texture2D *texture = nullptr;
    D3D11_CHECK(d3d11Swapchain->GetBuffer(0, IID_PPV_ARGS(&texture)), "Failed to get Swapchain Image.");
    return texture;
}

void GraphicsAPI_D3D11::AcquireDesktopSwapchanImage(void *swapchain, uint32_t& index) {
    IDXGISwapChain3 *d3d11Swapchain = reinterpret_cast<IDXGISwapChain3 *>(swapchain);
    index = d3d11Swapchain->GetCurrentBackBufferIndex();
}

void GraphicsAPI_D3D11::PresentDesktopSwapchainImage(void *swapchain, uint32_t index) {
    IDXGISwapChain *d3d11Swapchain = reinterpret_cast<IDXGISwapChain *>(swapchain);
    if (DesktopSwapchainVsync) {
        D3D11_CHECK(d3d11Swapchain->Present(1, 0), "Failed to present the Image from Swapchain.");
    } else {
        D3D11_CHECK(d3d11Swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING), "Failed to present the Image from Swapchain.");
    }
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_GetGraphicsBinding
void *GraphicsAPI_D3D11::GetGraphicsBinding() {
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
    graphicsBinding.device = device;
    return &graphicsBinding;
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D11_GetGraphicsBinding

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_AllocateSwapchainImageData
XrSwapchainImageBaseHeader *GraphicsAPI_D3D11::AllocateSwapchainImageData(XrSwapchain swapchain, SwapchainType type, uint32_t count) {
    swapchainImagesMap[swapchain].first = type;
    swapchainImagesMap[swapchain].second.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImagesMap[swapchain].second.data());
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D11_AllocateSwapchainImageData

void *GraphicsAPI_D3D11::CreateImage(const ImageCreateInfo &imageCI) {
    if (imageCI.dimension == 1) {
        ID3D11Texture1D *texture = nullptr;
        D3D11_TEXTURE1D_DESC desc;
        desc.Width = imageCI.width;
        desc.MipLevels = imageCI.mipLevels;
        desc.ArraySize = imageCI.arrayLayers;
        desc.Format = (DXGI_FORMAT)imageCI.format;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = (imageCI.sampled ? D3D11_BIND_SHADER_RESOURCE : 0) | (imageCI.colorAttachment ? D3D11_BIND_RENDER_TARGET : 0) | (imageCI.depthAttachment ? D3D11_BIND_DEPTH_STENCIL : 0);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = imageCI.mipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
        D3D11_CHECK(device->CreateTexture1D(&desc, nullptr, &texture), "Failed to create Texture1D.");
        return texture;
    } else if (imageCI.dimension == 2) {
        ID3D11Texture2D *texture = nullptr;
        D3D11_TEXTURE2D_DESC desc;
        desc.Width = imageCI.width;
        desc.Height = imageCI.height;
        desc.MipLevels = imageCI.mipLevels;
        desc.ArraySize = imageCI.arrayLayers;
        desc.Format = (DXGI_FORMAT)imageCI.format;
        desc.SampleDesc = {imageCI.sampleCount, 0};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = (imageCI.sampled ? D3D11_BIND_SHADER_RESOURCE : 0) | (imageCI.colorAttachment ? D3D11_BIND_RENDER_TARGET : 0) | (imageCI.depthAttachment ? D3D11_BIND_DEPTH_STENCIL : 0);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = (imageCI.cubemap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0) | (imageCI.mipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);
        D3D11_CHECK(device->CreateTexture2D(&desc, nullptr, &texture), "Failed to create Texture2D.");
        return texture;
    } else if (imageCI.dimension == 3) {
        ID3D11Texture3D *texture = nullptr;
        D3D11_TEXTURE3D_DESC desc;
        desc.Width = imageCI.width;
        desc.Height = imageCI.height;
        desc.Depth = imageCI.depth;
        desc.MipLevels = imageCI.mipLevels;
        desc.Format = (DXGI_FORMAT)imageCI.format;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = (imageCI.sampled ? D3D11_BIND_SHADER_RESOURCE : 0) | (imageCI.colorAttachment ? D3D11_BIND_RENDER_TARGET : 0) | (imageCI.depthAttachment ? D3D11_BIND_DEPTH_STENCIL : 0);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = imageCI.mipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
        D3D11_CHECK(device->CreateTexture3D(&desc, nullptr, &texture), "Failed to create Texture3D.");
        return texture;
    } else {
        DEBUG_BREAK;
        std::cout << "ERROR: D3D11: Unknown Dimension for CreateImage: " << imageCI.dimension << std::endl;
        return nullptr;
    }
}

void GraphicsAPI_D3D11::DestroyImage(void *&image) {
    ID3D11Resource *d3d11Image = reinterpret_cast<ID3D11Resource *>(image);
    D3D11_SAFE_RELEASE(d3d11Image);
    image = nullptr;
}

void *GraphicsAPI_D3D11::CreateImageView(const ImageViewCreateInfo &imageViewCI) {
    if (imageViewCI.type == ImageViewCreateInfo::Type::RTV) {
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = (DXGI_FORMAT)imageViewCI.format;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
            rtvDesc.Texture1D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_3D: {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
            rtvDesc.Texture3D.MipSlice = imageViewCI.baseMipLevel;
            rtvDesc.Texture3D.FirstWSlice = imageViewCI.baseArrayLayer;
            rtvDesc.Texture3D.WSize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvDesc.Texture1DArray.MipSlice = imageViewCI.baseMipLevel;
            rtvDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            rtvDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.MipSlice = imageViewCI.baseMipLevel;
            rtvDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            rtvDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D11: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        ID3D11RenderTargetView *rtv = nullptr;
        D3D11_CHECK(device->CreateRenderTargetView((ID3D11Resource *)imageViewCI.image, &rtvDesc, &rtv), "Failed to create ImageView.")
        return rtv;
    } else if (imageViewCI.type == ImageViewCreateInfo::Type::DSV) {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        dsvDesc.Format = (DXGI_FORMAT)imageViewCI.format;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
            dsvDesc.Texture1D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
            dsvDesc.Texture1DArray.MipSlice = imageViewCI.baseMipLevel;
            dsvDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            dsvDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvDesc.Texture2DArray.MipSlice = imageViewCI.baseMipLevel;
            dsvDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            dsvDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D11: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        ID3D11DepthStencilView *dsv = nullptr;
        D3D11_CHECK(device->CreateDepthStencilView((ID3D11Resource *)imageViewCI.image, &dsvDesc, &dsv), "Failed to create ImageView.")
        return dsv;
    } else if (imageViewCI.type == ImageViewCreateInfo::Type::SRV) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = (DXGI_FORMAT)imageViewCI.format;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            srvDesc.Texture1D.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture1D.MipLevels = imageViewCI.levelCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture2D.MipLevels = imageViewCI.levelCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_3D: {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture3D.MipLevels = imageViewCI.levelCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            srvDesc.Texture1DArray.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture1DArray.MipLevels = imageViewCI.levelCount;
            srvDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            srvDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.MostDetailedMip = imageViewCI.baseMipLevel;
            srvDesc.Texture2DArray.MipLevels = imageViewCI.levelCount;
            srvDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            srvDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D11: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        ID3D11ShaderResourceView *srv = nullptr;
        D3D11_CHECK(device->CreateShaderResourceView((ID3D11Resource *)imageViewCI.image, &srvDesc, &srv), "Failed to create ImageView.")
        return srv;
    } else if (imageViewCI.type == ImageViewCreateInfo::Type::UAV) {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.Format = (DXGI_FORMAT)imageViewCI.format;

        switch (imageViewCI.view) {
        case ImageViewCreateInfo::View::TYPE_1D: {
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
            uavDesc.Texture1D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D: {
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = imageViewCI.baseMipLevel;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_3D: {
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
            uavDesc.Texture3D.MipSlice = imageViewCI.baseMipLevel;
            uavDesc.Texture3D.FirstWSlice = imageViewCI.baseArrayLayer;
            uavDesc.Texture3D.WSize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_1D_ARRAY: {
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
            uavDesc.Texture1DArray.MipSlice = imageViewCI.baseMipLevel;
            uavDesc.Texture1DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            uavDesc.Texture1DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        case ImageViewCreateInfo::View::TYPE_2D_ARRAY: {
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
            uavDesc.Texture2DArray.MipSlice = imageViewCI.baseMipLevel;
            uavDesc.Texture2DArray.FirstArraySlice = imageViewCI.baseArrayLayer;
            uavDesc.Texture2DArray.ArraySize = imageViewCI.layerCount;
            break;
        }
        default:
            DEBUG_BREAK;
            std::cout << "ERROR: D3D11: Unknown ImageView View." << std::endl;
            return nullptr;
        }
        ID3D11UnorderedAccessView *uav = nullptr;
        D3D11_CHECK(device->CreateUnorderedAccessView((ID3D11Resource *)imageViewCI.image, &uavDesc, &uav), "Failed to create ImageView.")
        return uav;
    } else {
        DEBUG_BREAK;
        std::cout << "ERROR: D3D11: Unknown ImageView Type." << std::endl;
        return nullptr;
    }
}

void GraphicsAPI_D3D11::DestroyImageView(void *&imageView) {
    ID3D11View *d3d11ImageView = (ID3D11View *)imageView;
    D3D11_SAFE_RELEASE(d3d11ImageView);
    imageView = nullptr;
}

void *GraphicsAPI_D3D11::CreateSampler(const SamplerCreateInfo &samplerCI) {
    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = ToD3D11Filter(samplerCI.minFilter);
    samplerDesc.AddressU = ToD3D11TextureAddressMode(samplerCI.addressModeR);
    samplerDesc.AddressV = ToD3D11TextureAddressMode(samplerCI.addressModeS);
    samplerDesc.AddressW = ToD3D11TextureAddressMode(samplerCI.addressModeT);
    samplerDesc.MipLODBias = samplerCI.mipLodBias;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.ComparisonFunc = ToD3D11Comparison(samplerCI.compareOp);
    samplerDesc.BorderColor[0] = samplerCI.borderColor[0];
    samplerDesc.BorderColor[1] = samplerCI.borderColor[0];
    samplerDesc.BorderColor[2] = samplerCI.borderColor[0];
    samplerDesc.BorderColor[3] = samplerCI.borderColor[0];
    samplerDesc.MinLOD = samplerCI.minLod;
    samplerDesc.MaxLOD = samplerCI.maxLod;

    ID3D11SamplerState *d3D11SamplerState = nullptr;
    D3D11_CHECK(device->CreateSamplerState(&samplerDesc, &d3D11SamplerState), "Failed to create Sampler");
    return d3D11SamplerState;
}

void GraphicsAPI_D3D11::DestroySampler(void *&sampler) {
    ID3D11SamplerState *d3D11SamplerState = reinterpret_cast<ID3D11SamplerState *>(sampler);
    D3D11_SAFE_RELEASE(d3D11SamplerState);
    sampler = nullptr;
}

void *GraphicsAPI_D3D11::CreateBuffer(const BufferCreateInfo &bufferCI) {
    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = bufferCI.data;
    initData.SysMemPitch = (UINT)bufferCI.stride;
    initData.SysMemSlicePitch = 0;
    bool cpu_access = true;
    //(bufferCI.type == GraphicsAPI::BufferCreateInfo::Type::UNIFORM);

    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = (UINT)(bufferCI.size);
    desc.Usage = cpu_access ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    desc.BindFlags = ToD3D11BindFlag(bufferCI.type);
    desc.CPUAccessFlags = (cpu_access ? D3D11_CPU_ACCESS_WRITE : (UINT)0);
    desc.MiscFlags = (UINT)0;
    desc.StructureByteStride = 0;

    ID3D11Buffer *d3D11Buffer = nullptr;
    D3D11_CHECK(device->CreateBuffer(&desc, bufferCI.data ? &initData : nullptr, &d3D11Buffer), "Failed to create Buffer");

    SetBufferData(d3D11Buffer, 0, bufferCI.size, bufferCI.data);
    buffers[d3D11Buffer] = bufferCI;

    return d3D11Buffer;
}

void GraphicsAPI_D3D11::DestroyBuffer(void *&buffer) {
    ID3D11Buffer *d3D11Buffer = reinterpret_cast<ID3D11Buffer *>(buffer);
    buffers.erase(d3D11Buffer);
    D3D11_SAFE_RELEASE(d3D11Buffer);
}

void *GraphicsAPI_D3D11::CreateShader(const ShaderCreateInfo &shaderCI) {
    std::vector<char> compiledBinary(shaderCI.sourceSize);
    memcpy(compiledBinary.data(), shaderCI.sourceData, shaderCI.sourceSize);

    switch (shaderCI.type) {
    case ShaderCreateInfo::Type::VERTEX: {
        ID3D11VertexShader *vertexShader = nullptr;
        D3D11_CHECK(device->CreateVertexShader(compiledBinary.data(), compiledBinary.size(), nullptr, &vertexShader), "Failed to create Vertex Shader.");
        shaderCompiledBinaries[vertexShader] = compiledBinary;
        return vertexShader;
    }
    case ShaderCreateInfo::Type::TESSELLATION_CONTROL: {
        ID3D11HullShader *hullShader = nullptr;
        D3D11_CHECK(device->CreateHullShader(compiledBinary.data(), compiledBinary.size(), nullptr, &hullShader), "Failed to create Hull Shader.");
        shaderCompiledBinaries[hullShader] = compiledBinary;
        return hullShader;
    }
    case ShaderCreateInfo::Type::TESSELLATION_EVALUATION: {
        ID3D11DomainShader *domainShader = nullptr;
        D3D11_CHECK(device->CreateDomainShader(compiledBinary.data(), compiledBinary.size(), nullptr, &domainShader), "Failed to create Domain Shader.");
        shaderCompiledBinaries[domainShader] = compiledBinary;
        return domainShader;
    }
    case ShaderCreateInfo::Type::GEOMETRY: {
        ID3D11GeometryShader *geometryShader = nullptr;
        D3D11_CHECK(device->CreateGeometryShader(compiledBinary.data(), compiledBinary.size(), nullptr, &geometryShader), "Failed to create Geometry Shader.");
        shaderCompiledBinaries[geometryShader] = compiledBinary;
        return geometryShader;
    }
    case ShaderCreateInfo::Type::FRAGMENT: {
        ID3D11PixelShader *pixelShader = nullptr;
        D3D11_CHECK(device->CreatePixelShader(compiledBinary.data(), compiledBinary.size(), nullptr, &pixelShader), "Failed to create Pixel Shader.");
        shaderCompiledBinaries[pixelShader] = compiledBinary;
        return pixelShader;
    }
    case ShaderCreateInfo::Type::COMPUTE: {
        ID3D11ComputeShader *computeShader = nullptr;
        D3D11_CHECK(device->CreateComputeShader(compiledBinary.data(), compiledBinary.size(), nullptr, &computeShader), "Failed to create Compute Shader.");
        shaderCompiledBinaries[computeShader] = compiledBinary;
        return computeShader;
    }
    default:
        std::cout << "ERROR: D3D11: Unknown Shader Type." << std::endl;
        return nullptr;
    }
}

void GraphicsAPI_D3D11::DestroyShader(void *&shader) {
    ID3D11DeviceChild *d3d11DeviceChild = reinterpret_cast<ID3D11DeviceChild *>(shader);
    D3D11_SAFE_RELEASE(d3d11DeviceChild);
    shader = nullptr;
}

void *GraphicsAPI_D3D11::CreatePipeline(const PipelineCreateInfo &pipelineCI) {
    static UINT64 pipelineID = 0;
    pipelineID++;
    pipelines[pipelineID] = pipelineCI;
    return (void *)pipelineID;
}

void GraphicsAPI_D3D11::DestroyPipeline(void *&pipeline) {
    pipelines.erase((UINT64)pipeline);
    pipeline = nullptr;
}

void GraphicsAPI_D3D11::BeginRendering() {
}

void GraphicsAPI_D3D11::EndRendering() {
}

void GraphicsAPI_D3D11::SetBufferData(void *buffer, size_t offset, size_t size, void *data) {
    ID3D11Buffer *d3d11Buffer = (ID3D11Buffer *)buffer;
    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    D3D11_CHECK(immediateContext->Map(d3d11Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource), "Failed to map Resource.");
    if (mappedSubresource.pData && data)
        memcpy((char *)mappedSubresource.pData + offset, data, size);
    immediateContext->Unmap(d3d11Buffer, 0);
}

void GraphicsAPI_D3D11::ClearColor(void *imageView, float r, float g, float b, float a) {
    const FLOAT clearColor[4] = {r, g, b, a};
    immediateContext->ClearRenderTargetView((ID3D11RenderTargetView *)imageView, clearColor);
}

void GraphicsAPI_D3D11::ClearDepth(void *imageView, float d) {
    immediateContext->ClearDepthStencilView((ID3D11DepthStencilView *)imageView, D3D11_CLEAR_DEPTH, d, 0);
}

void GraphicsAPI_D3D11::SetRenderAttachments(void **colorViews, size_t colorViewCount, void *depthStencilView, uint32_t width, uint32_t height, void *pipeline) {
    immediateContext->OMSetRenderTargets((UINT)colorViewCount, (ID3D11RenderTargetView *const *)colorViews, (ID3D11DepthStencilView *)depthStencilView);
}

void GraphicsAPI_D3D11::SetViewports(Viewport *viewports, size_t count) {
    std::vector<D3D11_VIEWPORT> d3d11Viewports;
    d3d11Viewports.reserve(count);
    for (size_t i = 0; i < count; i++) {
        Viewport viewport = viewports[i];
        d3d11Viewports.push_back({viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth});
    }
    immediateContext->RSSetViewports(static_cast<UINT>(d3d11Viewports.size()), d3d11Viewports.data());
}

void GraphicsAPI_D3D11::SetScissors(Rect2D *scissors, size_t count) {
    std::vector<D3D11_RECT> d3d11Scissors;
    d3d11Scissors.reserve(count);
    for (size_t i = 0; i < count; i++) {
        Rect2D scissor = scissors[i];
        d3d11Scissors.push_back({static_cast<LONG>(scissor.offset.x), static_cast<LONG>(scissor.offset.y), static_cast<LONG>(scissor.extent.width), static_cast<LONG>(scissor.extent.height)});
    }
    immediateContext->RSSetScissorRects(static_cast<UINT>(d3d11Scissors.size()), d3d11Scissors.data());
}

void GraphicsAPI_D3D11::SetPipeline(void *pipeline) {
    PipelineCreateInfo pipelineCI = pipelines[(UINT64)pipeline];
    setPipeline = (UINT64)pipeline;

    // Shaders
    for (void *shader : pipelineCI.shaders) {
        HRESULT res = S_OK;
        ID3D11DeviceChild *d3d11Shader = (ID3D11DeviceChild *)shader;

        ID3D11VertexShader *vertexShader = nullptr;
        res = d3d11Shader->QueryInterface<ID3D11VertexShader>(&vertexShader);
        if (res == S_OK && vertexShader) {
            immediateContext->VSSetShader(vertexShader, nullptr, 0);
            continue;
        }
        ID3D11HullShader *hullShader = nullptr;
        res = d3d11Shader->QueryInterface<ID3D11HullShader>(&hullShader);
        if (res == S_OK && hullShader) {
            immediateContext->HSSetShader(hullShader, nullptr, 0);
            continue;
        }
        ID3D11DomainShader *domainShader = nullptr;
        res = d3d11Shader->QueryInterface<ID3D11DomainShader>(&domainShader);
        if (res == S_OK && domainShader) {
            immediateContext->DSSetShader(domainShader, nullptr, 0);
            continue;
        }
        ID3D11GeometryShader *geometryShader = nullptr;
        res = d3d11Shader->QueryInterface<ID3D11GeometryShader>(&geometryShader);
        if (res == S_OK && geometryShader) {
            immediateContext->GSSetShader(geometryShader, nullptr, 0);
            continue;
        }
        ID3D11PixelShader *pixexShader = nullptr;
        res = d3d11Shader->QueryInterface<ID3D11PixelShader>(&pixexShader);
        if (res == S_OK && pixexShader) {
            immediateContext->PSSetShader(pixexShader, nullptr, 0);
            continue;
        }
        ID3D11ComputeShader *computeShader = nullptr;
        res = d3d11Shader->QueryInterface<ID3D11ComputeShader>(&computeShader);
        if (res == S_OK && computeShader) {
            immediateContext->CSSetShader(computeShader, nullptr, 0);
            continue;
        }
    }

    // Vertex Input
    ID3D11VertexShader *vertexShader = nullptr;
    immediateContext->VSGetShader(&vertexShader, nullptr, 0);
    if (vertexShader) {
        std::vector<char> vsCompiledBinary = shaderCompiledBinaries[vertexShader];

        std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
        for (const VertexInputAttribute &attribute : pipelineCI.vertexInputState.attributes) {
            D3D11_INPUT_ELEMENT_DESC element{};
            element.SemanticName = attribute.semanticName;
            element.SemanticIndex = attribute.attribIndex;
            element.Format = ToDXGI_FORMAT(attribute.vertexType);
            element.InputSlot = attribute.bindingIndex;
            element.AlignedByteOffset = (UINT)attribute.offset;
            element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            element.InstanceDataStepRate = 0;
            elements.push_back(element);
        }

        ID3D11InputLayout *inputLayout = nullptr;
        D3D11_CHECK(device->CreateInputLayout(elements.data(), (UINT)elements.size(), vsCompiledBinary.data(), vsCompiledBinary.size(), &inputLayout), "Failed to create InputLayout");
        immediateContext->IASetInputLayout(inputLayout);
        D3D11_SAFE_RELEASE(inputLayout);
    }

    // Input Assembly
    D3D11_PRIMITIVE_TOPOLOGY primitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    switch (pipelineCI.inputAssemblyState.topology) {
    case PrimitiveTopology::POINT_LIST:
        primitiveTopologyType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    case PrimitiveTopology::LINE_LIST:
        primitiveTopologyType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case PrimitiveTopology::LINE_STRIP:
        primitiveTopologyType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
    case PrimitiveTopology::TRIANGLE_LIST:
        primitiveTopologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case PrimitiveTopology::TRIANGLE_STRIP:
        primitiveTopologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    case PrimitiveTopology::TRIANGLE_FAN:
        primitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        break;
    default:
        primitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        break;
    }
    immediateContext->IASetPrimitiveTopology(primitiveTopologyType);

    // Rasterisation
    ID3D11RasterizerState *rasteriserState = nullptr;
    D3D11_RASTERIZER_DESC rasteriserStateDesc{};
    rasteriserStateDesc.FillMode = pipelineCI.rasterisationState.polygonMode == PolygonMode::LINE ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
    rasteriserStateDesc.CullMode = D3D11_CULL_MODE((uint32_t)pipelineCI.rasterisationState.cullMode + 1);
    rasteriserStateDesc.FrontCounterClockwise = pipelineCI.rasterisationState.frontFace == FrontFace::COUNTER_CLOCKWISE ? true : false;
    rasteriserStateDesc.DepthBias = pipelineCI.rasterisationState.depthBiasEnable;
    rasteriserStateDesc.DepthBiasClamp = pipelineCI.rasterisationState.depthBiasClamp;
    rasteriserStateDesc.SlopeScaledDepthBias = pipelineCI.rasterisationState.depthBiasSlopeFactor;
    rasteriserStateDesc.DepthClipEnable = pipelineCI.rasterisationState.depthClampEnable;
    rasteriserStateDesc.ScissorEnable = true;
    rasteriserStateDesc.MultisampleEnable = pipelineCI.multisampleState.rasterisationSamples > 1;
    rasteriserStateDesc.AntialiasedLineEnable = false;

    D3D11_CHECK(device->CreateRasterizerState(&rasteriserStateDesc, &rasteriserState), "Failed to create Rasterizer State.");
    immediateContext->RSSetState(rasteriserState);
    D3D11_SAFE_RELEASE(rasteriserState);

    // Depth Stencil
    ID3D11DepthStencilState *depthStencilState = nullptr;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = pipelineCI.depthStencilState.depthTestEnable;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK(pipelineCI.depthStencilState.depthWriteEnable);
    depthStencilDesc.DepthFunc = ToD3D11Comparison(pipelineCI.depthStencilState.depthCompareOp);
    depthStencilDesc.StencilEnable = pipelineCI.depthStencilState.stencilTestEnable;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = ToD3D11StencilOp(pipelineCI.depthStencilState.front.failOp);
    depthStencilDesc.FrontFace.StencilDepthFailOp = ToD3D11StencilOp(pipelineCI.depthStencilState.front.depthFailOp);
    depthStencilDesc.FrontFace.StencilPassOp = ToD3D11StencilOp(pipelineCI.depthStencilState.front.passOp);
    depthStencilDesc.FrontFace.StencilFunc = ToD3D11Comparison(pipelineCI.depthStencilState.front.compareOp);
    depthStencilDesc.BackFace.StencilFailOp = ToD3D11StencilOp(pipelineCI.depthStencilState.back.failOp);
    depthStencilDesc.BackFace.StencilDepthFailOp = ToD3D11StencilOp(pipelineCI.depthStencilState.back.depthFailOp);
    depthStencilDesc.BackFace.StencilPassOp = ToD3D11StencilOp(pipelineCI.depthStencilState.back.passOp);
    depthStencilDesc.BackFace.StencilFunc = ToD3D11Comparison(pipelineCI.depthStencilState.back.compareOp);
    D3D11_CHECK(device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState), "Failed to create Depth Stencil State.");
    immediateContext->OMSetDepthStencilState(depthStencilState, 0xFFFFFFFF);
    D3D11_SAFE_RELEASE(depthStencilState);

    // Color Blend
    ID3D11BlendState *blendState = nullptr;
    D3D11_BLEND_DESC blendDesc{};
    blendDesc.AlphaToCoverageEnable = pipelineCI.multisampleState.alphaToCoverageEnable;
    blendDesc.IndependentBlendEnable = true;
    size_t i = 0;
    for (auto &blend : pipelineCI.colorBlendState.attachments) {
		blendDesc.RenderTarget[i].RenderTargetWriteMask=0xFF;
        blendDesc.RenderTarget[i].BlendEnable = blend.blendEnable;
        blendDesc.RenderTarget[i].SrcBlend = ToD3D11_BLEND(blend.srcColorBlendFactor);
        blendDesc.RenderTarget[i].DestBlend = ToD3D11_BLEND(blend.dstColorBlendFactor);
        blendDesc.RenderTarget[i].BlendOp = static_cast<D3D11_BLEND_OP>(static_cast<uint32_t>(blend.colorBlendOp) + 1);
        blendDesc.RenderTarget[i].SrcBlendAlpha = ToD3D11_BLEND(blend.srcAlphaBlendFactor);
        blendDesc.RenderTarget[i].DestBlendAlpha = ToD3D11_BLEND(blend.dstAlphaBlendFactor);
        blendDesc.RenderTarget[i].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(static_cast<uint32_t>(blend.alphaBlendOp) + 1);
        blendDesc.RenderTarget[i].RenderTargetWriteMask = static_cast<UINT8>(blend.colorWriteMask);

        i++;
        if (i >= 8)
            break;
    }
    D3D11_CHECK(device->CreateBlendState(&blendDesc, &blendState), "Failed to create Color Blend State.");
    immediateContext->OMSetBlendState(blendState, pipelineCI.colorBlendState.blendConstants, pipelineCI.multisampleState.sampleMask);
    D3D11_SAFE_RELEASE(blendState);
}

void GraphicsAPI_D3D11::SetDescriptor(const DescriptorInfo &descriptorInfo) {
    ID3D11DeviceContext1 *immediateContext1 = nullptr;
    D3D11_CHECK(immediateContext->QueryInterface(IID_PPV_ARGS(&immediateContext1)), "Failed to get ID3D11DeviceContext1 * from Immediate Context.");

    UINT slot = descriptorInfo.bindingIndex;
    UINT firstConstant = Align<UINT>(descriptorInfo.bufferOffset / 16, 16);
    UINT numConstants = Align<UINT>(descriptorInfo.bufferSize / 16, 16);
    switch (descriptorInfo.stage) {
    case DescriptorInfo::Stage::VERTEX: {
        if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
            immediateContext1->VSSetConstantBuffers1(slot, 1, (ID3D11Buffer *const *)&descriptorInfo.resource, &firstConstant, &numConstants);
        } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
            immediateContext1->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView *const *)&descriptorInfo.resource);
        } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
            immediateContext1->VSSetSamplers(slot, 1, (ID3D11SamplerState *const *)&descriptorInfo.resource);
        } else {
            std::cout << "ERROR: D3D11: Unknown Descriptor Type." << std::endl;
        }
        break;
    }
    case DescriptorInfo::Stage::TESSELLATION_CONTROL: {
        if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
            immediateContext1->HSSetConstantBuffers1(slot, 1, (ID3D11Buffer *const *)&descriptorInfo.resource, &firstConstant, &numConstants);
        } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
            immediateContext1->HSSetShaderResources(slot, 1, (ID3D11ShaderResourceView *const *)&descriptorInfo.resource);
        } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
            immediateContext1->HSSetSamplers(slot, 1, (ID3D11SamplerState *const *)&descriptorInfo.resource);
        } else {
            std::cout << "ERROR: D3D11: Unknown Descriptor Type." << std::endl;
        }
        break;
    }
    case DescriptorInfo::Stage::TESSELLATION_EVALUATION: {
        if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
            immediateContext1->DSSetConstantBuffers1(slot, 1, (ID3D11Buffer *const *)&descriptorInfo.resource, &firstConstant, &numConstants);
        } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
            immediateContext1->DSSetShaderResources(slot, 1, (ID3D11ShaderResourceView *const *)&descriptorInfo.resource);
        } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
            immediateContext1->DSSetSamplers(slot, 1, (ID3D11SamplerState *const *)&descriptorInfo.resource);
        } else {
            std::cout << "ERROR: D3D11: Unknown Descriptor Type." << std::endl;
        }
        break;
    }
    case DescriptorInfo::Stage::GEOMETRY: {
        if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
            immediateContext1->GSSetConstantBuffers1(slot, 1, (ID3D11Buffer *const *)&descriptorInfo.resource, &firstConstant, &numConstants);
        } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
            immediateContext1->GSSetShaderResources(slot, 1, (ID3D11ShaderResourceView *const *)&descriptorInfo.resource);
        } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
            immediateContext1->GSSetSamplers(slot, 1, (ID3D11SamplerState *const *)&descriptorInfo.resource);
        } else {
            std::cout << "ERROR: D3D11: Unknown Descriptor Type." << std::endl;
        }
        break;
    }
    case DescriptorInfo::Stage::FRAGMENT: {
        if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
            immediateContext1->PSSetConstantBuffers1(slot, 1, (ID3D11Buffer *const *)&descriptorInfo.resource, &firstConstant, &numConstants);
        } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
            immediateContext1->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView *const *)&descriptorInfo.resource);
        } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
            immediateContext1->PSSetSamplers(slot, 1, (ID3D11SamplerState *const *)&descriptorInfo.resource);
        } else {
            std::cout << "ERROR: D3D11: Unknown Descriptor Type." << std::endl;
        }
        break;
    }
    case DescriptorInfo::Stage::COMPUTE: {
        if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
            if (descriptorInfo.readWrite) {
                // UAVs?
            } else {
                immediateContext1->CSSetConstantBuffers1(slot, 1, (ID3D11Buffer *const *)&descriptorInfo.resource, &firstConstant, &numConstants);
            }
        } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
            if (descriptorInfo.readWrite) {
                immediateContext1->CSSetUnorderedAccessViews(slot, 1, (ID3D11UnorderedAccessView *const *)&descriptorInfo.resource, nullptr);
            } else {
                immediateContext1->CSSetShaderResources(slot, 1, (ID3D11ShaderResourceView *const *)&descriptorInfo.resource);
            }
        } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
            immediateContext1->CSSetSamplers(slot, 1, (ID3D11SamplerState *const *)&descriptorInfo.resource);
        } else {
            std::cout << "ERROR: D3D11: Unknown Descriptor Type." << std::endl;
        }
        break;
    }
    default:
        break;
    }

    D3D11_SAFE_RELEASE(immediateContext1);
}

void GraphicsAPI_D3D11::UpdateDescriptors() {
}

void GraphicsAPI_D3D11::SetVertexBuffers(void **vertexBuffers, size_t count) {
    const VertexInputState &vertexInputState = pipelines[setPipeline].vertexInputState;
    std::vector<UINT> strides;
    std::vector<UINT> offsets;
    for (size_t i = 0; i < count; i++) {
        for (const VertexInputBinding &vertexBinding : vertexInputState.bindings) {
            if (vertexBinding.bindingIndex == (uint32_t)i) {
                strides.push_back((UINT)vertexBinding.stride);
                offsets.push_back(0);
            }
        }
    }
    immediateContext->IASetVertexBuffers(0, (UINT)count, (ID3D11Buffer *const *)vertexBuffers, strides.data(), offsets.data());
}

void GraphicsAPI_D3D11::SetIndexBuffer(void *indexBuffer) {
    ID3D11Buffer *d3d11IndexBuffer = (ID3D11Buffer *)indexBuffer;
    const BufferCreateInfo &bufferCI = buffers[d3d11IndexBuffer];
    immediateContext->IASetIndexBuffer(d3d11IndexBuffer, bufferCI.stride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
}

void GraphicsAPI_D3D11::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    immediateContext->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void GraphicsAPI_D3D11::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    immediateContext->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_GetSupportedSwapchainFormats
const std::vector<int64_t> GraphicsAPI_D3D11::GetSupportedColorSwapchainFormats() {
    return {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB};
}
const std::vector<int64_t> GraphicsAPI_D3D11::GetSupportedDepthSwapchainFormats() {
    return {
        DXGI_FORMAT_D32_FLOAT,
        DXGI_FORMAT_D16_UNORM};
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D11_GetSupportedSwapchainFormats
#endif
