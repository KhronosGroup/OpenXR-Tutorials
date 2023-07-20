#pragma once
#include "HelperFunctions.h"

// Platform headers
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "unknwn.h"
#define XR_USE_PLATFORM_WIN32

#define XR_USE_GRAPHICS_API_D3D11
#define XR_USE_GRAPHICS_API_D3D12
#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_GRAPHICS_API_VULKAN
#endif

#if defined(__LINIX__)
#define XR_USE_PLATFORM_XLIB
#define XR_USE_PLATFORM_XCB
#define XR_USE_PLATFORM_WAYLAND

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_GRAPHICS_API_VULKAN
#endif

#if defined(__ANDROID__)
#include "android_native_app_glue.h"
#define XR_USE_PLATFORM_ANDROID

#define XR_USE_GRAPHICS_API_OPENGL_ES
#define XR_USE_GRAPHICS_API_VULKAN
#endif

// Graphic APIs headers
#if defined(XR_USE_GRAPHICS_API_D3D11)
#include <d3d11.h>
#endif

#if defined(XR_USE_GRAPHICS_API_D3D12)
#include <d3d12.h>
#include <dxgi1_6.h>
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL)
// gfxwrapper will redefine these macros
#undef XR_USE_PLATFORM_WIN32
#undef XR_USE_PLATFORM_XLIB
#undef XR_USE_PLATFORM_XCB
#undef XR_USE_PLATFORM_WAYLAND
#include "gfxwrapper_opengl.h"
#endif

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
#include "gfxwrapper_opengl.h"
#endif

#if defined(XR_USE_GRAPHICS_API_VULKAN)
#include "vulkan/vulkan.h"
#endif

// OpenXR
#include "OpenXRHelper.h"

enum GraphicsAPI_Type : uint8_t {
    UNKNOWN,
    D3D11,
    D3D12,
    OPENGL,
    OPENGL_ES,
    VULKAN
};

bool CheckGraphicsAPI_TypeIsValidForPlatform(GraphicsAPI_Type type);

const char* GetGraphicsAPIInstanceExtensionString(GraphicsAPI_Type type);


class GraphicsAPI {
public:
    // Pipeline Helpers
    #pragma region Pipeline Helpers
    enum class VertexType : uint8_t {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        INT,
        IVEC2,
        IVEC3,
        IVEC4,
        UINT,
        UVEC2,
        UVEC3,
        UVEC4
    };
    enum class PrimitiveTopology : uint8_t {
        POINT_LIST = 0,
        LINE_LIST = 1,
        LINE_STRIP = 2,
        TRIANGLE_LIST = 3,
        TRIANGLE_STRIP = 4,
        TRIANGLE_FAN = 5,
    };
    enum class PolygonMode : uint8_t {
        FILL = 0,
        LINE = 1,
        POINT = 2,
    };
    enum class CullMode : uint8_t {
        NONE = 0,
        FRONT = 1,
        BACK = 2,
        FRONT_AND_BACK = 3
    };
    enum class FrontFace : uint8_t {
        COUNTER_CLOCKWISE = 0,
        CLOCKWISE = 1,
    };
    enum class CompareOp : uint8_t {
        NEVER = 0,
        LESS = 1,
        EQUAL = 2,
        LESS_OR_EQUAL = 3,
        GREATER = 4,
        NOT_EQUAL = 5,
        GREATER_OR_EQUAL = 6,
        ALWAYS = 7,
    };
    enum class StencilOp : uint8_t {
        KEEP = 0,
        ZERO = 1,
        REPLACE = 2,
        INCREMENT_AND_CLAMP = 3,
        DECREMENT_AND_CLAMP = 4,
        INVERT = 5,
        INCREMENT_AND_WRAP = 6,
        DECREMENT_AND_WRAP = 7
    };
    struct StencilOpState {
        StencilOp failOp;
        StencilOp passOp;
        StencilOp depthFailOp;
        CompareOp compareOp;
        uint32_t compareMask;
        uint32_t writeMask;
        uint32_t reference;
    };
    enum class BlendFactor : uint8_t {
        ZERO = 0,
        ONE = 1,
        SRC_COLOUR = 2,
        ONE_MINUS_SRC_COLOUR = 3,
        DST_COLOUR = 4,
        ONE_MINUS_DST_COLOUR = 5,
        SRC_ALPHA = 6,
        ONE_MINUS_SRC_ALPHA = 7,
        DST_ALPHA = 8,
        ONE_MINUS_DST_ALPHA = 9,
    };
    enum class BlendOp : uint8_t {
        ADD = 0,
        SUBTRACT = 1,
        REVERSE_SUBTRACT = 2,
        MIN = 3,
        MAX = 4,
    };
    enum class ColourComponentBit : uint8_t {
        R_BIT = 0x00000001,
        G_BIT = 0x00000002,
        B_BIT = 0x00000004,
        A_BIT = 0x00000008,
    };
    struct ColourBlendAttachmentState {
        bool blendEnable;
        BlendFactor srcColourBlendFactor;
        BlendFactor dstColourBlendFactor;
        BlendOp colourBlendOp;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor dstAlphaBlendFactor;
        BlendOp alphaBlendOp;
        ColourComponentBit colourWriteMask;
    };
    enum class LogicOp : uint8_t {
        CLEAR = 0,
        AND = 1,
        AND_REVERSE = 2,
        COPY = 3,
        AND_INVERTED = 4,
        NO_OP = 5,
        XOR = 6,
        OR = 7,
        NOR = 8,
        EQUIVALENT = 9,
        INVERT = 10,
        OR_REVERSE = 11,
        COPY_INVERTED = 12,
        OR_INVERTED = 13,
        NAND = 14,
        SET = 15
    };
    #pragma endregion

    struct ShaderCreateInfo {
        enum class Type : uint8_t {
            VERTEX,
            TESSELLATION_CONTROL,
            TESSELLATION_EVALUATION,
            GEOMETRY,
            FRAGMENT,
            COMPUTE
        } type;
        const char* sourceData;
        size_t sourceSize;
    };
    struct VertexInputAttribute {
        uint32_t attribIndex;   // layout(location = X)
        uint32_t bindingIndex;  // Which buffer to use when bound for draws.
        VertexType vertexType;
        size_t offset;
        const char* semanticName;
    };
    typedef std::vector<VertexInputAttribute> VertexInputAttributes;
    struct VertexInputBinding {
        uint32_t bindingIndex;  // Which buffer to use when bound for draws.
        size_t offset;
        size_t stride;
    };
    typedef std::vector<VertexInputBinding> VertexInputBindings;
    struct VertexInputState {
        VertexInputAttributes attributes;
        VertexInputBindings bindings;
    };
    struct InputAssemblyState {
        PrimitiveTopology topology;
        bool primitiveRestartEnable;
    };
    struct RasterisationState {
        bool depthClampEnable;
        bool rasteriserDiscardEnable;
        PolygonMode polygonMode;
        CullMode cullMode;
        FrontFace frontFace;
        bool depthBiasEnable;
        float depthBiasConstantFactor;
        float depthBiasClamp;
        float depthBiasSlopeFactor;
        float lineWidth;
    };
    struct MultisampleState {
        uint32_t rasterisationSamples;
        bool sampleShadingEnable;
        float minSampleShading;
        uint32_t sampleMask;
        bool alphaToCoverageEnable;
        bool alphaToOneEnable;
    };
    struct DepthStencilState {
        bool depthTestEnable;
        bool depthWriteEnable;
        CompareOp depthCompareOp;
        bool depthBoundsTestEnable;
        bool stencilTestEnable;
        StencilOpState front;
        StencilOpState back;
        float minDepthBounds;
        float maxDepthBounds;
    };
    struct ColourBlendState {
        bool logicOpEnable;
        LogicOp logicOp;
        std::vector<ColourBlendAttachmentState> attachments;
        float blendConstants[4];
    };
    struct PipelineCreateInfo {
        std::vector<void*> shaders;
        VertexInputState vertexInputState;
        InputAssemblyState inputAssemblyState;
        RasterisationState rasterisationState;
        MultisampleState multisampleState;
        DepthStencilState depthStencilState;
        ColourBlendState colourBlendState;
    };

    struct BufferCreateInfo {
        enum class Type : uint8_t {
            VERTEX,
            INDEX,
            UNIFORM,
        } type;
		size_t stride;
        size_t size;
        void* data;
        bool indexBufferUint16; //Otherwise uint32
    };

    struct ImageCreateInfo {
        uint32_t dimension;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        uint32_t sampleCount;
        int64_t format;
        bool cubemap;
        bool colorAttachment;
        bool depthAttachment;
        bool sampled;
    };

    struct ImageViewCreateInfo {
        void* image;
        enum class Type : uint8_t {
            RTV,
            DSV,
            SRV,
            UAV
        } type;
        enum class View : uint8_t {
            TYPE_1D,
            TYPE_2D,
            TYPE_3D,
            TYPE_CUBE,
            TYPE_1D_ARRAY,
            TYPE_2D_ARRAY,
            TYPE_CUBE_ARRAY,
        } view;
        int64_t format;
        enum class Aspect : uint8_t {
            COLOR_BIT = 0x01,
            DEPTH_BIT = 0x02,
            STENCIL_BIT = 0x04
        } aspect;
        uint32_t baseMipLevel;
        uint32_t levelCount;
        uint32_t baseArrayLayer;
        uint32_t layerCount;
    };

    struct SamplerCreateInfo {
        enum class Filter :uint8_t {
            NEAREST,
            LINEAR
        } magFilter, minFilter;
        enum class MipmapMode : uint8_t {
            NEAREST,
            LINEAR,
            NOOP
        } mipmapMode;
        enum class AddressMode : uint8_t {
            REPEAT,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER,
            MIRROR_CLAMP_TO_EDGE
        } addressModeS, addressModeT, addressModeR;
        float mipLodBias;
        bool compareEnable;
        CompareOp compareOp;
        float minLod;
        float maxLod;
        float borderColour[4];
    };

    struct DescriptorInfo {
        uint32_t bindingIndex;
        void* resource;
        enum class Type : uint8_t {
            BUFFER,
            IMAGE,
            SAMPLER
        } type;
    };

public:
    virtual ~GraphicsAPI() = default;

    int64_t SelectSwapchainFormat(const std::vector<int64_t>& formats);

    virtual int64_t GetDepthFormat() = 0;

    virtual void* GetGraphicsBinding() = 0;
    virtual XrSwapchainImageBaseHeader* AllocateSwapchainImageData(uint32_t count) = 0;
    virtual XrSwapchainImageBaseHeader* GetSwapchainImageData(uint32_t index) = 0;
    virtual void* GetSwapchainImage(uint32_t index) = 0;

    virtual void* CreateImage(const ImageCreateInfo& imageCI) = 0;
    virtual void DestroyImage(void*& image) = 0;

    virtual void* CreateImageView(const ImageViewCreateInfo& imageViewCI) = 0;
    virtual void DestroyImageView(void*& imageView) = 0;
    
    // TODO: Make pure virtual
    virtual void* CreateSampler(const SamplerCreateInfo& samplerCI) { return nullptr; };
    virtual void DestroySampler(void*& sampler){};

    // TODO: Make pure virtual
    virtual void* CreateBuffer(const BufferCreateInfo& bufferCI) { return nullptr; }
    virtual void DestroyBuffer(void*& buffer) {}

    // TODO: Make pure virtual
    virtual void* CreateShader(const ShaderCreateInfo& shaderCI) { return nullptr; }
    virtual void DestroyShader(void*& shader) {}

    // TODO: Make pure virtual
    virtual void* CreatePipeline(const PipelineCreateInfo& pipelineCI) { return nullptr; }
    virtual void DestroyPipeline(void*& pipeline) {}

    virtual void BeginRendering(){};
    virtual void EndRendering(){};

    virtual void ClearColor(void* imageView, float r, float g, float b, float a) = 0;
    virtual void ClearDepth(void* imageView, float d) = 0;

    // TODO: Make pure virtual
    virtual void SetRenderAttachments(void** colorViews, size_t colorViewCount, void* depthStencilView) {}

    // TODO: Make pure virtual
    virtual void SetPipeline(void* pipeline) {}
    virtual void SetDescriptor(const DescriptorInfo& descriptorInfo) {}
    virtual void SetVertexBuffers(void** vertexBuffers, size_t count) {}
    virtual void SetIndexBuffer(void* indexBuffer) {}
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) {}
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) {}

protected:
    virtual const std::vector<int64_t> GetSupportedSwapchainFormats() = 0;
};