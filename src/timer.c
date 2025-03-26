#include <salamander/timer.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

void smTimer_Start(smTimer* timer)
{
    timer->startTime = glfwGetTime();
}

void smTimer_PrintSeconds(smTimer timer)
{
    double currentTime = glfwGetTime();
    printf("Timer: %f\n", currentTime - timer.startTime);
}
