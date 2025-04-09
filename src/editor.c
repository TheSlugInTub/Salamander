#include <salamander/editor.h>
#include <salamander/imgui_layer.h>
#include <salamander/components.h>
#include <salamander/registry.h>
#include <salamander/physics_2d.h>
#include <salamander/input.h>
#include <salamander/physics_3d.h>
#include <assert.h>
#include <string.h>

int        sm_selectedEntityIndex = -1;
smEntityID sm_selectedEntity = SM_ECS_INVALID_ENTITY;
char       sm_sceneName[100] = "sample_scene.json";
bool       sm_playing = false;

void smEditor_DrawHierarchy()
{
    smImGui_Begin("Hierarchy");
    for (size_t i = 0; i < smECS_EntityCount(smState.scene); ++i)
    {
        smEntityID ent = smECS_GetEntityAtIndex(smState.scene, i);

        if (!smECS_IsEntityValid(ent))
            continue;

        smImGui_PushID((int)i);

        smName* name = SM_ECS_GET(smState.scene, ent, smName);

        bool selected = sm_selectedEntityIndex == (int)i;
        assert(name != NULL);
        if (smImGui_Selectable(name->name, selected))
        {
            sm_selectedEntityIndex = (int)i;
            sm_selectedEntity = smECS_GetEntityAtIndex(
                smState.scene, sm_selectedEntityIndex);
        }

        if (smInput_GetKeyDown(SM_KEY_F1))
        {
            smECS_CloneEntity(smState.scene, sm_selectedEntity);
        }
        
        if (smInput_GetKeyDown(SM_KEY_F12))
        {
            smECS_DestroyEntity(smState.scene, sm_selectedEntity);
        }

        // if (smImGui_BeginDragDropSource(i << 3))
        // {
        //     smImGui_SetDragDropPayload("DND_DEMO_CELL", &i,
        //                                sizeof(size_t));
        //     smImGui_Textf("Dragging Object %d", i);
        //     smImGui_EndDragDropSource();
        // }

        // if (smImGui_BeginDragDropTarget())
        // {
        //     smImGuiPayload payload =
        //         smImGui_AcceptDragDropPayload("DND_DEMO_CELL");
        //     if (payload != NULL)
        //     {
        //         assert(payload->DataSize == sizeof(size_t));
        //         size_t payload_n =
        //             *(size_t*)smImGuiPayload_GetData(payload);
        //         if (payload_n != i)
        //         {
        //             //
        //             std::swap(engineState.scene.entities[payload_n],
        //             //           engineState.scene.entities[i]);
        //             if (sm_selectedEntityIndex == (int)i)
        //             {
        //                 sm_selectedEntityIndex = (int)payload_n;
        //                 sm_selectedEntity = smECS_GetEntityAtIndex(
        //                     smState.scene, sm_selectedEntityIndex);
        //             }
        //             else if (sm_selectedEntityIndex ==
        //             (int)payload_n)
        //             {
        //                 sm_selectedEntityIndex = (int)i;
        //                 sm_selectedEntity = smECS_GetEntityAtIndex(
        //                     smState.scene, sm_selectedEntityIndex);
        //             }
        //         }
        //     }
        //     smImGui_EndDragDropTarget();
        // }

        smImGui_PopID();
    }

    if (smImGui_BeginPopupContextWindow())
    {
        if (smImGui_MenuItem("Add Entity"))
        {
            smEntityID   ent = smECS_AddEntity(smState.scene);
            smTransform* trans =
                SM_ECS_ASSIGN(smState.scene, ent, smTransform);

            // Avert thine eyes
            trans->position[0] = 0.0f;
            trans->position[1] = 0.0f;
            trans->position[2] = 0.0f;
            trans->rotation[0] = 0.0f;
            trans->rotation[1] = 0.0f;
            trans->rotation[2] = 0.0f;
            trans->scale[0] = 1.0f;
            trans->scale[1] = 1.0f;
            trans->scale[2] = 1.0f;

            smName* name = SM_ECS_ASSIGN(smState.scene, ent, smName);
            strcpy(name->name, "Entity");
        }
        smImGui_EndPopup();
    }

    smImGui_End();
}

void smEditor_DrawInspector()
{
    if (sm_selectedEntityIndex == -1)
        return;

    smImGui_Begin("Inspector");

    smRegistry_DrawComponents(sm_selectedEntity);

    if (smInput_GetKeyDown(SM_KEY_DELETE))
    {
        smECS_DestroyEntity(smState.scene, sm_selectedEntity);
    }

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
                SM_ECS_ASSIGN_N(
                    smState.scene, sm_selectedEntity,
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
    smJson j = smJson_Create();
    smRegistry_SaveComponents(j);

    smJson_SaveToFile(j, scene);

    smJson_Destroy(j);
}

void smEditor_LoadScene(const char* scene)
{
    smJson j = smJson_LoadFromFile(scene);

    smPhysics3D_ClearWorld();

    smECS_ClearScene(smState.scene);

    smRegistry_LoadComponents(j);

    smECS_StartEditorStartSystems();

    smJson_Destroy(j);
}

void smEditor_DrawTray()
{
    smImGui_Begin("Tray");

    smImGui_InputText("Scene Name", sm_sceneName,
                      sizeof(sm_sceneName), 0);

    if (!sm_playing)
    {
        if (smImGui_Button("Play"))
        {
            smEditor_SaveScene(sm_sceneName);
            sm_playing = true;
            smECS_StartStartSystems();

            glfwSetInputMode(smState.window->window, GLFW_CURSOR,
                           GLFW_CURSOR_DISABLED);
        }

        if (smImGui_Button("Save"))
        {
            smEditor_SaveScene(sm_sceneName);
        }

        if (smImGui_Button("Load"))
        {
            smEditor_LoadScene(sm_sceneName);
        }
    }
    else
    {
        if (smImGui_Button("Stop") || smInput_GetKeyDown(SM_KEY_ESCAPE))
        {
            SM_ECS_ITER_START(smState.scene,
                              SM_ECS_COMPONENT_TYPE(smRigidbody2D))
            {
                smRigidbody2D* rigid =
                    SM_ECS_GET(smState.scene, _entity, smRigidbody2D);
                smCollider2D* col =
                    SM_ECS_GET(smState.scene, _entity, smCollider2D);

                if (col != NULL)
                {
                    b2DestroyShape(col->shapeID, false);
                    col->shapeID = b2_nullShapeId;
                }

                b2DestroyBody(rigid->bodyID);
                rigid->bodyID = b2_nullBodyId;
            }
            SM_ECS_ITER_END();
            sm_playing = false;
            smEditor_LoadScene(sm_sceneName);

            glfwSetInputMode(smState.window->window, GLFW_CURSOR,
                             GLFW_CURSOR_NORMAL);
        }
    }

    smImGui_End();
}
