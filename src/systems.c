#include <salamander/editor.h>
#include <salamander/components.h>
#include <salamander/state.h>
#include <salamander/renderer.h>
#include <salamander/systems.h>
#include <salamander/input.h>

void smSpriteRenderer_Sys()
{
    SM_ECS_ITER_START(smState.scene,
                      SM_ECS_COMPONENT_TYPE(smSpriteRenderer))
    {
        smTransform* trans =
            SM_ECS_GET(smState.scene, _entity, smTransform);
        smSpriteRenderer* sprite =
            SM_ECS_GET(smState.scene, _entity, smSpriteRenderer);

        if (trans == NULL)
            continue;

        mat4 view;
        smCamera_GetViewMatrix(&smState.camera, view);

        smRenderer_RenderQuad(
            trans->position, trans->rotation[2],
            (vec2) {trans->scale[0], trans->scale[1]},
            sprite->texture, sprite->color, smState.persProj, view);
    }
    SM_ECS_ITER_END();
}

// Variables to store the last mouse position
bool  firstMouse = true;
float lastX = 0.0f, lastY = 0.0f;

void smCamera_Sys()
{
    // Calculate movement speed
    float speed = 0.05f;

    // Calculate movement vectors based on camera orientation
    vec3 forward, right;

    glm_vec3_copy(smState.camera.front, forward);

    // Right vector is perpendicular to forward on the XZ plane
    glm_cross((vec3) {0.0f, 1.0f, 0.0f}, forward, right);
    glm_normalize(right);

    // Move forward/backward (along normalized front vector on XZ
    // plane)
    if (!sm_playing)
    {
        if (smInput_GetKey(SM_KEY_W))
        {
            smState.camera.position[0] += forward[0] * speed;
            smState.camera.position[1] += forward[1] * speed;
            smState.camera.position[2] += forward[2] * speed;
        }
        if (smInput_GetKey(SM_KEY_S))
        {
            smState.camera.position[0] -= forward[0] * speed;
            smState.camera.position[1] -= forward[1] * speed;
            smState.camera.position[2] -= forward[2] * speed;
        }

        // Strafe left/right (along right vector)
        if (smInput_GetKey(SM_KEY_A))
        {
            smState.camera.position[0] += right[0] * speed;
            smState.camera.position[2] += right[2] * speed;
        }
        if (smInput_GetKey(SM_KEY_D))
        {
            smState.camera.position[0] -= right[0] * speed;
            smState.camera.position[2] -= right[2] * speed;
        }
    }

    // Mouse looking logic (unchanged from previous suggestions)
    if (smInput_GetMouseButton(SM_MOUSE_BUTTON_MIDDLE))
    {
        double xpos, ypos;
        glfwGetCursorPos(smState.window->window, &xpos, &ypos);

        if (firstMouse)
        {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
            return; // Skip processing on first click to avoid a
                    // large jump
        }

        // Calculate offset
        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos;

        // Update last position
        lastX = (float)xpos;
        lastY = (float)ypos;

        // Apply sensitivity
        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        // Update camera angles
        smState.camera.yaw += xoffset;
        smState.camera.pitch += yoffset;

        // Constrain pitch to prevent flipping
        if (smState.camera.pitch > 89.0f)
            smState.camera.pitch = 89.0f;
        if (smState.camera.pitch < -89.0f)
            smState.camera.pitch = -89.0f;

        // Update camera vectors
        smCamera_UpdateVectors(&smState.camera);
    }
    else if (!smInput_GetMouseButton(SM_MOUSE_BUTTON_MIDDLE))
    {
        // Reset firstMouse when the middle button is released
        firstMouse = true;
    }
}
