// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

#include "GraphicsAPI_D3D11.h"
#include "GraphicsAPI_D3D12.h"
#include "xr_linear_algebra.h"

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

GraphicsAPI* graphicsAPI = nullptr;
GraphicsAPI_Type apiType = D3D12;
int64_t swapchainFormat = 0;
void *vertexBuffer = nullptr;
void *indexBuffer = nullptr;
void *uniformBuffer_Vert = nullptr;
void *uniformBuffer_Frag = nullptr;

void *vertexShader = nullptr, *fragmentShader = nullptr;
void *pipeline = nullptr;

struct CameraConstants {
    XrMatrix4x4f viewProj;
    XrMatrix4x4f modelViewProj;
    XrMatrix4x4f model;
};
CameraConstants cameraConstants;
// Six colors for the six faces of a cube. Bright for +, Dark is -
// Red for X faces, green for Y, blue for Z.
XrVector4f colors[6] = {
    {1.00f, 0.00f, 0.00f, 1.00f},
    {0.10f, 0.00f, 0.00f, 1.00f},
    {0.00f, 0.60f, 0.00f, 1.00f},
    {0.00f, 0.10f, 0.00f, 1.00f},
    {0.00f, 0.20f, 1.00f, 1.00f},
    {0.00f, 0.02f, 0.10f, 1.00f}};

void CreateResources() {
    // Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
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
        CUBE_FACE(2, 1, 0, 2, 3, 1) // -X
        CUBE_FACE(6, 4, 5, 6, 5, 7) // +X
        CUBE_FACE(0, 1, 5, 0, 5, 4) // -Y
        CUBE_FACE(2, 6, 7, 2, 7, 3) // +Y
        CUBE_FACE(0, 4, 6, 0, 6, 2) // -Z
        CUBE_FACE(1, 3, 7, 1, 7, 5) // +Z
    };

    uint32_t cubeIndices[36] = {
         0,  1,  2,  3,  4,  5, // -X
         6,  7,  8,  9, 10, 11, // +X
        12, 13, 14, 15, 16, 17, // -Y
        18, 19, 20, 21, 22, 23, // +Y
        24, 25, 26, 27, 28, 29, // -Z
        30, 31, 32, 33, 34, 35, // +Z
    };
    vertexBuffer = graphicsAPI->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::VERTEX, sizeof(float) * 4, sizeof(cubeVertices),
         &cubeVertices, false});

    indexBuffer = graphicsAPI->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::INDEX, sizeof(uint32_t), sizeof(cubeIndices),
         &cubeIndices, false});

    uniformBuffer_Frag = graphicsAPI->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(colors), colors, false});
    graphicsAPI->SetBufferData(uniformBuffer_Frag, 0, sizeof(colors), (void *)colors);

    uniformBuffer_Vert = graphicsAPI->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(CameraConstants), &cameraConstants, false});

    if (apiType == OPENGL || apiType == VULKAN) {
        std::string vertexSource = R"(
            #version 450
            //Color Vertex Shader
            layout(std140, binding = 1) uniform CameraConstants
            {
                mat4 viewProj;
                mat4 modelViewProj;
                mat4 model;
            };
            layout(location = 0) in highp vec4 a_Positions;
                layout(location = 0) out flat uvec2 o_TexCoord;
            void main()
            {
                gl_Position = modelViewProj * a_Positions;
                int face = gl_VertexID / 6;
                    o_TexCoord = uvec2(face, 0);
            })";
        vertexShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

        std::string fragmentSource = R"(
            #version 450
            //Texture Fragment Shader
            layout(location = 0) in highp vec2 i_TexCoord;
            layout(location = 0) out highp vec4 o_Color;
            layout(std140, binding = 0) uniform Data
            {
                highp vec4 colors[6];
            } d_Data;
            void main()
            {
                int i = int(i_TexCoord.x);
                o_Color = d_Data.colors[i];
            })";
        fragmentShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
    }
    if (apiType == OPENGL_ES) {
        std::string vertexSource = R"(
            #version 310 es
            //Color Vertex Shader
            layout(std140, binding = 1) uniform CameraConstants
            {
                mat4 viewProj;
                mat4 modelViewProj;
                mat4 model;
            };
            layout(location = 0) in highp vec4 a_Positions;
                layout(location = 0) out flat uvec2 o_TexCoord;
            void main()
            {
                gl_Position = modelViewProj * a_Positions;
                int face = gl_VertexID / 6;
                    o_TexCoord = uvec2(face, 0);
            })";
        vertexShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

        std::string fragmentSource = R"(
            #version 310 es
            //Color Fragment Shader
                layout(location = 0) in flat uvec2 i_TexCoord;
            layout(location = 0) out highp vec4 o_Color;
            layout(std140, binding = 0) uniform Data
            {
                highp vec4 colors[6];
            } d_Data;
            
            void main()
            {
                    uint i = i_TexCoord.x;
                o_Color = d_Data.colors[i];
            })";
        fragmentShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
    }
    if (apiType == D3D11 || apiType == D3D12) {
        std::string vertexSource = R"(
            //Color Vertex Shader

            cbuffer CameraConstants: register(b1)
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
                float4 o_Position	: SV_Position;
					uint2 o_TexCoord		: TEXCOORD0;
            };
            
            VS_OUT main(VS_IN IN)
            {
                VS_OUT OUT;
                OUT.o_Position = mul(modelViewProj,IN.a_Positions);
                    int face = IN.vertexId / 6;
                    OUT.o_TexCoord = uint2(face, 0);
                return OUT;
            })";
        vertexShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

        std::string fragmentSource = R"(
            //Color Fragment Shader
            
            struct PS_IN
            {
                float4 i_Position	: SV_Position;
					uint2 i_TexCoord		: TEXCOORD0;
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
        fragmentShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});
    }

    GraphicsAPI::PipelineCreateInfo pipelineCI;
    pipelineCI.shaders = {vertexShader, fragmentShader};
    pipelineCI.vertexInputState.attributes = {{0, 0, GraphicsAPI::VertexType::VEC4, 0, "TEXCOORD"}};
    pipelineCI.vertexInputState.bindings = {{0, 0, 4 * sizeof(float)}};
    pipelineCI.inputAssemblyState = {GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, false};
    pipelineCI.rasterisationState = {false, false, GraphicsAPI::PolygonMode::FILL, GraphicsAPI::CullMode::BACK, GraphicsAPI::FrontFace::COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f};
    pipelineCI.multisampleState = {1, false, 1.0f, 0xFFFFFFFF, false, false};
    pipelineCI.depthStencilState = {false, false, GraphicsAPI::CompareOp::GREATER, false, false, {}, {}, 0.0f, 1.0f};
    pipelineCI.colourBlendState = {false, GraphicsAPI::LogicOp::NO_OP, {{true, GraphicsAPI::BlendFactor::SRC_ALPHA, GraphicsAPI::BlendFactor::ONE_MINUS_SRC_ALPHA, GraphicsAPI::BlendOp::ADD, GraphicsAPI::BlendFactor::ONE, GraphicsAPI::BlendFactor::ZERO, GraphicsAPI::BlendOp::ADD, (GraphicsAPI::ColourComponentBit)15}}, {0.0f, 0.0f, 0.0f, 0.0f}};
    pipelineCI.colorFormats = {swapchainFormat};
    pipelineCI.depthFormat = graphicsAPI->GetDepthFormat();
    pipelineCI.layout = {{1, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX, false}, {0, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT, false}};
    pipeline = graphicsAPI->CreatePipeline(pipelineCI);
}

void DestroyResources() {
    graphicsAPI->DestroyPipeline(pipeline);
    graphicsAPI->DestroyShader(fragmentShader);
    graphicsAPI->DestroyShader(vertexShader);
    graphicsAPI->DestroyBuffer(uniformBuffer_Vert);
    graphicsAPI->DestroyBuffer(uniformBuffer_Frag);
    graphicsAPI->DestroyBuffer(indexBuffer);
    graphicsAPI->DestroyBuffer(vertexBuffer);
}

void RenderCuboid(XrPosef pose, XrVector3f scale) {
    XrMatrix4x4f_CreateTranslationRotationScale(&cameraConstants.model, &pose.position, &pose.orientation, &scale);

    XrMatrix4x4f_Multiply(&cameraConstants.modelViewProj, &cameraConstants.viewProj, &cameraConstants.model);
	
    graphicsAPI->SetPipeline(pipeline);

    graphicsAPI->SetBufferData(uniformBuffer_Vert, 0, sizeof(CameraConstants), &cameraConstants);
    graphicsAPI->SetDescriptor({1, uniformBuffer_Vert, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX, false});
    graphicsAPI->SetDescriptor({0, uniformBuffer_Frag, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT, false});
    graphicsAPI->UpdateDescriptors();

    graphicsAPI->SetVertexBuffers(&vertexBuffer, 1);
    graphicsAPI->SetIndexBuffer(indexBuffer);

    graphicsAPI->DrawIndexed(36);
}

void DrawTestObject()
{
	
	// Compute the view-projection transform.
	// All matrices (including OpenXR's) are column-major, right-handed.
	XrMatrix4x4f proj;
	XrFovf fov={-.5f,.5f,.5f,-.5f};
	XrMatrix4x4f_CreateProjectionFov(&proj, OPENGL_ES, fov, 0.05f, 100.0f);
	XrMatrix4x4f toView;
	XrVector3f scale1m{1.0f, 1.0f, 1.0f};
	XrVector3f view_position={0,0,0};
	XrQuaternionf view_orientation={0,0,0,1.0f};
	XrMatrix4x4f_CreateTranslationRotationScale(&toView, &view_position, &view_orientation, &scale1m);
	XrMatrix4x4f view;
	XrMatrix4x4f_InvertRigidBody(&view, &toView);
	XrMatrix4x4f_Multiply(&cameraConstants.viewProj, &proj, &view);
	
	// Let's draw a cuboid at the floor. Scale it by 2 in the X and Z, and 0.1 in the Y,
	RenderCuboid({{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, -1.5f, 0.0f}}, {2.0f, 0.1f, 2.0f});
	for(int i=0;i<10;i++)
	{
		float x=float(i)-5.f;
		for(int j=0;j<10;j++)
		{
			float y=float(j)-5.f;
			for(int k=0;k<10;k++)
			{
				float z=float(k)-5.f;
				RenderCuboid({{0.382862836f, -0.168145418f, 0.0987696573f, 0.902988195f}, {x,y,z}}, {0.1f, 0.1f, 0.1f});
	
			}
		}
	}
}


int main() {
    HMODULE RenderDoc = LoadLibraryA("C:/Program Files/RenderDoc/renderdoc.dll");

    graphicsAPI = new GraphicsAPI_D3D12();

    uint32_t width = 800;
    uint32_t height = 600;

    // Creates the windows
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindProc;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "GraphicsAPI_Test";
    RegisterClass(&wc);

    window = CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, 100, 100, width, height, 0, 0, 0, 0);
    ShowWindow(window, SW_SHOW);

    // Swapchain
    const uint32_t swapchainCount = 3;
    swapchainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    void* swapchain = graphicsAPI->CreateDesktopSwapchain({width, height, swapchainCount, window, swapchainFormat, false});
    void* swapchainImages[swapchainCount] = {nullptr, nullptr, nullptr};
    void* swapchainImageViews[swapchainCount] = {nullptr, nullptr, nullptr};
    for (uint32_t i = 0; i < swapchainCount; i++) {
        swapchainImages[i] = graphicsAPI->GetDesktopSwapchainImage(swapchain, i);

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
        swapchainImageViews[i] = graphicsAPI->CreateImageView(imageViewCI);
    }

    GraphicsAPI::ImageCreateInfo depthImageCI;
    depthImageCI.dimension = 2;
    depthImageCI.width = width;
    depthImageCI.height = height;
    depthImageCI.depth = 1;
    depthImageCI.mipLevels = 1;
    depthImageCI.arrayLayers = 1;
    depthImageCI.sampleCount = 1;
    depthImageCI.format = graphicsAPI->GetDepthFormat();
    depthImageCI.cubemap = false;
    depthImageCI.colorAttachment = false;
    depthImageCI.depthAttachment = true;
    depthImageCI.sampled = false;
    void* depthImage = graphicsAPI->CreateImage(depthImageCI);

    GraphicsAPI::ImageViewCreateInfo imageViewCI;
    imageViewCI.image = depthImage;
    imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::DSV;
    imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
    imageViewCI.format = graphicsAPI->GetDepthFormat();
    imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::DEPTH_BIT;
    imageViewCI.baseMipLevel = 0;
    imageViewCI.levelCount = 1;
    imageViewCI.baseArrayLayer = 0;
    imageViewCI.layerCount = 1;
    void* depthImageView = graphicsAPI->CreateImageView(imageViewCI);

	CreateResources();

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

    void* vertexBuffer = graphicsAPI->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::VERTEX, sizeof(float) * 4, sizeof(cubeVertices),
         &cubeVertices, false});

    void* indexBuffer = graphicsAPI->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::INDEX, sizeof(uint32_t), sizeof(cubeIndices),
         &cubeIndices, false});

    void* uniformBuffer_Frag = graphicsAPI->CreateBuffer(
        {GraphicsAPI::BufferCreateInfo::Type::UNIFORM, 0, sizeof(colors), colors, false});

    void* uniformBuffer_Vert = graphicsAPI->CreateBuffer(
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
    void* vertexShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::VERTEX, vertexSource.data(), vertexSource.size()});

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
    void* fragmentShader = graphicsAPI->CreateShader({GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT, fragmentSource.data(), fragmentSource.size()});

    GraphicsAPI::PipelineCreateInfo pipelineCI;
    pipelineCI.shaders = {vertexShader, fragmentShader};
    pipelineCI.vertexInputState.attributes = {{0, 0, GraphicsAPI::VertexType::VEC4, 0, "TEXCOORD"}};
    pipelineCI.vertexInputState.bindings = {{0, 0, 4 * sizeof(float)}};
    pipelineCI.inputAssemblyState = {GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, false};
    pipelineCI.rasterisationState = {false, false, GraphicsAPI::PolygonMode::FILL, GraphicsAPI::CullMode::BACK, GraphicsAPI::FrontFace::COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f};
    pipelineCI.multisampleState = {1, false, 1.0f, 0xFFFFFFFF, false, false};
    pipelineCI.depthStencilState = {true, true, GraphicsAPI::CompareOp::LESS_OR_EQUAL, false, false, {}, {}, 0.0f, 1.0f};
    pipelineCI.colourBlendState = {false, GraphicsAPI::LogicOp::NO_OP, {{true, GraphicsAPI::BlendFactor::SRC_ALPHA, GraphicsAPI::BlendFactor::ONE_MINUS_SRC_ALPHA, GraphicsAPI::BlendOp::ADD, GraphicsAPI::BlendFactor::ONE, GraphicsAPI::BlendFactor::ZERO, GraphicsAPI::BlendOp::ADD, (GraphicsAPI::ColourComponentBit)15}}, {0.0f, 0.0f, 0.0f, 0.0f}};
    pipelineCI.colorFormats = {swapchainFormat};
    pipelineCI.depthFormat = graphicsAPI->GetDepthFormat();
    pipelineCI.layout = {{1, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX, false}, {0, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT, false}};
    void* pipeline = graphicsAPI->CreatePipeline(pipelineCI);
   
    // Main Render Loop
    while (!g_WindowQuit) {
        WindowUpdate();

        uint32_t imageIndex = 0;
        graphicsAPI->AcquireDesktopSwapchanImage(swapchain, imageIndex);

        // Rendering
        graphicsAPI->BeginRendering();
        graphicsAPI->ClearColor(swapchainImageViews[imageIndex], 0.22f, 0.17f, 0.35f, 1.00f);
        graphicsAPI->ClearDepth(depthImageView, 1.0f);

        graphicsAPI->SetRenderAttachments(&swapchainImageViews[imageIndex], 1, depthImageView);
        GraphicsAPI::Viewport viewport = {0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
        GraphicsAPI::Rect2D scissor = {{(int32_t)0, (int32_t)0}, {width, height}};
        graphicsAPI->SetViewports(&viewport, 1);
        graphicsAPI->SetScissors(&scissor, 1);

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

        graphicsAPI->SetPipeline(pipeline);

        graphicsAPI->SetBufferData(uniformBuffer_Vert, 0, sizeof(CameraConstants), &cameraConstants);
        graphicsAPI->SetDescriptor({1, uniformBuffer_Vert, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX, false});
        graphicsAPI->SetBufferData(uniformBuffer_Frag, 0, sizeof(colors), (void*)colors);
        graphicsAPI->SetDescriptor({0, uniformBuffer_Frag, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT, false});
        graphicsAPI->UpdateDescriptors();

        graphicsAPI->SetVertexBuffers(&vertexBuffer, 1);
        graphicsAPI->SetIndexBuffer(indexBuffer);
        graphicsAPI->DrawIndexed(36);

        graphicsAPI->EndRendering();

        graphicsAPI->PresentDesktopSwapchainImage(swapchain, imageIndex);
    }

    graphicsAPI->DestroyPipeline(pipeline);
    graphicsAPI->DestroyShader(fragmentShader);
    graphicsAPI->DestroyShader(vertexShader);
    graphicsAPI->DestroyBuffer(uniformBuffer_Vert);
    graphicsAPI->DestroyBuffer(uniformBuffer_Frag);
    graphicsAPI->DestroyBuffer(indexBuffer);
    graphicsAPI->DestroyBuffer(vertexBuffer);

    FreeLibrary(RenderDoc);
}