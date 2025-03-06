#include <salamander/shader.h>
#include <glad/glad.h>
#include <string.h>

smShader smShader_Create(const char* vertexShader,
                         const char* fragmentShader)
{
    smShader shader = {};

    // step 1. read shaders
    char  vertexCode[5000] = {};
    char  fragmentCode[5000] = {};
    FILE* vShaderFile = fopen(vertexShader, "r");
    FILE* fShaderFile = fopen(fragmentShader, "r");

    if (vShaderFile == NULL)
    {
        printf("Vertex shader: %s could not be read.", vertexShader);
        return shader;
    }

    if (fShaderFile == NULL)
    {
        printf("Fragment shader: %s could not be read.",
               fragmentShader);
        return shader;
    }

    char line[256];
    // Read vertex shader file line by line
    while (fgets(line, sizeof(line), vShaderFile) != NULL)
    {
        strcat(vertexCode, line);
    }
    // Read fragment shader file line by line
    while (fgets(line, sizeof(line), fShaderFile) != NULL)
    {
        strcat(fragmentCode, line);
    }
    fclose(vShaderFile);
    fclose(fShaderFile);

    // step 2. compile shaders
    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexCodePtr = vertexCode;
    glShaderSource(vertex, 1, &vertexCodePtr, NULL);
    glCompileShader(vertex);
    smShader_CheckCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodePtr = fragmentCode;
    glShaderSource(fragment, 1, &fragmentCodePtr, NULL);
    glCompileShader(fragment);
    smShader_CheckCompileErrors(fragment, "FRAGMENT");

    // shader Program
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, vertex);
    glAttachShader(shader.ID, fragment);
    glLinkProgram(shader.ID);
    smShader_CheckCompileErrors(shader.ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and
    // no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shader;
}

smShader smShader_CreateGeometry(const char* vertexShader,
                                 const char* fragmentShader,
                                 const char* geometryShader)
{
    smShader shader = {};

    // step 1. read shaders
    char  vertexCode[5000];
    char  fragmentCode[5000];
    char  geometryCode[5000] = {0}; // Initialize to zero
    FILE* vShaderFile = fopen(vertexShader, "r");
    FILE* fShaderFile = fopen(fragmentShader, "r");
    FILE* gShaderFile = fopen(geometryShader, "r");

    if (vShaderFile == NULL)
    {
        printf("Vertex shader: %s could not be read.", vertexShader);
        return shader;
    }

    if (fShaderFile == NULL)
    {
        printf("Fragment shader: %s could not be read.",
               fragmentShader);
        return shader;
    }

    if (gShaderFile == NULL)
    {
        printf("Geometry shader: %s could not be read.",
               geometryShader);
        return shader;
    }

    char line[256];
    // Read vertex shader file line by line
    while (fgets(line, sizeof(line), vShaderFile) != NULL)
    {
        strcat(vertexCode, line);
    }
    // Read fragment shader file line by line
    while (fgets(line, sizeof(line), fShaderFile) != NULL)
    {
        strcat(fragmentCode, line);
    }
    // Read geometry shader file line by line
    while (fgets(line, sizeof(line), gShaderFile) != NULL)
    {
        strcat(geometryCode, line);
    }
    fclose(vShaderFile);
    fclose(fShaderFile);
    fclose(gShaderFile);

    // step 2. compile shaders
    unsigned int vertex, fragment, geometry = 0;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexCodePtr = vertexCode;
    glShaderSource(vertex, 1, &vertexCodePtr, NULL);
    glCompileShader(vertex);
    smShader_CheckCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodePtr = fragmentCode;
    glShaderSource(fragment, 1, &fragmentCodePtr, NULL);
    glCompileShader(fragment);
    smShader_CheckCompileErrors(fragment, "FRAGMENT");

    fgets(geometryCode, 5000, gShaderFile);
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    const char* geometryCodePtr = geometryCode;
    glShaderSource(geometry, 1, &geometryCodePtr, NULL);
    glCompileShader(geometry);
    smShader_CheckCompileErrors(geometry, "GEOMETRY");

    // Shader Program
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, vertex);
    glAttachShader(shader.ID, fragment);

    // Attach geometry shader if provided
    if (geometryShader)
    {
        glAttachShader(shader.ID, geometry);
    }

    glLinkProgram(shader.ID);
    smShader_CheckCompileErrors(shader.ID, "PROGRAM");

    // Clean up
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);

    // Close files
    fclose(vShaderFile);
    fclose(fShaderFile);
    fclose(gShaderFile);

    return shader;
}

void smShader_Use(smShader shader)
{
    glUseProgram(shader.ID);
}

void smShader_SetBool(smShader shader, const char* name,
                      const bool value)
{
    glUniform1i(glGetUniformLocation(shader.ID, name), (int)value);
}

void smShader_SetInt(smShader shader, const char* name,
                     const int value)
{
    glUniform1i(glGetUniformLocation(shader.ID, name), value);
}

void smShader_SetFloat(smShader shader, const char* name,
                       const float value)
{
    glUniform1f(glGetUniformLocation(shader.ID, name), value);
}

void smShader_SetVec2(smShader shader, const char* name,
                      const vec2 value)
{
    glUniform2f(glGetUniformLocation(shader.ID, name), value[0],
                value[1]);
}

void smShader_SetVec3(smShader shader, const char* name,
                      const vec3 value)
{
    glUniform3f(glGetUniformLocation(shader.ID, name), value[0],
                value[1], value[2]);
}

void smShader_SetVec4(smShader shader, const char* name,
                      const vec4 value)
{
    glUniform4f(glGetUniformLocation(shader.ID, name), value[0],
                value[1], value[2], value[3]);
}

void smShader_SetMat2(smShader shader, const char* name,
                      const mat2 value)
{
    glUniformMatrix2fv(glGetUniformLocation(shader.ID, name), 1,
                       GL_FALSE, &value[0][0]);
}

void smShader_SetMat3(smShader shader, const char* name,
                      const mat3 value)
{
    glUniformMatrix3fv(glGetUniformLocation(shader.ID, name), 1,
                       GL_FALSE, &value[0][0]);
}

void smShader_SetMat4(smShader shader, const char* name,
                      const mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, name), 1,
                       GL_FALSE, &value[0][0]);
}

void smShader_SetTexture2D(smShader shader, const char* name,
                           const unsigned int textureID,
                           int                textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shader.ID, name), textureUnit);
}

void smShader_CheckCompileErrors(unsigned int ID, const char* type)
{
    GLint  success;
    GLchar infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0)
    {
        glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(ID, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: "
                   "%s\n%s\n--------------------------",
                   type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ID, 1024, NULL, infoLog);
            printf("ERROR::PROGRAM_LINKING_ERROR of type: "
                   "%s\n%s\n--------------------------",
                   type, infoLog);
        }
    }
}
