#include <salamander/camera.h>
#include <salamander/state.h>

smCamera smCamera_Create(vec3 position, vec3 up,
                     float yaw, float pitch, float FOV)
{
    smCamera camera;
    camera.pitch = pitch;
    camera.yaw = yaw;
    camera.FOV = FOV;
    camera.zoom = 0.0f;
    camera.position[0] = position[0];
    camera.position[1] = position[1];
    camera.position[2] = position[2];
    camera.up[0] = up[0];
    camera.up[1] = up[1];
    camera.up[2] = up[2];
    camera.front[0] = 0.0f;
    camera.front[1] = 0.0f;
    camera.front[2] = -1.0f;
    return camera;
}

void smCamera_UpdateVectors(smCamera* camera)
{
    camera->front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    camera->front[1] = sin(glm_rad(camera->pitch));
    camera->front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    glm_normalize(camera->front);

    glm_cross(camera->front, camera->worldUp, camera->right);
    glm_cross(camera->right, camera->front, camera->up);
    glm_normalize(camera->right);
    glm_normalize(camera->up);
}

void smCamera_GetViewMatrix(smCamera* camera, mat4 view)
{
    glm_mat4_identity(view);
    vec3 center;
    glm_vec3_add(camera->position, camera->front, center);
    glm_lookat(camera->position, center, camera->up, view);
}

void smCamera_ScreenToWorld2D(const vec2 pos)
{
    // // Convert screen coordinates to Normalized Device Coordinates (NDC)
    // glm::vec4 rayStart_NDC(
    //     ((float)pos.x / (float)engineState.window->width - 0.5f) * 2.0f,
    //     ((float)pos.y / (float)engineState.window->height - 0.5f) * 2.0f,
    //     -1.0f, // Near plane
    //     1.0f);
    // glm::vec4 rayEnd_NDC(
    //     ((float)pos.x / (float)engineState.window->width - 0.5f) * 2.0f,
    //     ((float)pos.y / (float)engineState.window->height - 0.5f) * 2.0f,
    //     0.0f, // Far plane
    //     1.0f)r

    // // Compute the inverse of the combined projection and view matrix
    // glm::mat4 invM = glm::inverse(engineState.projMat * GetViewMatrix());

    // // Transform NDC coordinates to world coordinates
    // glm::vec4 rayStart_world = invM * rayStart_NDC;
    // rayStart_world /= rayStart_world.w;
    // glm::vec4 rayEnd_world = invM * rayEnd_NDC;
    // rayEnd_world /= rayEnd_world.w;

    // float planeZ = 0.0f;

    // // Compute the direction of the ray in world space
    // glm::vec3 rayDir = glm::normalize(glm::vec3(rayEnd_world - rayStart_world));

    // // Calculate the intersection of the ray with the 2D plane
    // float t = (planeZ - rayStart_world.z) / rayDir.z;
    // glm::vec2 result = glm::vec2(glm::vec3(rayStart_world) + t * rayDir);
    // result = glm::vec2(result.x, -result.y);
    // result.y += Position.y * 2 ;
    // return result;
}
