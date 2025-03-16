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
    smRenderer_InitLines();

    smImGui_Init(smState.window);
    smImGui_Theme1();

    // // smPhysics2D_Init();
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

    smECS_AddSystem(smSpriteRenderer_Sys, true, false);
    smECS_AddSystem(smCamera_Sys, true, false);
    // // smECS_AddSystem(smRigidbody2D_StartSys, false, true);
    // // smECS_AddSystem(smCollider2D_StartSys, false, true);
    // // smECS_AddSystem(smRigidbody2D_FixCollidersStartSys, false,
    // // true);

    // // smECS_AddSystem(smRigidbody2D_Sys, false, false);
    // // smECS_AddSystem(smCollider2D_DebugSys, true, false);
    smECS_AddSystem(smMeshRenderer_StartSys, true, true);
    smECS_AddSystem(smMeshRenderer_Sys, true, false);
    smECS_AddSystem(smRigidbody3D_StartSys, false, true);
    smECS_AddSystem(smRigidbody3D_Sys, false, false);
    smECS_AddSystem(smRigidbody3D_DebugSys, true, false);
    smECS_AddSystem(smLight3D_StartSys, false, true);

    smECS_StartEditorStartSystems();

    smShader shader;
    shader =
        smShader_Create("shaders/vs_3d.glsl", "shaders/fs_3d.glsl");
    smShader depthShader;
    depthShader = smShader_CreateGeometry(
        "shaders/vs_shadow_3d.glsl", "shaders/fs_shadow_3d.glsl",
        "shaders/gs_shadow_3d.glsl");

    unsigned int slugTex =
        smUtils_LoadTexture("res/textures/Slugarius.png");

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                    GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    smShader_Use(shader);
    smShader_SetInt(shader, "texture_diffuse", 0);
    smShader_SetInt(shader, "depthMap", 1);

    smModel boxModel;
    smModel_Create(&boxModel);
    smModel_Load(&boxModel, "res/models/box.dae");
    smModel foxModel;
    smModel_Create(&foxModel);
    smModel_Load(&foxModel, "res/models/fox.obj");

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
            smECS_UpdateSystems();

            // smPhysics2D_Step();
            smPhysics3D_Step();
        }
        smECS_UpdateEditorSystems();

        float nearPlane = 1.0f, farPlane = 25.0f;

        mat4 shadowProj;
        glm_perspective(glm_rad(90.0f),
                        (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT,
                        nearPlane, farPlane, shadowProj);

        mat4 shadowTransforms[6] = {};
        vec3 lightPos_val = {/* your light position values */};
        vec3 target, up;

        // +X direction
        glm_vec3_add(lightPos_val, (vec3) {1.0f, 0.0f, 0.0f}, target);
        glm_vec3_copy((vec3) {0.0f, -1.0f, 0.0f}, up);
        mat4 viewMatrix;
        glm_lookat(lightPos_val, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[0]);

        // -X direction
        glm_vec3_add(lightPos_val, (vec3) {-1.0f, 0.0f, 0.0f},
                     target);
        glm_lookat(lightPos_val, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[1]);

        // +Y direction
        glm_vec3_add(lightPos_val, (vec3) {0.0f, 1.0f, 0.0f}, target);
        glm_vec3_copy((vec3) {0.0f, 0.0f, 1.0f}, up);
        glm_lookat(lightPos_val, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[2]);

        // -Y direction
        glm_vec3_add(lightPos_val, (vec3) {0.0f, -1.0f, 0.0f},
                     target);
        glm_vec3_copy((vec3) {0.0f, 0.0f, -1.0f}, up);
        glm_lookat(lightPos_val, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[3]);

        // +Z direction
        glm_vec3_add(lightPos_val, (vec3) {0.0f, 0.0f, 1.0f}, target);
        glm_vec3_copy((vec3) {0.0f, -1.0f, 0.0f}, up);
        glm_lookat(lightPos_val, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[4]);

        // -Z direction
        glm_vec3_add(lightPos_val, (vec3) {0.0f, 0.0f, -1.0f},
                     target);
        glm_lookat(lightPos_val, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[5]);

        // Render scene to depthmap

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        smShader_Use(depthShader);

        smShader_SetMat4(depthShader, "shadowMatrices[0]",
                         shadowTransforms[0]);
        smShader_SetMat4(depthShader, "shadowMatrices[1]",
                         shadowTransforms[1]);
        smShader_SetMat4(depthShader, "shadowMatrices[2]",
                         shadowTransforms[2]);
        smShader_SetMat4(depthShader, "shadowMatrices[3]",
                         shadowTransforms[3]);
        smShader_SetMat4(depthShader, "shadowMatrices[4]",
                         shadowTransforms[4]);
        smShader_SetMat4(depthShader, "shadowMatrices[5]",
                         shadowTransforms[5]);
        smShader_SetFloat(depthShader, "far_plane", farPlane);
        smShader_SetVec3(depthShader, "lightPOs",
                         (vec3) {0.0f, 0.0f, 0.0f});

        smTransform trans = {};
        glm_vec3_copy((vec3) {0.0f, -2.0f, 0.0f}, trans.position);
        glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, trans.rotation);
        glm_vec3_copy((vec3) {10.0f, 1.0f, 10.0f}, trans.scale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, slugTex);

        smModel_Draw(&boxModel, &trans, depthShader);

        smTransform trans2 = {};
        glm_vec3_copy((vec3) {3.0f, -1.0f, 2.0f}, trans2.position);
        glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, trans2.rotation);
        glm_vec3_copy((vec3) {1.0f, 1.0f, 1.0f}, trans2.scale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, slugTex);

        smModel_Draw(&foxModel, &trans2, depthShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render scene

        glViewport(0, 0, 1920, 1080);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        smShader_Use(shader);

        smShader_SetVec3(shader, "light.pos",
                         (vec3) {0.0f, 0.0f, 0.0f});
        smShader_SetVec3(shader, "light.color",
                         (vec3) {1.0f, 1.0f, 1.0f});
        smShader_SetFloat(shader, "light.intensity", 5.0f);
        smShader_SetFloat(shader, "light.radius", 5.0f);
        smShader_SetBool(shader, "light.castShadows", true);

        smShader_SetMat4(shader, "projection", smState.persProj);
        mat4 view;
        smCamera_GetViewMatrix(&camera, view);
        smShader_SetMat4(shader, "view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, slugTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

        smModel_Draw(&boxModel, &trans, shader);
        smModel_Draw(&foxModel, &trans2, shader);

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
