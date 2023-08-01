// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

#include "GraphicsAPI_D3D11.h"
#include "xr_linear_algebra.h"

static HWND window;
static bool g_WindowQuit = false;
static LRESULT CALLBACK WindProc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_DESTROY || msg == WM_CLOSE) {
        PostQuitMessage(0);
        g_WindowQuit = true;
        return 0;
    }
    return DefWindowProc(handle, msg, wparam, lparam);
}

static void WindowUpdate() {
    MSG msg = {0};
    if (PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int main() {
    HMODULE RenderDoc = LoadLibraryA("C:/Program Files/RenderDoc/renderdoc.dll");

    GraphicsAPI_D3D11* d3d11 = new GraphicsAPI_D3D11();

    uint32_t width = 800;
    uint32_t height = 600;

    // Creates the windows
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindProc;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "MIRU_TEST";
    RegisterClass(&wc);

    window = CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, 100, 100, width, height, 0, 0, 0, 0);
    ShowWindow(window, SW_SHOW);

    // Swapchain
    const uint32_t swapchainCount = 3;
    DXGI_FORMAT swapchainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    void* swapchain = d3d11->CreateDesktopSwapchain({width, height, swapchainCount, window, swapchainFormat, false});
    void* swapchainImages[swapchainCount] = {nullptr, nullptr, nullptr};
    void* swapchainImageViews[swapchainCount] = {nullptr, nullptr, nullptr};
    for (uint32_t i = 0; i < swapchainCount; i++) {
        swapchainImages[i] = d3d11->GetDesktopSwapchainImage(swapchain, i);

        GraphicsAPI::ImageViewCreateInfo imageViewCI;
        imageViewCI.image = swapchainImages[i];
        imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::RTV;
        imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
        imageViewCI.format = swapchainFormat;
        imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::COLOR_BIT;
        imageViewCI.baseMipLevel = 0;
        imageViewCI.levelCount = 1;
        imageViewCI.baseArrayLayer = 0;
        imageViewCI.layerCount = 1;
        swapchainImageViews[i] = d3d11->CreateImageView(imageViewCI);
    }

    GraphicsAPI::ImageCreateInfo depthImageCI;
    depthImageCI.dimension = 2;
    depthImageCI.width = width;
    depthImageCI.height = height;
    depthImageCI.depth = 1;
    depthImageCI.mipLevels = 1;
    depthImageCI.arrayLayers = 1;
    depthImageCI.sampleCount = 1;
    depthImageCI.format = d3d11->GetDepthFormat();
    depthImageCI.cubemap = false;
    depthImageCI.colorAttachment = false;
    depthImageCI.depthAttachment = true;
    depthImageCI.sampled = false;
    void* depthImage = d3d11->CreateImage(depthImageCI);

    GraphicsAPI::ImageViewCreateInfo imageViewCI;
    imageViewCI.image = depthImage;
    imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::DSV;
    imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
    imageViewCI.format = d3d11->GetDepthFormat();
    imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::DEPTH_BIT;
    imageViewCI.baseMipLevel = 0;
    imageViewCI.levelCount = 1;
    imageViewCI.baseArrayLayer = 0;
    imageViewCI.layerCount = 1;
    void* depthImageView = d3d11->CreateImageView(imageViewCI);

    struct CameraConstants {
        XrMatrix4x4f viewProj;
        XrMatrix4x4f modelViewProj;
        XrMatrix4x4f model;
    };
    CameraConstants cameraConstants;

    XrVector4f colors[6] = {
        {1.00f, 0.00f, 0.00f, 1.00f},
        {0.10f, 0.00f, 0.00f, 1.00f},
        {0.00f, 0.60f, 0.00f, 1.00f},
        {0.00f, 0.10f, 0.00f, 1.00f},
        {0.00f, 0.20f, 1.00f, 1.00f},
        {0.00f, 0.02f, 0.10f, 1.00f}};

    constexpr XrVector4f vertexPositions[] = {
        {+0.5f, +0.5f, +0.5f, 1.0f},
        {+0.5f, +0.5f, -0.5f, 1.0f},
        {+0.5f, -0.5f, +0.5f, 1.0f},
        {+0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, +0.5f, +0.5f, 1.0f},
        {-0.5f, +0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, +0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f}};

#define CUBE_FACE(V1, V2, V3, V4, V5, V6) vertexPositions[V1], vertexPositions[V2], vertexPositions[V3], vertexPositions[V4], vertexPositions[V5], vertexPositions[V6],

    XrVector4f cubeVertices[] = {
        CUBE_FACE(2, 1, 0, 2, 3, 1)  // -X
        CUBE_FACE(6, 4, 5, 6, 5, 7)  // +X
        CUBE_FACE(0, 1, 5, 0, 5, 4)  // -Y
        CUBE_FACE(2, 6, 7, 2, 7, 3)  // +Y
        CUBE_FACE(0, 4, 6, 0, 6, 2)  // -Z
        CUBE_FACE(1, 3, 7, 1, 7, 5)  // +Z
    };

    uint32_t cubeIndices[36] = {
        0, 1, 2, 3, 4, 5,        // -X
        6, 7, 8, 9, 10, 11,      // +X
        12, 13, 14, 15, 16, 17,  // -Y
        18, 19, 20, 21, 22, 23,  // +Y
        24, 25, 26, 27, 28, 29,  // -Z
        30, 31, 32, 33, 34, 35,  // +Z
    };

    void* vertexBuffer = d3d11->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::VERTEX, sizeof(float) * 4, sizeof(cubeVertices),
         &cubeVertices, false});

    void* indexBuffer = d3d11->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::INDEX, sizeof(uint32_t), sizeof(cubeIndices),
         &cubeIndices, false});

    void* uniformBuffer_Frag = d3d11->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(colors), colors, false});

    void* uniformBuffer_Vert = d3d11->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(CameraConstants), &cameraConstants, false});

    std::string vertexSource = R"(
        //Color Vertex Shader

        cbuffer CameraConstants : register(b1)
        {
            float4x4 viewProj;
            float4x4 modelViewProj;
            float4x4 model;
        };
        struct VS_IN
        {
            uint vertexId : SV_VertexId;
            float4 a_Positions : TEXCOORD0;
        };
        
        struct VS_OUT
        {
            float4 o_Position : SV_Position;
            float2 o_TexCoord : TEXCOORD0;
        };
        
        VS_OUT main(VS_IN IN)
        {
            VS_OUT OUT;
            OUT.o_Position = mul(modelViewProj,IN.a_Positions);
            int face = IN.vertexId / 6;
            OUT.o_TexCoord = float2(float(face), 0);
            return OUT;
        })";
    void* vertexShader = d3d11->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

    std::string fragmentSource = R"(
        //Color Fragment Shader
        
        struct PS_IN
        {
            float4 i_Position : SV_Position;
            float2 i_TexCoord : TEXCOORD0;
        };
        struct PS_OUT
        {
            float4 o_Color : SV_Target0;
        };
        
        cbuffer Data : register(b0)
        {
            float4 colors[6];
        };
        
        PS_OUT main(PS_IN IN)
        {
            PS_OUT OUT;
            int i = int(IN.i_TexCoord.x);
            OUT.o_Color = colors[i];
            return OUT;
        })";
    void* fragmentShader = d3d11->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});

    GraphicsAPI::PipelineCreateInfo pipelineCI;
    pipelineCI.shaders = {vertexShader, fragmentShader};
    pipelineCI.vertexInputState.attributes = {{0, 0, GraphicsAPI::VertexType::VEC4, 0, "TEXCOORD"}};
    pipelineCI.vertexInputState.bindings = {{0, 0, 4 * sizeof(float)}};
    pipelineCI.inputAssemblyState = {GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, false};
    pipelineCI.rasterisationState = {false, false, GraphicsAPI::PolygonMode::FILL, GraphicsAPI::CullMode::BACK, GraphicsAPI::FrontFace::COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f};
    pipelineCI.multisampleState = {1, false, 1.0f, 0xFFFFFFFF, false, false};
    pipelineCI.depthStencilState = {true, true, GraphicsAPI::CompareOp::LESS_OR_EQUAL, false, false, {}, {}, 0.0f, 1.0f};
    pipelineCI.colourBlendState = {false, GraphicsAPI::LogicOp::NO_OP, {{true, GraphicsAPI::BlendFactor::SRC_ALPHA, GraphicsAPI::BlendFactor::ONE_MINUS_SRC_ALPHA, GraphicsAPI::BlendOp::ADD, GraphicsAPI::BlendFactor::ONE, GraphicsAPI::BlendFactor::ZERO, GraphicsAPI::BlendOp::ADD, (GraphicsAPI::ColourComponentBit)15}}, {0.0f, 0.0f, 0.0f, 0.0f}};
    void* pipeline = d3d11->CreatePipeline(pipelineCI);

    // Main Render Loop
    while (!g_WindowQuit) {
        WindowUpdate();

        uint32_t imageIndex = 0;
        d3d11->AcquireDesktopSwapchanImage(swapchain, imageIndex);

        // Rendering
        d3d11->BeginRendering();
        d3d11->ClearColor(swapchainImageViews[imageIndex], 0.22f, 0.17f, 0.35f, 1.00f);
        d3d11->ClearDepth(depthImageView, 1.0f);

        d3d11->SetRenderAttachments(&swapchainImageViews[imageIndex], 1, depthImageView);
        GraphicsAPI::Viewport viewport = {0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
        GraphicsAPI::Rect2D scissor = {{(int32_t)0, (int32_t)0}, {width, height}};
        d3d11->SetViewports(&viewport, 1);
        d3d11->SetScissors(&scissor, 1);

        float fov_deg_v = 90.0f;
        float fov_deg_h = fov_deg_v * (float(height) / float(width));
        XrFovf fov;
        fov.angleLeft = -(fov_deg_h /2.0f);
        fov.angleRight = +(fov_deg_h / 2.0f);
        fov.angleUp = -(fov_deg_v / 2.0f);
        fov.angleDown = +(fov_deg_v / 2.0f);

        XrMatrix4x4f proj;
        XrMatrix4x4f_CreateProjectionFov(&proj, D3D11, fov, 0.05f, 100.0f);
            
        XrMatrix4x4f view = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        XrMatrix4x4f_Multiply(&cameraConstants.viewProj, &proj, &view);
        XrVector3f scale = {1.0f, 1.0f, 1.0f};
        XrPosef pose = { {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -2.0f} };
        XrMatrix4x4f_CreateTranslationRotationScale(&cameraConstants.model, &pose.position, &pose.orientation, &scale);
        XrMatrix4x4f_Multiply(&cameraConstants.modelViewProj, &cameraConstants.viewProj, &cameraConstants.model);

        d3d11->SetPipeline(pipeline);

        d3d11->SetBufferData(uniformBuffer_Vert, 0, sizeof(CameraConstants), &cameraConstants);
        d3d11->SetDescriptor({1, uniformBuffer_Vert, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX});
        
        d3d11->SetBufferData(uniformBuffer_Frag, 0, sizeof(colors), (void*)colors);
        d3d11->SetDescriptor({0, uniformBuffer_Frag, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT});

        d3d11->SetVertexBuffers(&vertexBuffer, 1);
        d3d11->SetIndexBuffer(indexBuffer);
        d3d11->DrawIndexed(36);

        d3d11->EndRendering();

        d3d11->PresentDesktopSwapchainImage(swapchain, imageIndex);
    }

    d3d11->DestroyPipeline(pipeline);
    d3d11->DestroyShader(fragmentShader);
    d3d11->DestroyShader(vertexShader);
    d3d11->DestroyBuffer(uniformBuffer_Vert);
    d3d11->DestroyBuffer(uniformBuffer_Frag);
    d3d11->DestroyBuffer(indexBuffer);
    d3d11->DestroyBuffer(vertexBuffer);

    FreeLibrary(RenderDoc);
}