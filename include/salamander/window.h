#pragma once

#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <bgfx/c99/bgfx.h>

typedef struct
{
    const char*        title;
    int                width;
    int                height;
    struct GLFWwindow* window;
    bgfx_init_t        bgfxInit;
} smWindow;

// Initialize a GLFW window and OpenGL context
smWindow smWindow_Create(const char* title, int width, int height,
                         bool fullscreen, bool maximize);

// Is the window closed or not? Useful for running a game loop
bool smWindow_ShouldClose(smWindow* window);

// Swaps buffers, poll events
void smWindow_Update(smWindow* window);

// (float)width / (float)height
float smWindow_GetAspectRatio(smWindow* window);

// Closes the window
void smWindow_Close(smWindow* window);
