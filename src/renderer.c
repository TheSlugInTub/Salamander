#include <salamander/renderer.h>
#include <stb_image.h>
#include <glad/glad.h>
#include <salamander/state.h>

unsigned int sm_VAO = 0, sm_VBO = 0,
             sm_EBO = 0;   // OpenGL buffer objects for 2d
smShader sm_shader2d = {}; // 2D shader

unsigned int sm_linesVAO2d = 0, sm_linesVBO2d = 0, sm_linesEBO2d = 0;
smShader     sm_linesShader2d = {}; // Lines shader

unsigned int sm_linesVAO = 0, sm_linesVBO = 0, sm_linesEBO = 0;
smShader     sm_linesShader3d = {}; // Lines shader

smShader     sm_shader3d = {}; // 3D shader

void smRenderer_InitShaders()
{
    sm_shader2d = smShader_Create("shaders/vertex_2d.glsl",
                                 "shaders/fragment_2d.glsl");
    sm_linesShader2d = smShader_Create("shaders/vertex_line_2d.glsl",
                                 "shaders/fragment_line_2d.glsl");
    sm_shader3d = smShader_Create("shaders/vertex_3d.glsl",
                                 "shaders/fragment_3d.glsl");
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

void smRenderer_InitLines()
{
    glGenVertexArrays(1, &sm_linesVAO);
    glGenBuffers(1, &sm_linesVBO);
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
    glBindVertexArray(sm_linesVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sm_linesVBO);
    glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(vec2), lines,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec2),
                          (void*)0);
    glEnableVertexAttribArray(0);

    smShader_Use(sm_linesShader2d);
    smShader_SetMat4(sm_linesShader2d, "view", view);
    smShader_SetMat4(sm_linesShader2d, "projection", projection);

    mat4 model;
    glm_mat4_identity(model);

    smShader_SetMat4(sm_linesShader2d, "model", model);
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
