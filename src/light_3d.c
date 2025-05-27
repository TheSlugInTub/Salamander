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

    glGenFramebuffers(1, &light->depthMapFBO);

    // create depth cubemap texture
    glGenTextures(1, &light->depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthCubemap);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_DEPTH_COMPONENT, SM_SHADOW_WIDTH,
                     SM_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT,
                     GL_FLOAT, NULL);

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
    glBindFramebuffer(GL_FRAMEBUFFER, light->depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         light->depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!light->directional)
    {
        glm_vec3_copy((vec3) {1.0f, 0.0f, 0.0f}, light->direction);
    }
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

void smLight3D_Sys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(smLight3D))
    {
        smLight3D* light =
            SM_ECS_GET(smState.scene, _entity, smLight3D);

        if (light->shadowTransforms == NULL)
        {
            light->shadowTransforms =
                smVector_Create(sizeof(mat4), 6);
            smLight3D_MakePointLight(light);
        }

        smVector_Clear(light->shadowTransforms);

        float nearPlane = 1.0f, farPlane = light->radius;

        mat4 shadowProj;

        // Perspective projection for point lights
        glm_perspective(glm_rad(90.0f),
                        (float)SM_SHADOW_WIDTH /
                            (float)SM_SHADOW_HEIGHT,
                        nearPlane, farPlane, shadowProj);

        mat4 shadowTransforms[6] = {};
        vec3 target;
        vec3 up;
        if (fabsf(light->direction[1]) > 0.99f)
        {
            // If looking mostly up/down, change up to avoid gimbal
            // lock
            glm_vec3_copy((vec3) {0.0f, 0.0f, -1.0f}, up);
        }
        else
        {
            glm_vec3_copy((vec3) {0.0f, -1.0f, 0.0f}, up);
        }

        glm_mat4_identity(shadowTransforms[0]);

        vec3 normalizedDir;
        glm_vec3_copy(light->direction, normalizedDir);
        glm_normalize(normalizedDir);

        // +X direction
        glm_vec3_add(light->position, normalizedDir, target);
        mat4 viewMatrix;
        glm_lookat(light->position, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[0]);

        // -X direction
        glm_vec3_add(light->position, (vec3) {-1.0f, 0.0f, 0.0f},
                     target);
        glm_lookat(light->position, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[1]);

        // +Y direction
        glm_vec3_add(light->position, (vec3) {0.0f, 1.0f, 0.0f},
                     target);
        glm_vec3_copy((vec3) {0.0f, 0.0f, 1.0f}, up);
        glm_lookat(light->position, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[2]);

        // -Y direction
        glm_vec3_add(light->position, (vec3) {0.0f, -1.0f, 0.0f},
                     target);
        glm_vec3_copy((vec3) {0.0f, 0.0f, -1.0f}, up);
        glm_lookat(light->position, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[3]);

        // +Z direction
        glm_vec3_add(light->position, (vec3) {0.0f, 0.0f, 1.0f},
                     target);
        glm_vec3_copy((vec3) {0.0f, -1.0f, 0.0f}, up);
        glm_lookat(light->position, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[4]);

        // -Z direction
        glm_vec3_add(light->position, (vec3) {0.0f, 0.0f, -1.0f},
                     target);
        glm_lookat(light->position, target, up, viewMatrix);
        glm_mat4_mul(shadowProj, viewMatrix, shadowTransforms[5]);

        smVector_PushBack(light->shadowTransforms,
                          &shadowTransforms[0]);
        smVector_PushBack(light->shadowTransforms,
                          &shadowTransforms[1]);
        smVector_PushBack(light->shadowTransforms,
                          &shadowTransforms[2]);
        smVector_PushBack(light->shadowTransforms,
                          &shadowTransforms[3]);
        smVector_PushBack(light->shadowTransforms,
                          &shadowTransforms[4]);
        smVector_PushBack(light->shadowTransforms,
                          &shadowTransforms[5]);
    }
    SM_ECS_ITER_END();
}

void smLight3D_Draw(smLight3D* light)
{
    if (smImGui_CollapsingHeader("Light3D"))
    {
        smImGui_DragFloat3("Light3D Position", light->position, 0.1f);
        smImGui_DragFloat3("Light3D Color", light->color, 0.1f);
        smImGui_DragFloat("Light3D Radius", &light->radius, 0.1f);
        smImGui_DragFloat("Light3D Intensity", &light->intensity,
                          0.1f);
        smImGui_DragFloat("Light3D Falloff", &light->falloff, 0.1f);
        smImGui_Checkbox("Light3D Casts Shadows",
                         &light->castsShadows);
        smImGui_Checkbox("Light3D Directional", &light->directional);
        smImGui_DragFloat3("Light3D Direction", light->direction,
                           0.1f);
    }
}

smJson smLight3D_Save(smLight3D* light)
{
    smJson j = smJson_Create();

    smJson_SaveVec3(j, "Position", light->position);
    smJson_SaveVec3(j, "Color", light->color);
    smJson_SaveFloat(j, "Radius", light->radius);
    smJson_SaveFloat(j, "Intensity", light->intensity);
    smJson_SaveFloat(j, "Falloff", light->falloff);
    smJson_SaveBool(j, "CastsShadows", light->castsShadows);
    smJson_SaveBool(j, "Directional", light->directional);
    smJson_SaveVec3(j, "Direction", light->direction);

    return j;
}

void smLight3D_Load(smLight3D* light, smJson j)
{
    smJson_LoadVec3(j, "Position", light->position);
    smJson_LoadVec3(j, "Color", light->color);
    smJson_LoadFloat(j, "Radius", &light->radius);
    smJson_LoadFloat(j, "Intensity", &light->intensity);
    smJson_LoadFloat(j, "Falloff", &light->falloff);
    smJson_LoadBool(j, "CastsShadows", &light->castsShadows);
    smJson_LoadBool(j, "Directional", &light->directional);
    smJson_LoadVec3(j, "Direction", light->direction);
}
