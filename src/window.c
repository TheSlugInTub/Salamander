#include <salamander/window.h>
#include <stdio.h>
#include <stdlib.h>

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
    glfwMakeContextCurrent(window.window);
    if (maximize)
        glfwMaximizeWindow(window.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        exit(1);
    }

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
    glfwSwapBuffers(window->window);
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
    glfwTerminate();
}
