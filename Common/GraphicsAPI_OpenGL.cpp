#include "GraphicsAPI_OpenGL.h"

#if defined(XR_USE_GRAPHICS_API_OPENGL)

#if defined(OS_WINDOWS)
PROC GetExtension(const char *functionName) { return wglGetProcAddress(functionName); }
#elif defined(OS_APPLE)
void (*GetExtension(const char *functionName))() { return NULL; }
#elif defined(OS_LINUX_XCB) || defined(OS_LINUX_XLIB) || defined(OS_LINUX_XCB_GLX)
void (*GetExtension(const char *functionName))() { return glXGetProcAddress((const GLubyte *)functionName); }
#elif defined(OS_ANDROID) || defined(OS_LINUX_WAYLAND)
void (*GetExtension(const char *functionName))() { return eglGetProcAddress(functionName); }
#endif

#pragma region PiplineHelpers

GLenum GetGLTextureTarget(const GraphicsAPI::ImageCreateInfo& imageCI) {
    GLenum target = 0;
    if (imageCI.dimension == 1) {
        if (imageCI.arrayLayers > 1) {
            target = GL_TEXTURE_1D_ARRAY;
        } else {
            target = GL_TEXTURE_1D;
        }
    } else if (imageCI.dimension == 2) {
        if (imageCI.cubemap) {
            if (imageCI.arrayLayers > 6) {
                target = GL_TEXTURE_CUBE_MAP_ARRAY;
            } else {
                target = GL_TEXTURE_CUBE_MAP;
            }
        } else {
            if (imageCI.sampleCount > 1) {
                if (imageCI.arrayLayers > 1) {
                    target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
                } else {
                    target = GL_TEXTURE_2D_MULTISAMPLE;
                }
            } else {
                if (imageCI.arrayLayers > 1) {
                    target = GL_TEXTURE_2D_ARRAY;
                } else {
                    target = GL_TEXTURE_2D;
                }
            }
        }
    } else if (imageCI.dimension == 3) {
        target = GL_TEXTURE_3D;
    } else {
        DEBUG_BREAK;
        std::cout << "ERROR: OPENGL: Unknown Dimension for GetGLTextureTarget(): " << imageCI.dimension << std::endl;
    }
    return target;
}

GLint ToGLFilter(GraphicsAPI::SamplerCreateInfo::Filter filter) {
    switch (filter) {
    case GraphicsAPI::SamplerCreateInfo::Filter::NEAREST:
        return GL_NEAREST;
    case GraphicsAPI::SamplerCreateInfo::Filter::LINEAR:
        return GL_LINEAR;
    default:
        return 0;
    }
};
GLint ToGLFilterMipmap(GraphicsAPI::SamplerCreateInfo::Filter filter, GraphicsAPI::SamplerCreateInfo::MipmapMode mipmapMode) {
    switch (filter) {
    case GraphicsAPI::SamplerCreateInfo::Filter::NEAREST: {
        if (mipmapMode == GraphicsAPI::SamplerCreateInfo::MipmapMode::NEAREST)
            return GL_NEAREST_MIPMAP_LINEAR;
        else if (mipmapMode == GraphicsAPI::SamplerCreateInfo::MipmapMode::LINEAR)
            return GL_NEAREST_MIPMAP_NEAREST;
        else
            return GL_NEAREST;
    }
    case GraphicsAPI::SamplerCreateInfo::Filter::LINEAR: {
        if (mipmapMode == GraphicsAPI::SamplerCreateInfo::MipmapMode::NEAREST)
            return GL_LINEAR_MIPMAP_LINEAR;
        else if (mipmapMode == GraphicsAPI::SamplerCreateInfo::MipmapMode::LINEAR)
            return GL_LINEAR_MIPMAP_NEAREST;
        else
            return GL_LINEAR;
    }
    default:
        return 0;
    }
};
GLint ToGLAddressMode(GraphicsAPI::SamplerCreateInfo::AddressMode mode) {
    switch (mode) {
    case GraphicsAPI::SamplerCreateInfo::AddressMode::REPEAT:
        return GL_REPEAT;
    case GraphicsAPI::SamplerCreateInfo::AddressMode::MIRRORED_REPEAT:
        return GL_MIRRORED_REPEAT;
    case GraphicsAPI::SamplerCreateInfo::AddressMode::CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
    case GraphicsAPI::SamplerCreateInfo::AddressMode::CLAMP_TO_BORDER:
        return GL_CLAMP_TO_BORDER;
    case GraphicsAPI::SamplerCreateInfo::AddressMode::MIRROR_CLAMP_TO_EDGE:
        return GL_MIRROR_CLAMP_TO_EDGE;
    default:
        return 0;
    }
};

inline GLenum ToGLTopology(PrimitiveTopology topology) {
    switch (topology) {
    case PrimitiveTopology::POINT_LIST:
        return GL_POINTS;
    case PrimitiveTopology::LINE_LIST:
        return GL_LINES;
    case PrimitiveTopology::LINE_STRIP:
        return GL_LINE_STRIP;
    case PrimitiveTopology::TRIANGLE_LIST:
        return GL_TRIANGLES;
    case PrimitiveTopology::TRIANGLE_STRIP:
        return GL_TRIANGLE_STRIP;
    case PrimitiveTopology::TRIANGLE_FAN:
        return GL_TRIANGLE_FAN;
    default:
        return 0;
    }
};
inline GLenum ToGLPolygonMode(PolygonMode polygonMode) {
    switch (polygonMode) {
    case PolygonMode::FILL:
        return GL_BACK;
    case PolygonMode::LINE:
        return GL_FRONT;
    case PolygonMode::POINT:
        return GL_BACK;
    default:
        return 0;
    }
};
inline GLenum ToGLCullMode(CullMode cullMode) {
    switch (cullMode) {
    case CullMode::NONE:
        return GL_BACK;
    case CullMode::FRONT:
        return GL_FRONT;
    case CullMode::BACK:
        return GL_BACK;
    case CullMode::FRONT_AND_BACK:
        return GL_FRONT_AND_BACK;
    default:
        return 0;
    }
}
inline GLenum ToGLCompareOp(CompareOp op) {
    switch (op) {
    case CompareOp::NEVER:
        return GL_NEVER;
    case CompareOp::LESS:
        return GL_LESS;
    case CompareOp::EQUAL:
        return GL_EQUAL;
    case CompareOp::LESS_OR_EQUAL:
        return GL_LEQUAL;
    case CompareOp::GREATER:
        return GL_GREATER;
    case CompareOp::NOT_EQUAL:
        return GL_NOTEQUAL;
    case CompareOp::GREATER_OR_EQUAL:
        return GL_GEQUAL;
    case CompareOp::ALWAYS:
        return GL_ALWAYS;
    default:
        return 0;
    }
};
inline GLenum ToGLStencilCompareOp(StencilOp op) {
    switch (op) {
    case StencilOp::KEEP:
        return GL_KEEP;
    case StencilOp::ZERO:
        return GL_ZERO;
    case StencilOp::REPLACE:
        return GL_REPLACE;
    case StencilOp::INCREMENT_AND_CLAMP:
        return GL_INCR;
    case StencilOp::DECREMENT_AND_CLAMP:
        return GL_DECR;
    case StencilOp::INVERT:
        return GL_INVERT;
    case StencilOp::INCREMENT_AND_WRAP:
        return GL_INCR_WRAP;
    case StencilOp::DECREMENT_AND_WRAP:
        return GL_DECR_WRAP;
    default:
        return 0;
    }
};
inline GLenum ToGLBlendFactor(BlendFactor factor) {
    switch (factor) {
    case BlendFactor::ZERO:
        return GL_ZERO;
    case BlendFactor::ONE:
        return GL_ONE;
    case BlendFactor::SRC_COLOUR:
        return GL_SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOUR:
        return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DST_COLOUR:
        return GL_DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOUR:
        return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::SRC_ALPHA:
        return GL_SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
        return GL_DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
        return GL_ONE_MINUS_DST_ALPHA;
    default:
        return 0;
    }
};
inline GLenum ToGLBlendOp(BlendOp op) {
    switch (op) {
    case BlendOp::ADD:
        return GL_FUNC_ADD;
    case BlendOp::SUBTRACT:
        return GL_FUNC_SUBTRACT;
    case BlendOp::REVERSE_SUBTRACT:
        return GL_FUNC_REVERSE_SUBTRACT;
    case BlendOp::MIN:
        return GL_MIN;
    case BlendOp::MAX:
        return GL_MAX;
    default:
        return 0;
    }
};
inline GLenum ToGLLogicOp(LogicOp op) {
    switch (op) {
    case LogicOp::CLEAR:
        return GL_CLEAR;
    case LogicOp::AND:
        return GL_AND;
    case LogicOp::AND_REVERSE:
        return GL_AND_REVERSE;
    case LogicOp::COPY:
        return GL_COPY;
    case LogicOp::AND_INVERTED:
        return GL_AND_INVERTED;
    case LogicOp::NO_OP:
        return GL_NOOP;
    case LogicOp::XOR:
        return GL_XOR;
    case LogicOp::OR:
        return GL_OR;
    case LogicOp::NOR:
        return GL_NOR;
    case LogicOp::EQUIVALENT:
        return GL_EQUIV;
    case LogicOp::INVERT:
        return GL_INVERT;
    case LogicOp::OR_REVERSE:
        return GL_OR_REVERSE;
    case LogicOp::COPY_INVERTED:
        return GL_COPY_INVERTED;
    case LogicOp::OR_INVERTED:
        return GL_OR_INVERTED;
    case LogicOp::NAND:
        return GL_NAND;
    case LogicOp::SET:
        return GL_SET;
    default:
        return 0;
    }
};
#pragma endregion

// XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL
GraphicsAPI_OpenGL::GraphicsAPI_OpenGL(XrInstance xrInstance, XrSystemId systemId) {
    OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetOpenGLGraphicsRequirementsKHR", (PFN_xrVoidFunction *)&xrGetOpenGLGraphicsRequirementsKHR), "Failed to get InstanceProcAddr.");
    XrGraphicsRequirementsOpenGLKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR};
    OPENXR_CHECK(xrGetOpenGLGraphicsRequirementsKHR(xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for OpenGL.");

    // https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/f122f9f1fc729e2dc82e12c3ce73efa875182854/src/tests/hello_xr/graphicsplugin_opengl.cpp#L103-L121
    // Initialize the gl extensions. Note we have to open a window.
    ksDriverInstance driverInstance{};
    ksGpuQueueInfo queueInfo{};
    ksGpuSurfaceColorFormat colorFormat{KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8};
    ksGpuSurfaceDepthFormat depthFormat{KS_GPU_SURFACE_DEPTH_FORMAT_D24};
    ksGpuSampleCount sampleCount{KS_GPU_SAMPLE_COUNT_1};
    if (!ksGpuWindow_Create(&window, &driverInstance, &queueInfo, 0, colorFormat, depthFormat, sampleCount, 640, 480, false)) {
        std::cout << "ERROR: OPENGL: Failed to create Context." << std::endl;
    }

    GLint glMajorVersion = 0;
    GLint glMinorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

    const XrVersion glApiVersion = XR_MAKE_VERSION(glMajorVersion, glMinorVersion, 0);
    if (graphicsRequirements.minApiVersionSupported >= glApiVersion) {
        std::cout << "ERROR: OPENGL: The created OpenGL version doesn't meet the minimum requried API version for OpenXR." << std::endl;
    }
}
GraphicsAPI_OpenGL::~GraphicsAPI_OpenGL() {
    ksGpuWindow_Destroy(&window);
}
// XR_DOCS_TAG_END_GraphicsAPI_OpenGL

// XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_GetGraphicsBinding
void *GraphicsAPI_OpenGL::GetGraphicsBinding() {
    // https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/f122f9f1fc729e2dc82e12c3ce73efa875182854/src/tests/hello_xr/graphicsplugin_opengl.cpp#L123-L144
#if defined(XR_USE_PLATFORM_WIN32)
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR};
    graphicsBinding.hDC = window.context.hDC;
    graphicsBinding.hGLRC = window.context.hGLRC;
#elif defined(XR_USE_PLATFORM_XLIB)
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR};
    graphicsBinding.xDisplay = window.context.xDisplay;
    graphicsBinding.visualid = window.context.visualid;
    graphicsBinding.glxFBConfig = window.context.glxFBConfig;
    graphicsBinding.glxDrawable = window.context.glxDrawable;
    graphicsBinding.glxContext = window.context.glxContext;
#elif defined(XR_USE_PLATFORM_XCB)
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_OPENGL_XCB_KHR};
    // TODO: Still missing the platform adapter, and some items to make this usable.
    graphicsBinding.connection = window.connection;
    // m_graphicsBinding.screenNumber = window.context.screenNumber;
    // m_graphicsBinding.fbconfigid = window.context.fbconfigid;
    graphicsBinding.visualid = window.context.visualid;
    graphicsBinding.glxDrawable = window.context.glxDrawable;
    // m_graphicsBinding.glxContext = window.context.glxContext;
#elif defined(XR_USE_PLATFORM_WAYLAND)
    // TODO: Just need something other than NULL here for now (for validation).  Eventually need
    //       to correctly put in a valid pointer to an wl_display
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_OPENGL_WAYLAND};
    graphicsBinding.display = reinterpret_cast<wl_display *>(0xFFFFFFFF);
#endif
    return &graphicsBinding;
}
// XR_DOCS_TAG_END_GraphicsAPI_OpenGL_GetGraphicsBinding

XrSwapchainImageBaseHeader *GraphicsAPI_OpenGL::AllocateSwapchainImageData(uint32_t count) {
    swapchainImages.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImages.data());
}

void *GraphicsAPI_OpenGL::CreateImage(const ImageCreateInfo &imageCI) {
    GLuint texture = 0;
    glGenTextures(1, &texture);

    GLenum target = GetGLTextureTarget(imageCI);
    glBindTexture(target, texture);

    if (target == GL_TEXTURE_1D) {
        // glTexStorage1D() is not availble - Poor work around.
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexStorage2D(GL_TEXTURE_2D, imageCI.mipLevels, imageCI.format, imageCI.width, 1);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else if (target == GL_TEXTURE_2D) {
        glTexStorage2D(target, imageCI.mipLevels, imageCI.format, imageCI.width, imageCI.height);
    } else if (target == GL_TEXTURE_2D_MULTISAMPLE) {
        glTexStorage2DMultisample(target, imageCI.sampleCount, imageCI.format, imageCI.width, imageCI.height, GL_TRUE);
    } else if (target == GL_TEXTURE_3D) {
        glTexStorage3D(target, imageCI.mipLevels, imageCI.format, imageCI.width, imageCI.height, imageCI.depth);
    } else if (target == GL_TEXTURE_CUBE_MAP) {
        glTexStorage2D(target, imageCI.mipLevels, imageCI.format, imageCI.width, imageCI.height);
    } else if (target == GL_TEXTURE_1D_ARRAY) {
        glTexStorage2D(target, imageCI.mipLevels, imageCI.format, imageCI.width, imageCI.arrayLayers);
    } else if (target == GL_TEXTURE_2D_ARRAY) {
        glTexStorage3D(target, imageCI.mipLevels, imageCI.format, imageCI.width, imageCI.height, imageCI.arrayLayers);
    } else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) {
        glTexStorage3DMultisample(target, imageCI.sampleCount, imageCI.format, imageCI.width, imageCI.height, imageCI.arrayLayers, GL_TRUE);
    } else if (target == GL_TEXTURE_CUBE_MAP_ARRAY) {
        glTexStorage3D(target, imageCI.mipLevels, imageCI.format, imageCI.width, imageCI.height, imageCI.arrayLayers);
    }

    glBindTexture(target, 0);

    images[texture] = imageCI;
    return (void *)(uint64_t)texture;
}

void GraphicsAPI_OpenGL::DestroyImage(void *&image) {
    GLuint texture = (GLuint)(uint64_t)image;
    images.erase(texture);
    glDeleteTextures(1, &texture);
    image = nullptr;
}

void *GraphicsAPI_OpenGL::CreateImageView(const ImageViewCreateInfo &imageViewCI) {
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);

    GLenum attachment = imageViewCI.aspect == ImageViewCreateInfo::Aspect::COLOR_BIT ? GL_COLOR_ATTACHMENT0 : GL_DEPTH_ATTACHMENT;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D_ARRAY) {
        glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, attachment, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel, imageViewCI.baseArrayLayer, imageViewCI.layerCount);
    } else if (imageViewCI.view == ImageViewCreateInfo::View::TYPE_2D) {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, (GLuint)(uint64_t)imageViewCI.image, imageViewCI.baseMipLevel);
    } else {
        DEBUG_BREAK;
        std::cout << "ERROR: OPENGL: Unknown ImageView View type." << std::endl;
    }

    GLenum result = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        DEBUG_BREAK;
        std::cout << "ERROR: OPENGL: Framebuffer is not complete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return (void *)(uint64_t)framebuffer;
}

void GraphicsAPI_OpenGL::DestroyImageView(void *&imageView) {
    GLuint framebuffer = (GLuint)(uint64_t)imageView;
    glDeleteFramebuffers(1, &framebuffer);
    imageView = nullptr;
}

void *GraphicsAPI_OpenGL::CreateSampler(const SamplerCreateInfo &samplerCI) {
    GLuint sampler = 0;
    PFNGLGENSAMPLERSPROC glGenSamplers = (PFNGLGENSAMPLERSPROC)GetExtension("glGenSamplers");
    glGenSamplers(1, &sampler);


    PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)GetExtension("glSamplerParameteri");
    PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)GetExtension("glSamplerParameterf");
    PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)GetExtension("glSamplerParameterfv");

    
    

    // Filter
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, ToGLFilter(samplerCI.magFilter));
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, ToGLFilterMipmap(samplerCI.minFilter, samplerCI.mipmapMode));

    // AddressMode
    
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, ToGLAddressMode(samplerCI.addressModeS));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, ToGLAddressMode(samplerCI.addressModeT));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, ToGLAddressMode(samplerCI.addressModeR));

    // Lod Bias
    glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, samplerCI.mipLodBias);

    // Compare
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, samplerCI.compareEnable ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, ToGLCompareOp(samplerCI.compareOp));

    // Lod
    glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, samplerCI.minLod);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, samplerCI.maxLod);

    // BorderColour
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, samplerCI.borderColour);
    
    return (void *)(uint64_t)sampler;
}

void GraphicsAPI_OpenGL::DestroySampler(void *&sampler) {
    GLuint glsampler = (GLuint)(uint64_t)sampler;
    PFNGLDELETESAMPLERSPROC glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)GetExtension("glDeleteSamplers");
    glDeleteSamplers(1, &glsampler);
    sampler = nullptr;
}

void *GraphicsAPI_OpenGL::CreateBuffer(const BufferCreateInfo &bufferCI) {
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    GLenum target = 0;
    if (bufferCI.type == BufferCreateInfo::Type::VERTEX) {
        target = GL_ARRAY_BUFFER;
    } else if (bufferCI.type == BufferCreateInfo::Type::INDEX) {
        target = GL_ELEMENT_ARRAY_BUFFER;
    } else if (bufferCI.type == BufferCreateInfo::Type::UNIFORM) {
        target = GL_UNIFORM_BUFFER;
    } else {
        DEBUG_BREAK;
        std::cout << "ERROR: OPENGL: Unknown Buffer Type." << std::endl;
    }

    glBufferData(target, (GLsizeiptr)bufferCI.size, bufferCI.data, GL_STATIC_DRAW);
    buffers[buffer] = bufferCI;

    return (void *)(uint64_t)buffer;
}
void GraphicsAPI_OpenGL::DeleteBuffer(void *&buffer) {
    GLuint glBuffer = (GLuint)(uint64_t)buffer;
    buffers.erase(glBuffer);
    glDeleteBuffers(1, &glBuffer);
    buffer = nullptr;
}

void *GraphicsAPI_OpenGL::CreateShader(const ShaderCreateInfo &shaderCI) {
    GLenum type = 0;
    switch (shaderCI.type) {
    case ShaderCreateInfo::Type::VERTEX:
        type = GL_VERTEX_SHADER;
    case ShaderCreateInfo::Type::TESSELLATION_CONTROL:
        type = GL_TESS_CONTROL_SHADER;
    case ShaderCreateInfo::Type::TESSELLATION_EVALUATION:
        type = GL_TESS_EVALUATION_SHADER;
    case ShaderCreateInfo::Type::GEOMETRY:
        type = GL_GEOMETRY_SHADER;
    case ShaderCreateInfo::Type::FRAGMENT:
        type = GL_FRAGMENT_SHADER;
    case ShaderCreateInfo::Type::COMPUTE:
        type = GL_COMPUTE_SHADER;
    default:
        std::cout << "ERROR: OPENGL: Unknown Shader Type." << std::endl;
    }
    GLuint shader = glCreateShader(type);

    const char *shaderSource = shaderCI.source.data();
    glShaderSource(shader, 1, &shaderSource, nullptr);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(shader);
    }

    return (void *)(uint64_t)shader;
}

void GraphicsAPI_OpenGL::DeleteShader(void *&shader) {
    GLuint glShader = (GLuint)(uint64_t)shader;
    glDeleteShader(glShader);
    shader = nullptr;
}

void *GraphicsAPI_OpenGL::CreatePipeline(const PipelineCreateInfo &pipelineCI) {
    GLuint program = glCreateProgram();

    for (const void *const &shader : pipelineCI.shaders)
        glAttachShader(program, (GLuint)(uint64_t)shader);

    glLinkProgram(program);

    PFNGLVALIDATEPROGRAMPROC glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)GetExtension("glValidateProgram"); // 2.0+
    glValidateProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(program);
    }

    PFNGLDETACHSHADERPROC glDetachShader = (PFNGLDETACHSHADERPROC)GetExtension("glDetachShader"); // 2.0+
    for (const void *const &shader : pipelineCI.shaders)
        glDetachShader(program, (GLuint)(uint64_t)shader);

    pipelines[program] = pipelineCI;

    return (void *)(uint64_t)program;
}

void GraphicsAPI_OpenGL::DeletePipeline(void *&pipeline) {
    GLint program = (GLuint)(uint64_t)pipeline;
    pipelines.erase(program);
    glDeleteProgram(program);
    pipeline = nullptr;
}

void GraphicsAPI_OpenGL::BeginRendering() {
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
}

void GraphicsAPI_OpenGL::EndRendering() {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertexArray);
}

void GraphicsAPI_OpenGL::ClearColor(void *image, float r, float g, float b, float a) {
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)(uint64_t)image);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsAPI_OpenGL::ClearDepth(void *image, float d) {
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)(uint64_t)image);
    glClearDepth(d);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsAPI_OpenGL::SetPipeline(void *pipeline) {
    GLuint program = (GLuint)(uint64_t)pipeline;
    glUseProgram(program);
    setPipeline = program;

    const PipelineCreateInfo &pipelineCI = pipelines[program];

    // InputAssemblyState
    const InputAssemblyState &IAS = pipelineCI.inputAssemblyState;
    if (IAS.primitiveRestartEnable) {
        glEnable(GL_PRIMITIVE_RESTART);
    } else {
        glDisable(GL_PRIMITIVE_RESTART);
    }

    // RasterisationState
    const RasterisationState &RS = pipelineCI.rasterisationState;

    if (RS.depthClampEnable) {
        glEnable(GL_DEPTH_CLAMP);
    } else {
        glDisable(GL_DEPTH_CLAMP);
    }

    if (RS.rasteriserDiscardEnable) {
        glEnable(GL_RASTERIZER_DISCARD);
    } else {
        glDisable(GL_RASTERIZER_DISCARD);
    }

    if (RS.cullMode == CullMode::FRONT_AND_BACK) {
        glPolygonMode(GL_FRONT_AND_BACK, ToGLPolygonMode(RS.polygonMode));
    }

    if (RS.cullMode > CullMode::NONE) {
        glEnable(GL_CULL_FACE);
        glCullFace(ToGLCullMode(RS.cullMode));
    } else {
        glDisable(GL_CULL_FACE);
    }

    glFrontFace(RS.frontFace == FrontFace::COUNTER_CLOCKWISE ? GL_CCW : GL_CW);

    GLenum polygonOffsetMode = 0;
    switch (RS.polygonMode) {
    default:
    case PolygonMode::FILL: {
        polygonOffsetMode = GL_POLYGON_OFFSET_FILL;
    }
    case PolygonMode::LINE: {
        polygonOffsetMode = GL_POLYGON_OFFSET_LINE;
    }
    case PolygonMode::POINT: {
        polygonOffsetMode = GL_POLYGON_OFFSET_POINT;
    }
    }
    if (RS.depthBiasEnable) {
        glEnable(polygonOffsetMode);
        // glPolygonOffsetClamp
        glPolygonOffset(RS.depthBiasSlopeFactor, RS.depthBiasConstantFactor);
    } else {
        glDisable(polygonOffsetMode);
    }

    glLineWidth(RS.lineWidth);

    // MultisampleState
    const MultisampleState &MS = pipelineCI.multisampleState;

    if (MS.rasterisationSamples > 1) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }

    if (MS.sampleShadingEnable) {
        glEnable(GL_SAMPLE_SHADING);
        PFNGLMINSAMPLESHADINGPROC glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)GetExtension("glMinSampleShading"); // 4.0+
        glMinSampleShading(MS.minSampleShading);
    } else {
        glDisable(GL_SAMPLE_SHADING);
    }

    if (MS.sampleMask > 0) {
        glEnable(GL_SAMPLE_MASK);
        PFNGLSAMPLEMASKIPROC glSampleMaski = (PFNGLSAMPLEMASKIPROC)GetExtension("glSampleMaski"); // 3.2+
        glSampleMaski(0, MS.sampleMask);
    } else {
        glDisable(GL_SAMPLE_MASK);
    }

    if (MS.alphaToCoverageEnable) {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    } else {
        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }

    if (MS.alphaToOneEnable)
        glEnable(GL_SAMPLE_ALPHA_TO_ONE);
    else
        glDisable(GL_SAMPLE_ALPHA_TO_ONE);

    // DepthStencilState
    const DepthStencilState &DSS = pipelineCI.depthStencilState;

    if (DSS.depthTestEnable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(DSS.depthWriteEnable ? GL_TRUE : GL_FALSE);

    glDepthFunc(ToGLCompareOp(DSS.depthCompareOp));

    PFNGLDEPTHBOUNDSEXTPROC glDepthBoundsEXT = (PFNGLDEPTHBOUNDSEXTPROC)GetExtension("glDepthBoundsEXT"); // EXT
    if (glDepthBoundsEXT) {
        if (DSS.depthBoundsTestEnable) {
            glEnable(GL_DEPTH_BOUNDS_TEST_EXT);
            glDepthBoundsEXT(DSS.minDepthBounds, DSS.maxDepthBounds);
        } else {
            glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
        }
    }

    if (DSS.stencilTestEnable) {
        glEnable(GL_STENCIL_TEST);
    } else {
        glDisable(GL_STENCIL_TEST);
    }

    PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)GetExtension("glStencilOpSeparate"); // 2.0+
    PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)GetExtension("glStencilFuncSeparate"); // 2.0+
    PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)GetExtension("glStencilMaskSeparate"); // 2.0+

    glStencilOpSeparate(GL_FRONT,
                        ToGLStencilCompareOp(DSS.front.failOp),
                        ToGLStencilCompareOp(DSS.front.depthFailOp),
                        ToGLStencilCompareOp(DSS.front.passOp));
    glStencilFuncSeparate(GL_FRONT,
                          ToGLCompareOp(DSS.front.compareOp),
                          DSS.front.reference,
                          DSS.front.compareMask);
    glStencilMaskSeparate(GL_FRONT, DSS.front.writeMask);

    glStencilOpSeparate(GL_BACK,
                        ToGLStencilCompareOp(DSS.back.failOp),
                        ToGLStencilCompareOp(DSS.back.depthFailOp),
                        ToGLStencilCompareOp(DSS.back.passOp));
    glStencilFuncSeparate(GL_BACK,
                          ToGLCompareOp(DSS.back.compareOp),
                          DSS.back.reference,
                          DSS.back.compareMask);
    glStencilMaskSeparate(GL_BACK, DSS.back.writeMask);

    // ColourBlendState
    const ColourBlendState &CBS = pipelineCI.colourBlendState;

    if (CBS.logicOpEnable) {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(ToGLLogicOp(CBS.logicOp));
    } else {
        glDisable(GL_COLOR_LOGIC_OP);
    }

    for (int i = 0; i < (int)CBS.attachments.size(); i++) {
        const ColourBlendAttachmentState &CBA = CBS.attachments[i];

        PFNGLENABLEIPROC glEnablei = (PFNGLENABLEIPROC)GetExtension("glEnablei"); // 3.0+
        PFNGLDISABLEIPROC glDisablei = (PFNGLDISABLEIPROC)GetExtension("glDisablei"); // 3.0+
        PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)GetExtension("glBlendEquationSeparatei"); // 4.0+
        PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)GetExtension("glBlendFuncSeparatei"); // 4.0+
        PFNGLCOLORMASKIPROC glColorMaski = (PFNGLCOLORMASKIPROC)GetExtension("glColorMaski"); // 3.0+

        if (CBA.blendEnable) {
            glEnablei(GL_BLEND, i);
        } else {
            glDisablei(GL_BLEND, i);
        }

        glBlendEquationSeparatei(i, ToGLBlendOp(CBA.colourBlendOp), ToGLBlendOp(CBA.alphaBlendOp));

        glBlendFuncSeparatei(i,
                             ToGLBlendFactor(CBA.srcColourBlendFactor),
                             ToGLBlendFactor(CBA.dstColourBlendFactor),
                             ToGLBlendFactor(CBA.srcAlphaBlendFactor),
                             ToGLBlendFactor(CBA.dstAlphaBlendFactor));

        glColorMaski(i,
                     (((uint32_t)CBA.colourWriteMask & (uint32_t)ColourComponentBit::R_BIT) == (uint32_t)ColourComponentBit::R_BIT),
                     (((uint32_t)CBA.colourWriteMask & (uint32_t)ColourComponentBit::G_BIT) == (uint32_t)ColourComponentBit::G_BIT),
                     (((uint32_t)CBA.colourWriteMask & (uint32_t)ColourComponentBit::B_BIT) == (uint32_t)ColourComponentBit::B_BIT),
                     (((uint32_t)CBA.colourWriteMask & (uint32_t)ColourComponentBit::A_BIT) == (uint32_t)ColourComponentBit::A_BIT));
    }
    glBlendColor(CBS.blendConstants[0], CBS.blendConstants[1], CBS.blendConstants[2], CBS.blendConstants[3]);
}

void GraphicsAPI_OpenGL::SetDescriptor(const DescriptorInfo &descriptorInfo) {
    GLuint glResource = (GLuint)(uint64_t)descriptorInfo.resource;
    GLuint bindingIndex = ~0;
    GLint uniformLocation = glGetUniformLocation(setPipeline, descriptorInfo.name.c_str());
    PFNGLGETUNIFORMUIVPROC glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)GetExtension("glGetUniformuiv"); // 3.0+
    glGetUniformuiv(setPipeline, uniformLocation, &bindingIndex);

    if (descriptorInfo.type == DescriptorInfo::Type::BUFFER) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, glResource);

    } else if (descriptorInfo.type == DescriptorInfo::Type::IMAGE) {
        glActiveTexture(GL_TEXTURE0 + bindingIndex);
        glBindTexture(GetGLTextureTarget(images[glResource]), glResource);
    } else if (descriptorInfo.type == DescriptorInfo::Type::SAMPLER) {
        PFNGLBINDSAMPLERPROC glBindSampler = (PFNGLBINDSAMPLERPROC)GetExtension("glBindSampler");  // 3.0+
        glBindSampler(bindingIndex, glResource);
    } else {
        std::cout << "ERROR: OPENGL: Unknown Descriptor Type." << std::endl;
    }
}

void GraphicsAPI_OpenGL::SetVertexBuffers(void **vertexBuffers, size_t count) {
    const VertexInputState &vertexInputState = pipelines[setPipeline].vertexInputState;
    for (size_t i = 0; i < count; i++) {
        GLuint glVertexBufferID = (GLuint)(uint64_t)vertexBuffers[i];
        if (buffers[glVertexBufferID].type != BufferCreateInfo::Type::VERTEX) {
            std::cout << "ERROR: OpenGL: Provided buffer is not type: VERTEX." << std::endl;
        }            

        glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(uint64_t)vertexBuffers[i]);

        // https://i.redd.it/fyxp5ah06a661.png
        for (const VertexInputBinding &vertexBinding : vertexInputState.bindings) {
            if (vertexBinding.bindingIndex == (uint32_t)i) {
                for (const VertexInputAttribute &vertexAttribute : vertexInputState.attributes) {
                    if (vertexAttribute.bindingIndex == (uint32_t)i) {
                        GLuint attribIndex = vertexAttribute.attribIndex;
                        GLint size = ((GLint)vertexAttribute.vertexType % 4) + 1;
                        GLenum type = (GLenum)vertexAttribute.vertexType >= (GLenum)VertexType::UINT ? GL_UNSIGNED_INT : (GLenum)vertexAttribute.vertexType >= (GLenum)VertexType::INT ? GL_INT
                                                                                                                                                                                       : GL_FLOAT;
                        GLsizei stride = vertexBinding.stride;
                        const void *offset = (const void *)vertexAttribute.offset;
                        glEnableVertexAttribArray(attribIndex);
                        glVertexAttribPointer(attribIndex, size, type, false, stride, offset);
                    }
                }
            }
        }
    }
}
void GraphicsAPI_OpenGL::SetIndexBuffer(void *indexBuffer) {
    GLuint glIndexBufferID = (GLuint)(uint64_t)indexBuffer;
    if (buffers[glIndexBufferID].type != BufferCreateInfo::Type::INDEX) {
        std::cout << "ERROR: OpenGL: Provided buffer is not type: INDEX." << std::endl;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBufferID);
    setIndexBuffer = glIndexBufferID;
}

void GraphicsAPI_OpenGL::DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) {
    PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)GetExtension("glDrawElementsInstancedBaseVertexBaseInstance"); // 4.2+
    GLenum indexType = buffers[setIndexBuffer].indexBufferUint16 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
    glDrawElementsInstancedBaseVertexBaseInstance(ToGLTopology(pipelines[setPipeline].inputAssemblyState.topology), indexCount, indexType, nullptr, instanceCount, vertexOffset, firstInstance);
}

void GraphicsAPI_OpenGL::Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) {
    PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)GetExtension("glDrawArraysInstancedBaseInstance"); // 4.2+
    glDrawArraysInstancedBaseInstance(ToGLTopology(pipelines[setPipeline].inputAssemblyState.topology), firstVertex, vertexCount, instanceCount, firstInstance);
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_GetSupportedSwapchainFormats
const std::vector<int64_t> GraphicsAPI_OpenGL::GetSupportedSwapchainFormats() {
    // https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/f122f9f1fc729e2dc82e12c3ce73efa875182854/src/tests/hello_xr/graphicsplugin_opengl.cpp#L229-L236
    return {
        GL_RGB10_A2,
        GL_RGBA16F,
        // The two below should only be used as a fallback, as they are linear color formats without enough bits for color
        // depth, thus leading to banding.
        GL_RGBA8,
        GL_RGBA8_SNORM,
    };
}
// XR_DOCS_TAG_END_GraphicsAPI_OpenGL_GetSupportedSwapchainFormats
#endif