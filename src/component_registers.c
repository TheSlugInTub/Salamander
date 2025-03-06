#include <salamander/component_registers.h>
#include <salamander/registry.h>
#include <salamander/utils.h>

void smName_Draw(smName* name)
{
    smImGui_InputText("Name", name->name, 128, 0);

    smImGui_Separator();
}

Json smName_Save(smName* name)
{
    Json j = Json_Create();
    Json_SaveString(j, "Name", name->name);
    return j;
}

void smName_Load(smName* name, Json j)
{
    Json_LoadString(j, "Name", name->name);
}

// ----------------------------------

void smTransform_Draw(smTransform* trans)
{
    if (smImGui_CollapsingHeader("Transform"))
    {
        smImGui_DragFloat3("Position", trans->position, 0.1f);
        smImGui_DragFloat3("Rotation", trans->rotation, 0.1f);
        smImGui_DragFloat3("Scale", trans->scale, 0.1f);
    }
}

Json smTransform_Save(smTransform* trans)
{
    Json j = Json_Create();
    Json_SaveVec3(j, "Position", trans->position);
    Json_SaveVec3(j, "Rotation", trans->rotation);
    Json_SaveVec3(j, "Scale", trans->scale);
    return j;
}

void smTransform_Load(smTransform* trans, Json j)
{
    Json_LoadVec3(j, "Position", trans->position);
    Json_LoadVec3(j, "Rotation", trans->rotation);
    Json_LoadVec3(j, "Scale", trans->scale);
}

// ----------------------------------

void smSpriteRenderer_Draw(smSpriteRenderer* sprite)
{
    if (smImGui_CollapsingHeader("Sprite Renderer"))
    {
        if (smImGui_InputText("Texture Path", sprite->texturePath,
                              128, 0))
        {
            sprite->texture =
                smUtils_LoadTexture(sprite->texturePath);
        }

        smImGui_ColorPicker("Color", sprite->color);
    }
}

Json smSpriteRenderer_Save(smSpriteRenderer* sprite)
{
    Json j = Json_Create();
    Json_SaveString(j, "TexturePath", sprite->texturePath);
    Json_SaveVec4(j, "Color", sprite->color);
    return j;
}

void smSpriteRenderer_Load(smSpriteRenderer* sprite, Json j)
{
    Json_LoadString(j, "TexturePath", sprite->texturePath);
    sprite->texture = smUtils_LoadTexture(sprite->texturePath);
    Json_LoadVec4(j, "Color", sprite->color);
}

// ----------------------------------
