#include <salamander/utils.h>
#include <stb_image.h>
#include <glad/glad.h>

unsigned int smUtils_LoadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int            width, height, nrComponents;
    unsigned char* data =
        stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                     format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        format == GL_RGBA ? GL_CLAMP_TO_EDGE
                                          : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        format == GL_RGBA ? GL_CLAMP_TO_EDGE
                                          : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
    else
    {
        printf("Texture failed to load at path: %s\n", path);
        stbi_image_free(data);
        return smUtils_LoadTexture("res/textures/MissingTexture.png");
    }

    return textureID;
}

void smUtils_PrintMat4(float** transform)
{
    printf("\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
           transform[0][0], transform[1][0], transform[2][0],
           transform[3][0], transform[0][1], transform[1][1],
           transform[2][1], transform[3][1], transform[0][2],
           transform[1][2], transform[2][2], transform[3][2],
           transform[0][3], transform[1][3], transform[2][3],
           transform[3][3]);
}
