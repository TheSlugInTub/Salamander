#include <salamander/window.h>
#include <stdio.h>
#include <stdlib.h>

void APIENTRY smGLErrorCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam)
{
    if(type == GL_DEBUG_TYPE_ERROR) {
        // fprintf(stderr, "** GL ERROR ** type = 0x%x, severity = 0x%x, message = %s\n",
        //         type, severity, message);
    }
}

smWindow smWindow_Create(const char* title, int width, int height,
                         bool fullscreen, bool maximize)
{
    smWindow window;

    // Glfw: Initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    
    glDebugMessageCallback(smGLErrorCallback, NULL);
    glDebugMessageControl(GL_DONT_CARE,    // source
                          GL_DONT_CARE,    // type
                          GL_DONT_CARE,    // severity
                          0,               // count
                          NULL,            // ids
                          GL_TRUE);        // enabled


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
