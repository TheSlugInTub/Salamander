#include <salamander/ui.h>
#include <salamander/state.h>
#include <salamander/components.h>
#include <salamander/renderer.h>
#include <salamander/utils.h>
#include <salamander/imgui_layer.h>

void smImage_Draw(smImage* image)
{
    if (smImGui_CollapsingHeader("Image"))
    {
        char buffer[256];
        strcpy(buffer, image->texturePath);
        if (smImGui_InputText("Texture Path", buffer, sizeof(buffer),
                              0))
        {
            strcpy(image->texturePath, buffer);
            image->texture = smUtils_LoadTexture(image->texturePath);
        }

        smImGui_ColorEdit4("Color", image->color);
        smImGui_DragFloat2("Position", image->position, 1);
        smImGui_DragFloat2("Scale", image->scale, 1);
    }
}

smJson smImage_Save(smImage* image)
{
    smJson j = smJson_Create();
    smJson_SaveString(j, "TexturePath", image->texturePath);
    smJson_SaveVec4(j, "Color", image->color);
    smJson_SaveVec2(j, "Position", image->position);
    smJson_SaveVec2(j, "Scale", image->scale);
    return j;
}

void smImage_Load(smImage* image, smJson j)
{
    smJson_LoadString(j, "TexturePath", image->texturePath);
    smJson_LoadVec4(j, "Color", image->color);
    smJson_LoadVec2(j, "Position", image->position);
    smJson_LoadVec2(j, "Scale", image->scale);

    image->texture = smUtils_LoadTexture(image->texturePath);
}

void smImage_StartSys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(smImage))
    {
        smImage* image = SM_ECS_GET(smState.scene, _entity, smImage);

        image->texture = smUtils_LoadTexture(image->texturePath);
    }
    SM_ECS_ITER_END();
}

void smImage_Sys()
{
    SM_ECS_ITER_START(smState.scene, SM_ECS_COMPONENT_TYPE(smImage))
    {
        smImage* image = SM_ECS_GET(smState.scene, _entity, smImage);

        smRenderer_RenderImage(image->position, 0.0f, image->scale,
                               image->texture, image->color,
                               smState.orthoProj);
    }
    SM_ECS_ITER_END();
}
