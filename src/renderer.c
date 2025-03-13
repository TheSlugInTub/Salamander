#include <salamander/renderer.h>
#include <stb_image.h>
#include <glad/glad.h>
#include <salamander/state.h>
#include <salamander/window.h>

unsigned int sm_VAO = 0, sm_VBO = 0,
             sm_EBO = 0;   // OpenGL buffer objects for 2d
smShader sm_shader2d = {}; // 2D shader

unsigned int sm_linesVAO2d = 0, sm_linesVBO2d = 0, sm_linesEBO2d = 0;
smShader     sm_linesShader2d = {}; // Lines shader

unsigned int sm_linesVAO3d = 0, sm_linesVBO3d = 0, sm_linesEBO3d = 0;
smShader     sm_linesShader3d = {}; // Lines shader

smShader sm_shader3d = {}; // 3D shader

smShader sm_shadowShader3d = {}; // 3D shadows shader

unsigned int sm_gBuffer = 0;
unsigned int sm_gPosition = 0, sm_gNormal = 0, sm_gColorSpec = 0,
             sm_gAlbedoSpec = 0;

void smRenderer_InitShaders()
{
    sm_shader2d =
        smShader_Create("shaders/vs_2d.glsl", "shaders/fs_2d.glsl");
    sm_linesShader2d = smShader_Create("shaders/vs_line_2d.glsl",
                                       "shaders/fs_line_2d.glsl");
    sm_linesShader3d = smShader_Create("shaders/vs_line_3d.glsl",
                                       "shaders/fs_line_3d.glsl");
    sm_shader3d =
        smShader_Create("shaders/vs_3d.glsl", "shaders/fs_3d.glsl");
    sm_shadowShader3d = smShader_CreateGeometry(
        "shaders/vs_shadow_3d.glsl", "shaders/fs_shadow_3d.glsl",
        "shaders/gs_shadow_3d.glsl");
}

void smRenderer_Init2D()
{
    stbi_set_flip_vertically_on_load(true);

    /*
    These are vertices for rendering a triangle and the indices make
    it a quad, since those are all we need for a 2D renderer.
    */
    float vertices[] = {
        // positions         // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // top left
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f  // bottom right
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &sm_VAO);
    glGenBuffers(1, &sm_VBO);
    glGenBuffers(1, &sm_EBO);

    glBindVertexArray(sm_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sm_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sm_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void smRenderer_Init3D()
{
    glGenFramebuffers(1, &sm_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, sm_gBuffer);

    const int SCR_WIDTH = smState.window->width;
    const int SCR_HEIGHT = smState.window->height;

    // - position color buffer
    glGenTextures(1, &sm_gPosition);
    glBindTexture(GL_TEXTURE_2D, sm_gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, sm_gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &sm_gNormal);
    glBindTexture(GL_TEXTURE_2D, sm_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                           GL_TEXTURE_2D, sm_gNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &sm_gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, sm_gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                           GL_TEXTURE_2D, sm_gAlbedoSpec, 0);

    // - tell OpenGL which color attachments we'll use (of this
    // framebuffer) for rendering
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0,
                                   GL_COLOR_ATTACHMENT1,
                                   GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
}

void smRenderer_InitLines()
{
    glGenVertexArrays(1, &sm_linesVAO3d);
    glGenBuffers(1, &sm_linesVBO3d);
    glGenVertexArrays(1, &sm_linesVAO2d);
    glGenBuffers(1, &sm_linesVBO2d);
}

void smRenderer_RenderQuad(vec3 position, float rotation, vec2 scale,
                           unsigned int texture, vec4 color,
                           mat4 projection, mat4 view)
{
    if (color[3] == 0)
    {
        // If the alpha of the object is zero, then don't bother with
        // rendering it.
        return;
    }

    smShader_Use(sm_shader2d);
    smShader_SetTexture2D(sm_shader2d, "texture1", texture, 0);

    mat4 transform;

    glm_mat4_identity(transform);

    glm_translate(transform, (vec3) {position[0], position[1], 0.0f});
    glm_rotate(transform, rotation, (vec3) {0.0f, 0.0f, 1.0f});
    glm_scale(transform, (vec3) {scale[0], scale[1], 1.0f});

    // Setting all the uniforms.
    smShader_SetMat4(sm_shader2d, "model", transform);
    smShader_SetMat4(sm_shader2d, "view", view);
    smShader_SetMat4(sm_shader2d, "projection", projection);
    smShader_SetVec4(sm_shader2d, "color", color);

    glBindVertexArray(sm_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void smRenderer_RenderLine2D(vec2* lines, int lineCount, vec4 color,
                             float pointSize, float lineSize,
                             bool looping, mat4 projection, mat4 view)
{
    glBindVertexArray(sm_linesVAO2d);

    glBindBuffer(GL_ARRAY_BUFFER, sm_linesVBO2d);
    glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(vec2), lines,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec2),
                          (void*)0);
    glEnableVertexAttribArray(0);

    smShader_Use(sm_linesShader2d);
    smShader_SetMat4(sm_linesShader2d, "view", view);
    smShader_SetMat4(sm_linesShader2d, "projection", projection);

    smShader_SetVec4(sm_linesShader2d, "color", color);

    // Draw line
    glLineWidth(lineSize);
    glDrawArrays(looping ? GL_LINE_LOOP : GL_LINE_STRIP, 0,
                 (GLsizei)lineCount);

    // Draw points
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, (GLsizei)lineCount);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void smRenderer_RenderLine3D(vec3* lines, int lineCount, vec4 color,
                             float pointSize, float lineSize,
                             bool looping, mat4 projection, mat4 view)
{
    glBindVertexArray(sm_linesVAO3d);

    glBindBuffer(GL_ARRAY_BUFFER, sm_linesVBO3d);
    glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(vec3), lines,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
                          (void*)0);
    glEnableVertexAttribArray(0);

    smShader_Use(sm_linesShader3d);
    smShader_SetMat4(sm_linesShader3d, "view", view);
    smShader_SetMat4(sm_linesShader3d, "projection", projection);

    smShader_SetVec4(sm_linesShader3d, "color", color);

    // Draw line
    glLineWidth(lineSize);
    glDrawArrays(looping ? GL_LINE_LOOP : GL_LINE_STRIP, 0,
                 (GLsizei)lineCount);

    // Draw points
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, (GLsizei)lineCount);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void smRenderer_RenderOneLine3D(vec3 line1, vec3 line2, vec4 color,
                                float pointSize, float lineSize,
                                bool looping, mat4 projection,
                                mat4 view)
{
    glBindVertexArray(sm_linesVAO3d);

    vec3 lines[2];
    glm_vec3_copy(line1, lines[0]);
    glm_vec3_copy(line2, lines[1]);

    glBindBuffer(GL_ARRAY_BUFFER, sm_linesVBO3d);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), lines,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
                          (void*)0);
    glEnableVertexAttribArray(0);

    smShader_Use(sm_linesShader3d);
    smShader_SetMat4(sm_linesShader3d, "view", view);
    smShader_SetMat4(sm_linesShader3d, "projection", projection);

    smShader_SetVec4(sm_linesShader3d, "color", color);

    // Draw line
    glLineWidth(lineSize);
    glDrawArrays(looping ? GL_LINE_LOOP : GL_LINE_STRIP, 0,
                 (GLsizei)2);

    // Draw points
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, (GLsizei)2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
