#include <salamander/salamander.h>

bool jey = false;

int main(int argc, char** argv)
{
    smWindow window =
        smWindow_Create("Bombratter", 1920, 1080, false, true);

    SceneHandle scene = ECS_CreateScene();

    smEngineState state;
    state.scene = scene;
    state.window = &window;

    smSetState(&state);

    smImGui_Init(smState.window->window);
    smImGui_Theme1();

    SM_REGISTER_COMPONENT(smName, smName_Draw, smName_Save,
                          smName_Load);
    SM_REGISTER_COMPONENT(smSpriteRenderer, smSpriteRenderer_Draw,
                          smSpriteRenderer_Save,
                          smSpriteRenderer_Load);

    while (!smWindow_ShouldClose(&window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        smImGui_NewFrame();

        smEditor_DrawHierarchy();
        smEditor_DrawInspector();
        smEditor_DrawTray();

        smImGui_EndFrame();

        smWindow_Update(&window);
    }

    smImGui_Terminate();

    return 0;
}
