#include <salamander/camera.h>
#include <salamander/state.h>

smCamera smCamera_Create(vec3 position, vec3 up, float yaw,
                         float pitch, float FOV)
{
    smCamera camera;
    camera.pitch = pitch;
    camera.yaw = yaw;
    camera.roll = 0.0f;
    camera.FOV = FOV;
    camera.zoom = 0.0f;
    camera.position[0] = position[0];
    camera.position[1] = position[1];
    camera.position[2] = position[2];
    camera.up[0] = up[0];
    camera.up[1] = up[1];
    camera.up[2] = up[2];
    camera.worldUp[0] = 0.0f;
    camera.worldUp[1] = 1.0f;
    camera.worldUp[2] = 0.0f;
    camera.front[0] = 0.0f;
    camera.front[1] = 0.0f;
    camera.front[2] = -1.0f;
    return camera;
}

void smCamera_UpdateVectors(smCamera* camera)
{
    camera->front[0] =
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    camera->front[1] = sin(glm_rad(camera->pitch));
    camera->front[2] =
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    glm_normalize(camera->front);

    glm_cross(camera->front, camera->worldUp, camera->right);
    glm_cross(camera->right, camera->front, camera->up);
    glm_normalize(camera->right);
    glm_normalize(camera->up);

    if (camera->roll != 0.0f)
    {
        float cosRoll = cos(camera->roll);
        float sinRoll = sin(camera->roll);

        // Store original right vector
        vec3 originalRight;
        glm_vec3_copy(camera->right, originalRight);

        // Rotate right vector around front
        camera->right[0] =
            cosRoll * originalRight[0] - sinRoll * camera->up[0];
        camera->right[1] =
            cosRoll * originalRight[1] - sinRoll * camera->up[1];
        camera->right[2] =
            cosRoll * originalRight[2] - sinRoll * camera->up[2];

        // Recalculate up vector to be perpendicular to front and
        // right
        glm_cross(camera->right, camera->front, camera->up);
        glm_normalize(camera->right);
        glm_normalize(camera->up);
    }
}

void smCamera_GetViewMatrix(smCamera* camera, mat4 view)
{
    glm_mat4_identity(view);
    vec3 center;
    glm_vec3_add(camera->position, camera->front, center);
    glm_lookat(camera->position, center, camera->up, view);
}

void smCamera_ScreenToWorld2D(smCamera* camera, const vec2 pos,
                              vec2 dest)
{
    // Convert screen coordinates to normalized device coordinates
    // (NDC) Assuming pos is in pixels where (0,0) is top-left and
    // screen dimensions are known (we'll need to add these parameters
    // or make them globals)

    // Convert to NDC space (-1 to 1)
    float ndcX = (2.0f * pos[0]) / smState.window->width - 1.0f;
    float ndcY =
        1.0f -
        (2.0f * pos[1]) /
            smState.window->height; // Y is flipped in screen space

    // Create a ray in clip space
    vec4 rayClip = {ndcX, ndcY, -1.0f, 1.0f}; // Near plane

    // Convert to eye space
    mat4 projectionInverse;
    mat4 projection;

    // Create projection matrix
    glm_perspective(glm_rad(camera->FOV),
                    (float)smState.window->width /
                        (float)smState.window->height,
                    0.1f, 100.0f, projection);

    // Invert the projection matrix
    glm_mat4_inv(projection, projectionInverse);

    // Transform ray to eye space
    vec4 rayEye;
    glm_mat4_mulv(projectionInverse, rayClip, rayEye);
    rayEye[2] = -1.0f; // We want the ray to point forward
    rayEye[3] = 0.0f;  // Convert to direction vector

    // Transform to world space
    mat4 viewInverse;
    mat4 view;

    // Get the view matrix
    smCamera_GetViewMatrix(camera, view);

    // Invert the view matrix
    glm_mat4_inv(view, viewInverse);

    // Transform ray to world space
    vec4 rayWorld;
    glm_mat4_mulv(viewInverse, rayEye, rayWorld);

    // Normalize the direction
    vec3 rayDirection = {rayWorld[0], rayWorld[1], rayWorld[2]};
    glm_vec3_normalize(rayDirection);

    // Calculate t where ray intersects Z=0 plane
    // The plane equation is z = 0
    // Ray equation: camera->position + t * rayDirection
    // At intersection: (camera->position + t * rayDirection).z = 0
    // Solve for t: t = -camera->position.z / rayDirection.z

    // Check if ray is parallel to the Z=0 plane
    if (fabsf(rayDirection[2]) < 0.000001f)
    {
        // Ray is parallel to plane, no intersection
        dest[0] = 0.0f;
        dest[1] = 0.0f;
        return;
    }

    float t = -camera->position[2] / rayDirection[2];

    // If t is negative, the intersection is behind the camera
    if (t < 0.0f)
    {
        // No valid intersection in front of camera
        dest[0] = 0.0f;
        dest[1] = 0.0f;
        return;
    }

    // Calculate the intersection point
    vec3 intersection;
    intersection[0] = camera->position[0] + rayDirection[0] * t;
    intersection[1] = camera->position[1] + rayDirection[1] * t;
    intersection[2] = 0.0f; // Should be 0 by definition

    // Return the x,y coordinates
    dest[0] = intersection[0];
    dest[1] = intersection[1];
}
