// Simul Software Ltd 2023
// OpenXR Tutorial for Khronos Group

#include "GraphicsAPI_D3D11.h"

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

    // Main Render Loop
    while (!g_WindowQuit) {
        WindowUpdate();

        uint32_t imageIndex = 0;
        d3d11->AcquireDesktopSwapchanImage(swapchain, imageIndex);

        //Rendering
        d3d11->BeginRendering();
        d3d11->ClearColor(swapchainImageViews[imageIndex], 0.22f, 0.17f, 0.35f, 1.00f);

        d3d11->EndRendering();

        d3d11->PresentDesktopSwapchainImage(swapchain, imageIndex);
    }

    FreeLibrary(RenderDoc);
}