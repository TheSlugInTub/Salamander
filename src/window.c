#include <salamander/window.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3native.h>

smWindow smWindow_Create(const char* title, int width, int height,
                         bool fullscreen, bool maximize)
{
    smWindow window;

    // Glfw: Initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    // Don't create an OpenGL context if using bgfx
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Glfw window creation
    // --------------------
    window.window = glfwCreateWindow(
        width, height, title,
        fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (window.window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        exit(1);
    }

    // With bgfx, we don't need to make the context current
    // glfwMakeContextCurrent(window.window);

    if (maximize)
        glfwMaximizeWindow(window.window);

    // Initialize bgfx
    // ---------------
    bgfx_init_ctor(&window.bgfxInit);

    #if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    window.bgfxInit.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window.window);
    window.bgfxInit.platformData.ndt = glfwGetX11Display();
#elif BX_PLATFORM_OSX
    window.bgfxInit.platformData.nwh = glfwGetCocoaWindow(window.window);
#elif BX_PLATFORM_WINDOWS
    window.bgfxInit.platformData.nwh = (void*)(uintptr_t)glfwGetWin32Window(window.window);
#endif

    window.bgfxInit.resolution.width = width;
    window.bgfxInit.resolution.height = height;
    window.bgfxInit.resolution.reset =
        BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4;

    // Choose your preferred rendering API (OpenGL, Vulkan, DirectX,
    // etc.) Change this based on your needs init.type =
    // BGFX_RENDERER_TYPE_OPENGL;   // For OpenGL init.type =
    // BGFX_RENDERER_TYPE_VULKAN;   // For Vulkan init.type =
    // BGFX_RENDERER_TYPE_DIRECT3D11; // For DirectX 11 or use the
    // default:
    window.bgfxInit.type =
        BGFX_RENDERER_TYPE_COUNT; // Auto-select the best renderer

    if (!bgfx_init(&window.bgfxInit))
    {
        printf("Failed to initialize bgfx\n");
        glfwTerminate();
        exit(1);
    }

    // Set view clear color
    bgfx_set_view_clear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                        0x303030ff, 1.0f, 0);

    // Set viewport
    bgfx_set_view_rect(0, 0, 0, width, height);

    window.width = width;
    window.height = height;

    return window;
}

bool smWindow_ShouldClose(smWindow* window)
{
    return glfwWindowShouldClose(window->window);
}

void smWindow_Update(smWindow* window)
{
    // Submit frame to bgfx
    bgfx_frame(false);

    // Handle GLFW events
    glfwPollEvents();
}

float smWindow_GetAspectRatio(smWindow* window)
{
    if (window->width == 0 || window->height == 0)
    {
        // Handle the minimized window case
        return 1.0f;
    }

    return (float)window->width / (float)window->height;
}

void smWindow_Close(smWindow* window)
{
    bgfx_shutdown();
    glfwTerminate();
}
