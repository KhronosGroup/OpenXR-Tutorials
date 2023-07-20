#include "GraphicsAPI_OpenGL_ES.h"

#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)

// XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES
GraphicsAPI_OpenGL_ES::GraphicsAPI_OpenGL_ES(XrInstance xrInstance, XrSystemId systemId) {
    OPENXR_CHECK(xrGetInstanceProcAddr(xrInstance, "xrGetOpenGLESGraphicsRequirementsKHR", (PFN_xrVoidFunction *)&xrGetOpenGLESGraphicsRequirementsKHR), "Failed to get InstanceProcAddr.");
    XrGraphicsRequirementsOpenGLESKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
    OPENXR_CHECK(xrGetOpenGLESGraphicsRequirementsKHR(xrInstance, systemId, &graphicsRequirements), "Failed to get Graphics Requirements for OpenGLES.");

    // https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/f122f9f1fc729e2dc82e12c3ce73efa875182854/src/tests/hello_xr/graphicsplugin_opengles.cpp#L101-L119
    // Initialize the gl extensions. Note we have to open a window.
    ksDriverInstance driverInstance{};
    ksGpuQueueInfo queueInfo{};
    ksGpuSurfaceColorFormat colorFormat{KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8};
    ksGpuSurfaceDepthFormat depthFormat{KS_GPU_SURFACE_DEPTH_FORMAT_D24};
    ksGpuSampleCount sampleCount{KS_GPU_SAMPLE_COUNT_1};
    if (!ksGpuWindow_Create(&window, &driverInstance, &queueInfo, 0, colorFormat, depthFormat, sampleCount, 640, 480, false)) {
        std::cout << "ERROR: OPENGL ES: Failed to create Context." << std::endl;
    }

    GLint glMajorVersion = 0;
    GLint glMinorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

    const XrVersion glApiVersion = XR_MAKE_VERSION(glMajorVersion, glMinorVersion, 0);
    if (graphicsRequirements.minApiVersionSupported >= glApiVersion) {
        std::cout << "ERROR: OPENGL ES: The created OpenGL ES version doesn't meet the minimum requried API version for OpenXR." << std::endl;
    }
}

GraphicsAPI_OpenGL_ES::~GraphicsAPI_OpenGL_ES() {
    ksGpuWindow_Destroy(&window);
}
// XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES

// XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES_GetGraphicsBinding
void *GraphicsAPI_OpenGL_ES::GetGraphicsBinding() {
    graphicsBinding = {XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
    graphicsBinding.display = window.display;
    graphicsBinding.config = window.context.config;
    graphicsBinding.context = window.context.context;
    return &graphicsBinding;
}
// XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES_GetGraphicsBinding

XrSwapchainImageBaseHeader *GraphicsAPI_OpenGL_ES::AllocateSwapchainImageData(uint32_t count) {
    swapchainImages.resize(count, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});
    return reinterpret_cast<XrSwapchainImageBaseHeader *>(swapchainImages.data());
}

void *GraphicsAPI_OpenGL_ES::CreateImage(const ImageCreateInfo &imageCI) {
    GLuint texture = 0;
    glGenTextures(1, &texture);

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
        std::cout << "ERROR: OPENGL: Unknown Dimension for CreateImage: " << imageCI.dimension << std::endl;
        return nullptr;
    }

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

    return (void *)(uint64_t)texture;
}
void GraphicsAPI_OpenGL_ES::DestroyImage(void *&image) {
    GLuint texture = (GLuint)(uint64_t)image;
    glDeleteTextures(1, &texture);
    image = nullptr;
}

void *GraphicsAPI_OpenGL_ES::CreateImageView(const ImageViewCreateInfo &imageViewCI) {
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

void GraphicsAPI_OpenGL_ES::DestroyImageView(void *&imageView) {
    GLuint framebuffer = (GLuint)(uint64_t)imageView;
    glDeleteFramebuffers(1, &framebuffer);
    imageView = nullptr;
}

void GraphicsAPI_OpenGL_ES::ClearColor(void *imageView, float r, float g, float b, float a) {
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)(uint64_t)imageView);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsAPI_OpenGL_ES::ClearDepth(void *imageView, float d) {
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)(uint64_t)imageView);
    glClearDepthf(d);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// XR_DOCS_TAG_BEGIN_GraphicsAPI_OpenGL_ES_GetSupportedSwapchainFormats
const std::vector<int64_t> GraphicsAPI_OpenGL_ES::GetSupportedSwapchainFormats() {
    // https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/f122f9f1fc729e2dc82e12c3ce73efa875182854/src/tests/hello_xr/graphicsplugin_opengles.cpp#L208-L216
    GLint glMajorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    if (glMajorVersion >= 3) {
        return {GL_RGBA8, GL_RGBA8_SNORM, GL_SRGB8_ALPHA8};
    } else {
        return {GL_RGBA8, GL_RGBA8_SNORM};
    }
}
// XR_DOCS_TAG_END_GraphicsAPI_OpenGL_ES_GetSupportedSwapchainFormats
#endif