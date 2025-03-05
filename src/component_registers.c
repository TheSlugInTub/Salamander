#include <salamander/component_registers.h>
#include <salamander/registry.h>

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

void smSpriteRenderer_Draw(smSpriteRenderer* sprite)
{
    if (smImGui_CollapsingHeader("Sprite Renderer"))
    {
        smImGui_InputText("Texture Path", sprite->texturePath, 128, 0);
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
    Json_LoadVec4(j, "Color", sprite->color);
}

// ----------------------------------
