#include <salamander/imgui_layer.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

struct smImGuiPayload_t
{
    const ImGuiPayload* payload;
};

ImFont* mainfont = nullptr;

extern "C"
{

void smImGui_Init(smWindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    mainfont =
        io.Fonts->AddFontFromFileTTF("res/fonts/ui_font.ttf", 14.5f);
    io.Fonts->Build();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void smImGui_NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushFont(mainfont);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                    ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground; // No background

    // Dockspace flags
    ImGuiDockNodeFlags dockspace_flags =
        ImGuiDockNodeFlags_PassthruCentralNode;

    // Begin dockspace
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
                     dockspace_flags);

    ImGui::End(); // End dockspace window
}

void smImGui_EndFrame()
{
    ImGui::PopFont();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void smImGui_Terminate()
{
    // End of program.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void smImGui_Theme1()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] =
        ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow] =
        ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] =
        ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] =
        ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] =
        ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] =
        ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] =
        ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] =
        ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] =
        ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] =
        ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] =
        ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.30f, 0.30f, 0.30f, 0.71f);
    colors[ImGuiCol_HeaderHovered] =
        ImVec4(0.34f, 0.34f, 0.34f, 0.36f);
    colors[ImGuiCol_HeaderActive] =
        ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] =
        ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] =
        ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] =
        ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] =
        ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] =
        ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] =
        ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview] =
        ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] =
        ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] =
        ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] =
        ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] =
        ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] =
        ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] =
        ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] =
        ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] =
        ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] =
        ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] =
        ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] =
        ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] =
        ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] =
        ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] =
        ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;
}

bool smImGui_Begin(const char* name)
{
    return ImGui::Begin(name);
}

void smImGui_End()
{
    ImGui::End();
}

void smImGui_DebugWindow()
{
    ImGui::ShowStyleEditor();
}

bool smImGui_DragFloat(const char* name, float* val, float speed)
{
    return ImGui::DragFloat(name, val, speed);
}

bool smImGui_DragFloat2(const char* name, float* val, float speed)
{
    return ImGui::DragFloat2(name, val, speed);
}

bool smImGui_DragFloat3(const char* name, float* val, float speed)
{
    return ImGui::DragFloat3(name, val, speed);
}

bool smImGui_DragFloat4(const char* name, float* val, float speed)
{
    return ImGui::DragFloat4(name, val, speed);
}

bool smImGui_InputInt(const char* name, int* val)
{
    return ImGui::InputInt(name, val);
}

bool smImGui_InputHex(const char* name, unsigned int* val)
{
    return ImGui::InputScalar(name, ImGuiDataType_U32, val,
                              NULL, NULL, "%08X",
                              ImGuiInputTextFlags_CharsHexadecimal);
}

bool smImGui_ComboBox(const char* name, const char** types,
                      int* currentType, int typeSize)
{
    return ImGui::Combo("Body Type", currentType, types, 3);
}

bool smImGui_Checkbox(const char* name, bool* val)
{
    return ImGui::Checkbox(name, val);
}

bool smImGui_SliderInt(const char* name, int* currentType, int min,
                       int max)
{
    return ImGui::SliderInt(name, currentType, min, max);
}

bool smImGui_Button(const char* name)
{
    return ImGui::Button(name);
}

bool smImGui_ImageButton(smImGuiTextureID tex, vec2 size)
{
    return ImGui::ImageButton(tex, ImVec2(size[0], size[1]));
}

bool smImGui_InputText(const char* name, char* buffer, size_t size,
                       int flags)
{
    return ImGui::InputText(name, buffer, size, flags);
}

bool smImGui_InputTextMultiline(const char* name, char* buffer, size_t size,
                       int flags)
{
    return ImGui::InputTextMultiline(name, buffer, size, 
                             ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), 
                             ImGuiInputTextFlags_AllowTabInput);
}

bool smImGui_ColorEdit4(const char* name, float* val)
{
    return ImGui::ColorEdit4(name, val);
}

bool smImGui_IsWindowHovered()
{
    return ImGui::IsWindowHovered();
}

bool smImGui_CollapsingHeader(const char* name)
{
    return ImGui::CollapsingHeader(name);
}

bool smImGui_ColorPicker(const char* name, vec4 color)
{
    return ImGui::ColorPicker4(name, color);
}

void smImGui_Textf(const char* val, ...)
{
    char buffer[256]; // Adjust size as needed
    // Initialize variable argument list
    va_list args;
    va_start(args, val);
    // Use vsnprintf to safely format the string
    vsnprintf(buffer, sizeof(buffer), val, args);
    // Clean up the variable argument list
    va_end(args);
    // Call ImGui::Text with the formatted name and value
    ImGui::Text("%s: %s", val, buffer);
}

void smImGui_Text(const char* val)
{
    ImGui::Text(val);
}

bool smImGui_MenuItem(const char* name)
{
    return ImGui::MenuItem(name);
}

void smImGui_PushID(int id)
{
    ImGui::PushID(id);
}

void smImGui_PopID()
{
    ImGui::PopID();
}

bool smImGui_Selectable(const char* name, bool selected)
{
    return ImGui::Selectable(name, selected);
}

bool smImGui_BeginDragDropSource(int flags)
{
    return ImGui::BeginDragDropSource(flags);
}

void smImGui_SetDragDropPayload(const char* name, const void* data,
                                size_t size)
{
    ImGui::SetDragDropPayload(name, data, size);
}

void smImGui_EndDragDropSource()
{
    ImGui::EndDragDropSource();
}

void smImGui_EndDragDropTarget()
{
    ImGui::EndDragDropTarget();
}

bool smImGui_BeginDragDropTarget()
{
    return ImGui::BeginDragDropTarget();
}

void smImGui_Separator()
{
    ImGui::Separator();
}

smImGuiPayload smImGui_AcceptDragDropPayload(const char* name)
{
    return smImGuiPayload(ImGui::AcceptDragDropPayload(name));
}

void* smImGuiPayload_GetData(smImGuiPayload payload)
{
    return payload->payload->Data;
}

int smImGuiPayload_GetDataSize(smImGuiPayload payload)
{
    return payload->payload->DataSize;
}

bool smImGui_BeginPopupContextWindow()
{
    return ImGui::BeginPopupContextWindow();
}

void smImGui_EndPopup()
{
    ImGui::EndPopup();
}
}
