#include <salamander/salamander.h>

int main(int argc, char** argv)
{
    smWindow window =
        smWindow_Create("Bombratter", 1920, 1080, false, true);

    smSceneHandle scene = smECS_CreateScene();

    smCamera camera = smCamera_Create((vec3) {0.0f, 0.0f, 3.0f},
                                      (vec3) {0.0f, 1.0f, 0.0f},
                                      -90.0f, 0.0f, 90.0f);

    smEngineState state;
    state.scene = scene;
    state.window = &window;
    state.camera = camera;

    smSetState(&state);

    smRenderer_InitShaders();
    smRenderer_Init2D();

    smImGui_Init(smState.window->window);
    smImGui_Theme1();

    SM_REGISTER_COMPONENT(smName, smName_Draw, smName_Save,
                          smName_Load);
    SM_REGISTER_COMPONENT(smTransform, smTransform_Draw,
                          smTransform_Save, smTransform_Load);
    SM_REGISTER_COMPONENT(smSpriteRenderer, smSpriteRenderer_Draw,
                          smSpriteRenderer_Save,
                          smSpriteRenderer_Load);

    smECS_AddSystem(SpriteRendererSys, true, false);
    smECS_AddSystem(CameraSys, true, false);

    smECS_StartEditorStartSystems();

    while (!smWindow_ShouldClose(&window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Main game loop

        if (smPlaying)
        {
            smECS_UpdateSystems();
        }
        smECS_UpdateEditorSystems();

        // Handle ImGui rendering

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
