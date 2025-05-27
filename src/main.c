#include <salamander/salamander.h>

const int screenWidth = 1600;
const int screenHeight = 900;

void smFramebufferSizeCallback(GLFWwindow* window, int width,
                               int height)
{
    glViewport(0, 0, width, height);

    glm_perspective(glm_rad(smState.camera.FOV),
                    smWindow_GetAspectRatio(smState.window), 0.1f,
                    1000.0f, smState.persProj);

    glm_ortho(0.0f, (float)smState.window->width, 0.0f,
              (float)smState.window->height, -100.0f, 100.0f,
              smState.orthoProj);
}

int main(int argc, char** argv)
{
    smWindow window = smWindow_Create("Bombratter", screenWidth,
                                      screenHeight, false, true);
    glfwSetFramebufferSizeCallback(window.window, smFramebufferSizeCallback);
    // glfwSwapInterval(1);

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
    smRenderer_InitUI();
    smRenderer_InitLines();
    
    smAudio_InitializeAudio();

    smImGui_Init(smState.window);
    smImGui_Theme1();

    // smPhysics2D_Init();
    smPhysics3D_Init();

    SM_REGISTER_COMPONENT(smName, smName_Draw, smName_Save,
                          smName_Load);
    SM_REGISTER_COMPONENT(smTransform, smTransform_Draw,
                          smTransform_Save, smTransform_Load);
    SM_REGISTER_COMPONENT(smSpriteRenderer, smSpriteRenderer_Draw,
                          smSpriteRenderer_Save,
                          smSpriteRenderer_Load);
    SM_REGISTER_COMPONENT(smRigidbody2D, smRigidbody2D_Draw,
                          smRigidbody2D_Save, smRigidbody2D_Load);
    SM_REGISTER_COMPONENT(smCollider2D, smCollider2D_Draw,
                          smCollider2D_Save, smCollider2D_Load);
    SM_REGISTER_COMPONENT(smMeshRenderer, smMeshRenderer_Draw,
                          smMeshRenderer_Save, smMeshRenderer_Load);
    SM_REGISTER_COMPONENT(smRigidbody3D, smRigidbody3D_Draw,
                          smRigidbody3D_Save, smRigidbody3D_Load);
    SM_REGISTER_COMPONENT(smLight3D, smLight3D_Draw, smLight3D_Save,
                          smLight3D_Load);
    SM_REGISTER_COMPONENT(smImage, smImage_Draw, smImage_Save,
                          smImage_Load);
    SM_REGISTER_COMPONENT(smText, smText_Draw, smText_Save,
                          smText_Load);

    smECS_AddSystem(smSpriteRenderer_Sys, true, false);
    smECS_AddSystem(smCamera_Sys, true, false);
    // // smECS_AddSystem(smRigidbody2D_StartSys, false, true);
    // // smECS_AddSystem(smCollider2D_StartSys, false, true);
    // // smECS_AddSystem(smRigidbody2D_FixCollidersStartSys, false,
    // // true);

    // // smECS_AddSystem(smRigidbody2D_Sys, false, false);
    // // smECS_AddSystem(smCollider2D_DebugSys, true, false);
    smECS_AddSystem(smLight3D_StartSys, false, true);
    smECS_AddSystem(smLight3D_Sys, true, false);
    smECS_AddSystem(smMeshRenderer_StartSys, true, true);
    smECS_AddSystem(smMeshRenderer_Sys, true, false);
    smECS_AddSystem(smRigidbody3D_StartSys, false, true);
    smECS_AddSystem(smRigidbody3D_Sys, false, false);
    smECS_AddSystem(smRigidbody3D_DebugSys, true, false);
    smECS_AddSystem(smDeltaTime_Sys, true, false);
    smECS_AddSystem(smImage_StartSys, true, true);
    smECS_AddSystem(smImage_Sys, true, false);
    smECS_AddSystem(smText_StartSys, true, true);
    smECS_AddSystem(smText_Sys, true, false);
    
    smECS_StartEditorStartSystems();

    glViewport(0, 0, 1920, 1080);
    glEnable(GL_DEPTH_TEST);

    JPH_PhysicsSystem_OptimizeBroadPhase(sm3d_state.system);

    float fps = 0.0f;
    int   frameCount = 0;
    float lastTime = glfwGetTime();
    float timeAccumulator = 0.0f;

    while (!smWindow_ShouldClose(&window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Main game loop

        if (sm_playing)
        {
            // smPhysics2D_Step();
            smPhysics3D_Step();
            
            smECS_UpdateSystems();
        }
        smECS_UpdateEditorSystems();

        float currentTime = glfwGetTime();
        float elapsed = currentTime - lastTime;
        lastTime = currentTime;

        // Update frame count
        frameCount++;

        // Accumulate time
        timeAccumulator += elapsed;

        // Update FPS every second
        if (timeAccumulator >= 0.1f)
        {
            fps = frameCount / timeAccumulator;

            frameCount = 0;
            timeAccumulator = 0.0f;

            printf("FPS: %f\n", fps);
        }

        // Handle ImGui rendering

        smImGui_NewFrame();

        smEditor_DrawHierarchy();
        smEditor_DrawInspector();
        smEditor_DrawTray();

        smImGui_EndFrame();

        smWindow_Update(&window);
    }

    smImGui_Terminate();

    // smPhysics2D_Destroy();
    smPhysics3D_Destroy();

    smWindow_Close(&window);

    return 0;
}
