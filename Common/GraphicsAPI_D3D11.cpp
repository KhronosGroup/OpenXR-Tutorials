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

static D3D11_TEXTURE_ADDRESS_MODE toD3d11TextureAddressMode(GraphicsAPI::SamplerCreateInfo::AddressMode f)
{
	if(f==GraphicsAPI::SamplerCreateInfo::AddressMode::CLAMP_TO_BORDER)
	if(f==GraphicsAPI::SamplerCreateInfo::AddressMode::CLAMP_TO_EDGE)
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	if(f==GraphicsAPI::SamplerCreateInfo::AddressMode::REPEAT)
		return D3D11_TEXTURE_ADDRESS_WRAP;
	if(f==GraphicsAPI::SamplerCreateInfo::AddressMode::MIRROR_CLAMP_TO_EDGE)
	if(f==GraphicsAPI::SamplerCreateInfo::AddressMode::MIRRORED_REPEAT)
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	return D3D11_TEXTURE_ADDRESS_WRAP;
}

static D3D11_FILTER toD3d11Filter(GraphicsAPI::SamplerCreateInfo::Filter f)
{
	if(f==GraphicsAPI::SamplerCreateInfo::Filter::LINEAR)
		return D3D11_FILTER_ANISOTROPIC;
	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

D3D11_COMPARISON_FUNC toD3dComparison(GraphicsAPI::CompareOp d)
{
	switch(d)
	{
	case GraphicsAPI::CompareOp::ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	case GraphicsAPI::CompareOp::LESS:
		return D3D11_COMPARISON_LESS;
	case GraphicsAPI::CompareOp::EQUAL:
		return D3D11_COMPARISON_EQUAL;
	case GraphicsAPI::CompareOp::LESS_OR_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
	case GraphicsAPI::CompareOp::GREATER:
		return D3D11_COMPARISON_GREATER;
	case GraphicsAPI::CompareOp::NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
	case GraphicsAPI::CompareOp::GREATER_OR_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
	default:
		break;
	};
	return D3D11_COMPARISON_LESS;
}

void* GraphicsAPI_D3D11::CreateSampler(const SamplerCreateInfo& samplerCI)
{	
	D3D11_SAMPLER_DESC samplerDesc;
	
    ZeroMemory( &samplerDesc, sizeof( D3D11_SAMPLER_DESC ) );
    samplerDesc.Filter = toD3d11Filter (samplerCI.minFilter) ;
    samplerDesc.AddressU = toD3d11TextureAddressMode(samplerCI.addressModeR);
    samplerDesc.AddressV = toD3d11TextureAddressMode(samplerCI.addressModeS);
    samplerDesc.AddressW = toD3d11TextureAddressMode(samplerCI.addressModeT);
    samplerDesc.ComparisonFunc = toD3dComparison(samplerCI.compareOp);
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	ID3D11SamplerState *d3D11SamplerState=nullptr;
	device->CreateSamplerState(&samplerDesc,&d3D11SamplerState);
	return d3D11SamplerState;
	
}

void GraphicsAPI_D3D11::DestroySampler(void*& sampler) 
{
	ID3D11SamplerState *d3D11SamplerState=reinterpret_cast<ID3D11SamplerState *>(sampler);
	D3D11_SAFE_RELEASE(d3D11SamplerState);
	sampler=nullptr;
}
D3D11_BIND_FLAG ToD3D11BindFlag(GraphicsAPI::BufferCreateInfo::Type t)
{
	switch(t)
	{
	case GraphicsAPI::BufferCreateInfo::Type::VERTEX:
	return D3D11_BIND_VERTEX_BUFFER;
	case GraphicsAPI::BufferCreateInfo::Type::INDEX:
	return D3D11_BIND_INDEX_BUFFER;
	case GraphicsAPI::BufferCreateInfo::Type::UNIFORM:
	default:
	return D3D11_BIND_CONSTANT_BUFFER;
	};
}

void* GraphicsAPI_D3D11::CreateBuffer(const BufferCreateInfo& bufferCI) 
{
    D3D11_SUBRESOURCE_DATA InitData;
    memset( &InitData,0,sizeof(D3D11_SUBRESOURCE_DATA) );
    InitData.pSysMem		=bufferCI.data;
	InitData.SysMemPitch	=bufferCI.stride;
	InitData.SysMemSlicePitch	= 0;
	bool cpu_access=(bufferCI.type==GraphicsAPI::BufferCreateInfo::Type::UNIFORM);
	D3D11_BUFFER_DESC desc=
	{
		(unsigned)(bufferCI.size)
		,cpu_access?D3D11_USAGE_DYNAMIC:D3D11_USAGE_DEFAULT
		,ToD3D11BindFlag(bufferCI.type)
		,(cpu_access?D3D11_CPU_ACCESS_WRITE: (unsigned)0),(unsigned)0
	};
	ID3D11Buffer *d3D11Buffer=nullptr;
	device->CreateBuffer(&desc,bufferCI.data?&InitData:NULL,&d3D11Buffer);
	return d3D11Buffer;
}

void GraphicsAPI_D3D11::DestroyBuffer(void*& buffer) 
{
	ID3D11SamplerState *d3D11Buffer=reinterpret_cast<ID3D11SamplerState *>(buffer);
	D3D11_SAFE_RELEASE(d3D11Buffer);
}

void* GraphicsAPI_D3D11::CreateShader(const ShaderCreateInfo& shaderCI) 
{
    switch (shaderCI.type) {
    case ShaderCreateInfo::Type::VERTEX: {
		ID3D11VertexShader* vertexShader = nullptr;
		device->CreateVertexShader(shaderCI.sourceData, shaderCI.sourceSize, NULL, &vertexShader);
		return vertexShader;
        break;
    }
    case ShaderCreateInfo::Type::FRAGMENT: {
		ID3D11PixelShader* pixelShader=nullptr;
		device->CreatePixelShader(shaderCI.sourceData, shaderCI.sourceSize, NULL, &pixelShader);
		return pixelShader;
        break;
    }
    case ShaderCreateInfo::Type::COMPUTE: {
		ID3D11ComputeShader* computeShader= nullptr;
		device->CreateComputeShader(shaderCI.sourceData, shaderCI.sourceSize, NULL, &computeShader);
		return computeShader;
        break;
    }
    default:
        std::cout << "ERROR:  Unknown Shader Type." << std::endl;
		return nullptr;
    }
}

void GraphicsAPI_D3D11::DestroyShader(void*& shader) 
{
	ID3D11DeviceChild *d3d11DeviceChild=reinterpret_cast<ID3D11DeviceChild*>(shader);
	D3D11_SAFE_RELEASE(d3d11DeviceChild);
	shader=nullptr;
}

void* GraphicsAPI_D3D11::CreatePipeline(const PipelineCreateInfo& pipelineCI) 
{
	return nullptr;
}

void GraphicsAPI_D3D11::DestroyPipeline(void*& pipeline) 
{

}


void GraphicsAPI_D3D11::BeginRendering()
{
}

void GraphicsAPI_D3D11::EndRendering()
{
}


void GraphicsAPI_D3D11::ClearColor(void *imageView, float r, float g, float b, float a) {
    const FLOAT clearColor[4] = {r, g, b, a};
    immediateContext->ClearRenderTargetView((ID3D11RenderTargetView *)imageView, clearColor);
}

void GraphicsAPI_D3D11::ClearDepth(void *imageView, float d) {
    immediateContext->ClearDepthStencilView((ID3D11DepthStencilView *)imageView, D3D11_CLEAR_DEPTH, d, 0);
}

void GraphicsAPI_D3D11::SetPipeline(void* pipeline) 
{

}

void GraphicsAPI_D3D11::SetDescriptor(const DescriptorInfo& descriptorInfo) 
{

}

void GraphicsAPI_D3D11::SetVertexBuffers(void** vertexBuffers, size_t count) 
{

}

void GraphicsAPI_D3D11::SetIndexBuffer(void* indexBuffer) 
{

}

void GraphicsAPI_D3D11::DrawIndexed(uint32_t indexCount, uint32_t instanceCount , uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) 
{

}

void GraphicsAPI_D3D11::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) 
{

}


// XR_DOCS_TAG_BEGIN_GraphicsAPI_D3D11_GetSupportedSwapchainFormats
const std::vector<int64_t> GraphicsAPI_D3D11::GetSupportedSwapchainFormats() {
    return {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB};
}
// XR_DOCS_TAG_END_GraphicsAPI_D3D11_GetSupportedSwapchainFormats
#endif