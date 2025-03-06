#include <salamander/registry.h>
#include <stdlib.h>
#include <string.h>

smComponentRegistry g_componentRegistry = {};

void smRegistry_RegisterComponent(const char* componentType, int size,
                                  smComponentDrawFunc drawFunc,
                                  smComponentSaveFunc saveFunc,
                                  smComponenLoadFunc  loadFunc)
{

    if (g_componentRegistry.registrationCount >=
        SM_MAX_REGISTERED_COMPONENTS)
    {
        return; // Registration failed
    }

    smComponentRegistration* reg =
        &g_componentRegistry
             .registrations[g_componentRegistry.registrationCount];

    strncpy(reg->componentType, componentType,
            SM_MAX_COMPONENT_TYPE_LENGTH - 1);
    reg->drawFunc = drawFunc;
    reg->saveFunc = saveFunc;
    reg->loadFunc = loadFunc;
    reg->size = size;

    g_componentRegistry.registrationCount++;
}

// Draw registered components for a selected entity
void smRegistry_DrawComponents(smEntityID selectedEntity)
{
    for (int i = 0; i < g_componentRegistry.registrationCount; i++)
    {
        smComponentRegistration* reg =
            &g_componentRegistry.registrations[i];
        void* component = SM_ECS_GET_N(smState.scene, selectedEntity,
                                       reg->componentType, reg->size);

        if (component && reg->drawFunc)
        {
            reg->drawFunc(component);

            // Create remove button
            char buttonLabel[128];
            snprintf(buttonLabel, sizeof(buttonLabel), "Remove##%s",
                     reg->componentType);

            if (strcmp(reg->componentType, "smName") != 0 &&
                smImGui_Button(buttonLabel))
            {
                SM_ECS_REMOVE_N(smState.scene, selectedEntity,
                                reg->componentType, reg->size);
            }
        }
    }
}

// Save all components to JSON
void smRegistry_SaveComponents(smJson jsonObj)
{
    for (int i = 0; i < smECS_EntityCount(smState.scene); i++)
    {
        smEntityID ent = smECS_GetEntityAtIndex(smState.scene, i);
        if (!smECS_IsEntityValid(ent))
            continue;

        smJson entityData = smJson_Create();

        for (int j = 0; j < g_componentRegistry.registrationCount;
             j++)
        {
            smComponentRegistration* reg =
                &g_componentRegistry.registrations[j];
            void* component = SM_ECS_GET_N(
                smState.scene, ent, reg->componentType, reg->size);

            if (component && reg->saveFunc)
            {
                smJson compData = reg->saveFunc(component);
                smJson_SaveString(compData, "type",
                                  reg->componentType);
                smJson_PushBack(entityData, compData);
                smJson_Destroy(compData);
            }
        }

        smJson_PushBack(jsonObj, entityData);
        smJson_Destroy(entityData);
    }
}

// Load all components from JSON
void smRegistry_LoadComponents(smJson jsonObj)
{
    for (size_t i = 0; i < smJson_GetsmJsonArraySize(jsonObj); i++)
    {
        smJson     entitysmJson = smJson_GetsmJsonAtIndex(jsonObj, i);
        smEntityID newEnt = smECS_AddEntity(smState.scene);

        for (size_t j = 0;
             j < smJson_GetsmJsonArraySize(entitysmJson); j++)
        {
            smJson compsmJson =
                smJson_GetsmJsonAtIndex(entitysmJson, j);

            if (smJson_HasKey(compsmJson, "type"))
            {
                char type[128] = {};
                smJson_LoadString(compsmJson, "type", type);

                for (int k = 0;
                     k < g_componentRegistry.registrationCount; k++)
                {
                    smComponentRegistration* reg =
                        &g_componentRegistry.registrations[k];

                    if (strcmp(type, reg->componentType) == 0 &&
                        reg->loadFunc)
                    {
                        void* component = SM_ECS_ASSIGN_N(
                            smState.scene, newEnt, reg->componentType,
                            reg->size);
                        reg->loadFunc(component, compsmJson);
                        break;
                    }
                }
            }

            smJson_Destroy(compsmJson);
        }

        smJson_Destroy(entitysmJson);
    }
}
