#include "GraphicsAPI_D3D11.h"

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

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11
GraphicsAPI_D3D11::GraphicsAPI_D3D11(XrInstance xrInstance, XrSystemId systemId) {
    OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction *)&xrGetD3D11GraphicsRequirementsKHR), "Failed to get InstanceProcAddr.");
    XrGraphicsRequirementsD3D11KHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
    OPENXR_CHECK(xrGetD3D11GraphicsRequirementsKHR(xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for D3D11.");

    D3D11_CHECK(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

    UINT i = 0;
    IDXGIAdapter *adapter = nullptr;
    DXGI_ADAPTER_DESC adapterDesc = {};
    while (factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
        adapter->GetDesc(&adapterDesc);
        if (adapterDesc.AdapterLuid.LowPart == graphicsRequirements.adapterLuid.LowPart && adapterDesc.AdapterLuid.HighPart == graphicsRequirements.adapterLuid.HighPart) {
            break;  // We have the matching adapter that OpenXR wants.
        }
    }

    D3D11_CHECK(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, 0, 0, &graphicsRequirements.minFeatureLevel, 1, D3D11_SDK_VERSION, &device, nullptr, &immediateContext), "Failed to create D3D11 Device.");
}

GraphicsAPI_D3D11::~GraphicsAPI_D3D11() {
    D3D11_SAFE_RELEASE(immediateContext);
    D3D11_SAFE_RELEASE(device);
    D3D11_SAFE_RELEASE(factory);
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D11

// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_GetGraphicsBinding
void *GraphicsAPI_D3D11::GetGraphicsBinding() {
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
    graphicsBinding.device = device;
    return &graphicsBinding;
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D11_GetGraphicsBinding

XrSwapchainImageBaseHeader *GraphicsAPI_D3D11::AllocateSwapchainImageData(uint32_t count) {
    swapchainImages.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImages.data());
}

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

void GraphicsAPI_D3D11::ClearColor(void *imageView, float r, float g, float b, float a) {
    const FLOAT clearColor[4] = {r, g, b, a};
    immediateContext->ClearRenderTargetView((ID3D11RenderTargetView *)imageView, clearColor);
}

void GraphicsAPI_D3D11::ClearDepth(void *imageView, float d) {
    immediateContext->ClearDepthStencilView((ID3D11DepthStencilView *)imageView, D3D11_CLEAR_DEPTH, d, 0);
}

const std::vector<int64_t> GraphicsAPI_D3D11::GetSupportedSwapchainFormats() {
    return {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB};
}
#endif