#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>

typedef struct
{
    const char*        title;
    int                width;
    int                height;
    struct GLFWwindow* window;
} smWindow;

// Initialize a GLFW window and OpenGL context
void smWindow_Create(smWindow* window, const char* title, int width,
                     int height, bool fullscreen, bool maximize);

// Is the window closed or not? Useful for running a game loop
bool smWindow_ShouldClose(smWindow* window);

// Swaps buffers, poll events
void smWindow_Update(smWindow* window);

// (float)width / (float)height
float smWindow_GetAspectRatio(smWindow* window);

// Closes the window
void smWindow_Close(smWindow* window);
