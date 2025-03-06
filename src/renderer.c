#include "cglm/mat4.h"
#include <salamander/renderer.h>
#include <stb_image.h>
#include <glad/glad.h>

unsigned int smVAO = 0, smVBO = 0,
             smEBO = 0;   // OpenGL buffer objects for 2d
smShader smShader2d = {}; // 2D shader

unsigned int smLinesVAO2d = 0, smLinesVBO2d = 0, smLinesEBO2d = 0;
smShader     smShaderLines2d = {}; // Lines shader

unsigned int smLinesVAO = 0, smLinesVBO = 0, smLineEBO = 0;
smShader     smShaderLines3d = {}; // Lines shader

void smRenderer_InitShaders()
{
    smShader2d = smShader_Create("shaders/vertex_2d.glsl",
                                 "shaders/fragment_2d.glsl");
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

    glGenVertexArrays(1, &smVAO);
    glGenBuffers(1, &smVBO);
    glGenBuffers(1, &smEBO);

    glBindVertexArray(smVAO);

    glBindBuffer(GL_ARRAY_BUFFER, smVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smEBO);
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
    glGenVertexArrays(1, &smLinesVAO);
    glGenBuffers(1, &smLinesVBO);
    glGenVertexArrays(1, &smLinesVAO2d);
    glGenBuffers(1, &smLinesVBO2d);
}

void smRenderer_RenderQuad(const smQuad* quad)
{
    if (quad->color[3] == 0)
    {
        // If the alpha of the object is zero, then don't bother with
        // rendering it.
        return;
    }

    smShader_Use(smShader2d);
    smShader_SetTexture2D(smShader2d, "texture1", quad->texture, 0);

    mat4 transform;

    glm_mat4_identity(transform);

    glm_translate(transform, (vec3) {quad->position[0],
                                     quad->position[1], 0.0f});
    glm_rotate(transform, quad->rotation, (vec3) {0.0f, 0.0f, 1.0f});
    glm_scale(transform,
              (vec3) {quad->scale[0], quad->scale[1], 1.0f});

    // Setting all the uniforms.
    smShader_SetMat4(smShader2d, "model", transform);
    smShader_SetMat4(smShader2d, "view", quad->view);
    smShader_SetMat4(smShader2d, "projection", quad->projection);
    smShader_SetVec4(smShader2d, "color", quad->color);

    glBindVertexArray(smVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
