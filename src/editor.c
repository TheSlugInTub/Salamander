#include <salamander/editor.h>
#include <salamander/imgui_layer.h>
#include <salamander/components.h>
#include <salamander/registry.h>
#include <assert.h>
#include <string.h>

int      smSelectedEntityIndex = -1;
EntityID smSelectedEntity = ECS_INVALID_ENTITY;
char     smSceneName[100] = "sample_scene.json";
bool     smPlaying = false;

void smEditor_DrawHierarchy()
{
    smImGui_Begin("Hierarchy");
    for (size_t i = 0; i < ECS_EntityCount(smState.scene); ++i)
    {
        EntityID ent = ECS_GetEntityAtIndex(smState.scene, i);

        if (!ECS_IsEntityValid(ent))
            continue;

        smImGui_PushID((int)i);

        smName* name = ECS_GET(smState.scene, ent, smName);

        bool selected = smSelectedEntityIndex == (int)i;
        assert(name != nullptr);
        if (smImGui_Selectable(name->name, selected))
        {
            smSelectedEntityIndex = (int)i;
            smSelectedEntity = ECS_GetEntityAtIndex(
                smState.scene, smSelectedEntityIndex);
        }

        if (smImGui_BeginDragDropSource(i << 3))
        {
            smImGui_SetDragDropPayload("DND_DEMO_CELL", &i,
                                       sizeof(size_t));
            smImGui_Textf("Dragging Object %d", i);
            smImGui_EndDragDropSource();
        }

        if (smImGui_BeginDragDropTarget())
        {
            smImGuiPayload payload =
                smImGui_AcceptDragDropPayload("DND_DEMO_CELL");
            if (payload != NULL)
            {
                assert(payload->DataSize == sizeof(size_t));
                size_t payload_n =
                    *(size_t*)smImGuiPayload_GetData(payload);
                if (payload_n != i)
                {
                    // std::swap(engineState.scene.entities[payload_n],
                    //           engineState.scene.entities[i]);
                    if (smSelectedEntityIndex == (int)i)
                    {
                        smSelectedEntityIndex = (int)payload_n;
                        smSelectedEntity = ECS_GetEntityAtIndex(
                            smState.scene, smSelectedEntityIndex);
                    }
                    else if (smSelectedEntityIndex == (int)payload_n)
                    {
                        smSelectedEntityIndex = (int)i;
                        smSelectedEntity = ECS_GetEntityAtIndex(
                            smState.scene, smSelectedEntityIndex);
                    }
                }
            }
            smImGui_EndDragDropTarget();
        }

        smImGui_PopID();
    }

    if (smImGui_BeginPopupContextWindow())
    {
        if (smImGui_MenuItem("Add Entity"))
        {
            EntityID     ent = ECS_AddEntity(smState.scene);
            smTransform* trans =
                ECS_ASSIGN(smState.scene, ent, smTransform);

            // Avert thine eyes
            trans->position[0] = 0.0f;
            trans->position[1] = 0.0f;
            trans->position[2] = 0.0f;
            trans->rotation[0] = 0.0f;
            trans->rotation[1] = 0.0f;
            trans->rotation[2] = 0.0f;
            trans->scale[0] = 0.0f;
            trans->scale[1] = 0.0f;
            trans->scale[2] = 0.0f;

            smName* name = ECS_ASSIGN(smState.scene, ent, smName);
            strcpy(name->name, "Entity");
        }
        smImGui_EndPopup();
    }

    smImGui_End();
}

void smEditor_DrawInspector()
{
    if (smSelectedEntityIndex == -1)
        return;

    smImGui_Begin("Inspector");

    smRegistry_DrawComponents(smSelectedEntity);

    // Begin popup menu on right click
    if (smImGui_BeginPopupContextWindow())
    {
        // Loop through every registered component
        for (int i = 0; i < g_componentRegistry.registrationCount;
             ++i)
        {
            if (smImGui_MenuItem(g_componentRegistry.registrations[i]
                                     .componentType))
            {
                // Check if the component doens't already exist and
                // add it if it doenn't
                ECS_ASSIGN_N(
                    smState.scene, smSelectedEntity,
                    g_componentRegistry.registrations[i]
                        .componentType,
                    g_componentRegistry.registrations[i].size);
            }
        }

        smImGui_EndPopup();
    }

    smImGui_End();
}

void smEditor_SaveScene(const char* scene)
{
    Json j = Json_Create();
    smRegistry_SaveComponents(j);

    Json_SaveToFile(j, scene);

    Json_Destroy(j);
}

void smEditor_LoadScene(const char* scene)
{
    Json j = Json_LoadFromFile(scene);

    ECS_ClearScene(smState.scene);
    smRegistry_LoadComponents(j);

    Json_Destroy(j);
}

void smEditor_DrawTray()
{
    smImGui_Begin("Tray");

    smImGui_InputText("Scene Name", smSceneName, sizeof(smSceneName),
                      0);

    if (!smPlaying)
    {
        if (smImGui_Button("Play"))
        {
            smEditor_SaveScene(smSceneName);
            smPlaying = true;
            ECS_StartStartSystems();
        }

        if (smImGui_Button("Save"))
        {
            smEditor_SaveScene(smSceneName);
        }

        if (smImGui_Button("Load"))
        {
            smEditor_LoadScene(smSceneName);
        }
    }
    else
    {
        if (smImGui_Button("Stop"))
        {
            smPlaying = false;
            smEditor_LoadScene(smSceneName);
        }
    }

    smImGui_End();
}
