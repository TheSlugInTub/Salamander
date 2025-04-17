#include <salamander/input.h>
#include <salamander/state.h>

typedef unsigned char Bool;

Bool keyState[400] = {};
Bool prevKeyState[400] = {};

bool smInput_GetKeyDown(smKey key)
{
    prevKeyState[key] = keyState[key];
    keyState[key] =
        glfwGetKey(smState.window->window, key) == GLFW_PRESS;
    return keyState[key] && !prevKeyState[key];
}

bool smInput_GetKey(smKey key)
{
    return glfwGetKey(smState.window->window, key) ==
           GLFW_PRESS;
}

bool smInput_GetMouseButtonDown(smKey mouseKey)
{
    prevKeyState[mouseKey] = keyState[mouseKey];
    keyState[mouseKey] =
        glfwGetMouseButton(smState.window->window, mouseKey) == GLFW_PRESS;
    return keyState[mouseKey] && !prevKeyState[mouseKey];
}

bool smInput_GetMouseButton(smKey mouseKey)
{
        return glfwGetMouseButton(smState.window->window,
                                  mouseKey) == GLFW_PRESS;
}

int smInput_GetMouseInputHorizontal()
{
    double mouseX, mouseY;
    glfwGetCursorPos(smState.window->window, &mouseX, &mouseY);
    return mouseX;
}

int smInput_GetMouseInputVertical()
{
    double mouseX, mouseY;
    glfwGetCursorPos(smState.window->window, &mouseX, &mouseY);
    return mouseY;
}
