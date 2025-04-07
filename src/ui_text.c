#include <salamander/ui_text.h>
#include <salamander/imgui_layer.h>
#include <salamander/state.h>
#include <salamander/renderer.h>

FT_Library sm_fontLibrary;
smUmap*    sm_fonts; // umap<const char*, umap<char, smCharacter>>

void smText_Draw(smText* text)
{
    if (smImGui_CollapsingHeader("Text"))
    {
        char buffer[256];
        strcpy(buffer, text->fontPath);
        if (smImGui_InputText("Font Path", buffer, sizeof(buffer), 0))
        {
            strcpy(text->fontPath, buffer);
        }

        smImGui_InputTextMultiline("Text Display", text->text, 4096,
                                   0);

        smImGui_ColorEdit4("Color", text->color);
        smImGui_DragFloat2("Position", text->position, 1);
        smImGui_DragFloat2("Scale", text->scale, 1);
        smImGui_DragFloat("Leading", &text->leading, 0.1f);
    }
}

smJson smText_Save(smText* text)
{
    smJson j = smJson_Create();
    smJson_SaveString(j, "FontPath", text->fontPath);
    smJson_SaveString(j, "Text", text->text);
    smJson_SaveVec4(j, "Color", text->color);
    smJson_SaveVec2(j, "Position", text->position);
    smJson_SaveVec2(j, "Scale", text->scale);
    smJson_SaveFloat(j, "Leading", text->leading);
    return j;
}

void smText_Load(smText* text, smJson j)
{
    smJson_LoadString(j, "FontPath", text->fontPath);
    smJson_LoadString(j, "Text", text->text);
    smJson_LoadVec4(j, "Color", text->color);
    smJson_LoadVec2(j, "Position", text->position);
    smJson_LoadVec2(j, "Scale", text->scale);
    smJson_LoadFloat(j, "Leading", &text->leading);
}

void smText_StartSys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(smText))
    {
        smText* text = SM_ECS_GET(smState.scene, _entity, smText);

        smLoadFont(text->fontPath, 48);
    }
    SM_ECS_ITER_END();
}

void smText_Sys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(smText))
    {
        smText* text = SM_ECS_GET(smState.scene, _entity, smText);

        smShader_Use(sm_textShader);
        glBindVertexArray(sm_textVAO);

        float x = text->position[0];
        float y = text->position[1];

        smShader_SetVec4(sm_textShader, "textColor", text->color);
        smShader_SetMat4(sm_textShader, "projection",
                         smState.orthoProj);

        // Split text into lines
        char textCopy[4096];
        strcpy(textCopy, text->text);

        // Count how many lines we have and store their lengths
        int    lineCount = 1;
        size_t lineStartIndices[100] = {
            0}; // Assuming no more than 100 lines
        size_t lineLength[100] = {0};

        for (int i = 0; i < strlen(textCopy); i++)
        {
            if (textCopy[i] == '\n')
            {
                lineStartIndices[lineCount] = i + 1;
                lineCount++;
            }
            else
            {
                lineLength[lineCount - 1]++;
            }
        }

        // Render each line separately
        for (int line = 0; line < lineCount; line++)
        {
            // Calculate the total width of this line
            float  totalWidth = 0.0f;
            char*  lineText = &textCopy[lineStartIndices[line]];
            size_t len = lineLength[line];

            for (int charIndex = 0; charIndex < len; charIndex++)
            {
                char c = lineText[charIndex];

                smUmap* umap = (smUmap*)smUmap_Get(
                    sm_fonts, (void*)text->fontPath);
                smCharacter* chPtr =
                    (smCharacter*)smUmap_Get(umap, &c);
                smCharacter ch = *chPtr;

                totalWidth += (ch.advance >> 6) * text->scale[0];
            }

            // Calculate the starting x position to center this line
            float startX = x - totalWidth / 2.0f;
            float ypos = y - line * text->leading;

            // Render each character in this line
            for (int charIndex = 0; charIndex < len; charIndex++)
            {
                char c = lineText[charIndex];

                smUmap* umap = (smUmap*)smUmap_Get(
                    sm_fonts, (void*)text->fontPath);
                smCharacter* chPtr =
                    (smCharacter*)smUmap_Get(umap, &c);
                smCharacter ch = *chPtr;

                float xpos = startX + ch.bearing[0] * text->scale[0];
                float ycharpos = ypos - (ch.size[1] - ch.bearing[1]) *
                                            text->scale[1];

                float w = ch.size[0] * text->scale[0];
                float h = ch.size[1] * text->scale[1];

                float vertices[6][4] = {
                    {xpos, ycharpos + h, 0.0f, 0.0f},
                    {xpos, ycharpos, 0.0f, 1.0f},
                    {xpos + w, ycharpos, 1.0f, 1.0f},

                    {xpos, ycharpos + h, 0.0f, 0.0f},
                    {xpos + w, ycharpos, 1.0f, 1.0f},
                    {xpos + w, ycharpos + h, 1.0f, 0.0f}};

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ch.textureID);
                glBindBuffer(GL_ARRAY_BUFFER, sm_textVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices),
                                vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLES, 0, 6);

                startX += (ch.advance >> 6) * text->scale[0];
            }
        }

        glBindVertexArray(0);
    }
    SM_ECS_ITER_END();
}

void smLoadFont(const char* fontPath, int pixelSize)
{
    FT_Face face;
    if (FT_New_Face(sm_fontLibrary, fontPath, 0, &face))
    {
        printf("ERROR::FREETYPE: Failed to load font\n");
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, pixelSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    smUmap* characters =
        smUmap_Create(sizeof(char), sizeof(smCharacter), 25);
    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("ERROR::FREETYPE: Failed to load Glyph");
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);

        smCharacter character;
        glm_vec2_copy((vec2) {face->glyph->bitmap.width,
                              face->glyph->bitmap.rows},
                      character.size);
        glm_vec2_copy((vec2) {face->glyph->bitmap_left,
                              face->glyph->bitmap_top},
                      character.bearing);
        character.textureID = texture;
        character.advance = (GLuint)(face->glyph->advance.x);

        smUmap_PushBack(characters, &c, &character);
    }
    smUmap_PushBack(sm_fonts, (void*)fontPath, (void*)characters);

    FT_Done_Face(face);
}
