#include <salamander/light_3d.h>
#include <salamander/state.h>
#include <glad/glad.h>
#include <salamander/imgui_layer.h>

void smLight3D_MakePointLight(smLight3D* light)
{
    if (light->shadowTransforms == NULL)
    {
        light->shadowTransforms = smVector_Create(sizeof(mat4), 6);
    }

    vec3 lightPos;
    glm_vec3_copy(light->position, lightPos);

    mat4 shadowProj;
    glm_perspective(glm_rad(90.0f),
                    (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 1.0f,
                    25.0f, shadowProj);

    if (light->shadowTransforms->size != 0)
    {
        smVector_Clear(light->shadowTransforms);
    }

    // For each face of the cube
    // +X direction
    {
        mat4 view, result;
        vec3 center, up = {0.0f, -1.0f, 0.0f};

        glm_vec3_add(lightPos, (vec3) {1.0f, 0.0f, 0.0f}, center);
        glm_lookat(lightPos, center, up, view);
        glm_mat4_mul(shadowProj, view, result);

        smVector_PushBack(light->shadowTransforms, &result);
    }

    // -X direction
    {
        mat4 view, result;
        vec3 center, up = {0.0f, -1.0f, 0.0f};

        glm_vec3_add(lightPos, (vec3) {-1.0f, 0.0f, 0.0f}, center);
        glm_lookat(lightPos, center, up, view);
        glm_mat4_mul(shadowProj, view, result);

        smVector_PushBack(light->shadowTransforms, &result);
    }

    // +Y direction
    {
        mat4 view, result;
        vec3 center, up = {0.0f, 0.0f, 1.0f};

        glm_vec3_add(lightPos, (vec3) {0.0f, 1.0f, 0.0f}, center);
        glm_lookat(lightPos, center, up, view);
        glm_mat4_mul(shadowProj, view, result);

        smVector_PushBack(light->shadowTransforms, &result);
    }

    // -Y direction
    {
        mat4 view, result;
        vec3 center, up = {0.0f, 0.0f, -1.0f};

        glm_vec3_add(lightPos, (vec3) {0.0f, -1.0f, 0.0f}, center);
        glm_lookat(lightPos, center, up, view);
        glm_mat4_mul(shadowProj, view, result);

        smVector_PushBack(light->shadowTransforms, &result);
    }

    // +Z direction
    {
        mat4 view, result;
        vec3 center, up = {0.0f, -1.0f, 0.0f};

        glm_vec3_add(lightPos, (vec3) {0.0f, 0.0f, 1.0f}, center);
        glm_lookat(lightPos, center, up, view);
        glm_mat4_mul(shadowProj, view, result);

        smVector_PushBack(light->shadowTransforms, &result);
    }

    // -Z direction
    {
        mat4 view, result;
        vec3 center, up = {0.0f, -1.0f, 0.0f};

        glm_vec3_add(lightPos, (vec3) {0.0f, 0.0f, -1.0f}, center);
        glm_lookat(lightPos, center, up, view);
        glm_mat4_mul(shadowProj, view, result);

        smVector_PushBack(light->shadowTransforms, &result);
    }

    // The OpenGL code remains the same
    glGenFramebuffers(1, &light->depthMapFBO);

    // Create depth cubemap texture
    glGenTextures(1, &light->depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthCubemap);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

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

    // Attach depth cubemap as the FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, light->depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         light->depthCubemap, 0);
    glDrawBuffer(GL_NONE); // No color buffer is drawn
    glReadBuffer(GL_NONE); // No need to read from a buffer

    // Ensure framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE)
        printf("Framebuffer not complete!\n");

    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void smLight3D_StartSys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(smLight3D))
    {
        smLight3D* light =
            SM_ECS_GET(smState.scene, _entity, smLight3D);

        smLight3D_MakePointLight(light);
    }
    SM_ECS_ITER_END();
}

void smLight3D_Draw(smLight3D* light)
{
    if (smImGui_CollapsingHeader("Light3D"))
    {
        smImGui_DragFloat3("Light3D Position", light->position, 0.1f);
        smImGui_DragFloat3("Light3D Color", light->position, 0.1f);
        smImGui_DragFloat("Light3D Radius", &light->radius, 0.1f);
        smImGui_DragFloat("Light3D Intensity", &light->intensity,
                          0.1f);
        smImGui_Checkbox("Light3D Casts Shadows",
                         &light->castsShadows);
    }
}

smJson smLight3D_Save(smLight3D* light)
{
    smJson j = smJson_Create();

    smJson_SaveVec3(j, "Position", light->position);
    smJson_SaveVec3(j, "Color", light->color);
    smJson_SaveFloat(j, "Radius", light->radius);
    smJson_SaveFloat(j, "Intensity", light->intensity);
    smJson_SaveBool(j, "CastsShadows", light->castsShadows);

    return j;
}

void smLight3D_Load(smLight3D* light, smJson j)
{
    smJson_LoadVec3(j, "Position", light->position);
    smJson_LoadVec3(j, "Color", light->color);
    smJson_LoadFloat(j, "Radius", &light->radius);
    smJson_LoadFloat(j, "Intensity", &light->intensity);
    smJson_LoadBool(j, "CastsShadows", &light->castsShadows);
}
