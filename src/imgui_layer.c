#include <salamander/imgui_layer.h>
#include <salamander/state.h>

void smImGui_Init()
{
    struct ImGuiContext* context;
    struct ImGuiIO*      io;

    context = igCreateContext(NULL);
    io = igGetIO();

    const char* glslVersion = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(smState.window->window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // Setup style
    igStyleColorsDark(NULL);
}

void smImGui_NewFrame();
void smImGui_EndFrame();
void smImGui_Terminate();
void smImGui_EmbraceTheDarkness();
