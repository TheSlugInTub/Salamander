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
void smRegistry_DrawComponents(EntityID selectedEntity)
{
    for (int i = 0; i < g_componentRegistry.registrationCount; i++)
    {
        smComponentRegistration* reg =
            &g_componentRegistry.registrations[i];
        void* component = ECS_GET_N(smState.scene, selectedEntity,
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
                ECS_REMOVE_N(smState.scene, selectedEntity,
                             reg->componentType, reg->size);
            }
        }
    }
}

// Save all components to JSON
void smRegistry_SaveComponents(Json jsonObj)
{
    for (int i = 0; i < ECS_EntityCount(smState.scene); i++)
    {
        EntityID ent = ECS_GetEntityAtIndex(smState.scene, i);
        if (!ECS_IsEntityValid(ent))
            continue;

        Json entityData = Json_Create();

        for (int j = 0; j < g_componentRegistry.registrationCount;
             j++)
        {
            smComponentRegistration* reg =
                &g_componentRegistry.registrations[j];
            void* component = ECS_GET_N(
                smState.scene, ent, reg->componentType, reg->size);

            if (component && reg->saveFunc)
            {
                Json compData = reg->saveFunc(component);
                Json_SaveString(compData, "type", reg->componentType);
                Json_PushBack(entityData, compData);
                Json_Destroy(compData);
            }
        }

        Json_PushBack(jsonObj, entityData);
        Json_Destroy(entityData);
    }
}

// Load all components from JSON
void smRegistry_LoadComponents(Json jsonObj)
{
    for (size_t i = 0; i < Json_GetJsonArraySize(jsonObj); i++)
    {
        Json     entityJson = Json_GetJsonAtIndex(jsonObj, i);
        EntityID newEnt = ECS_AddEntity(smState.scene);

        for (size_t j = 0; j < Json_GetJsonArraySize(entityJson); j++)
        {
            Json compJson = Json_GetJsonAtIndex(entityJson, j);

            if (Json_HasKey(compJson, "type"))
            {
                char type[128] = {};
                Json_LoadString(compJson, "type", type);

                for (int k = 0;
                     k < g_componentRegistry.registrationCount; k++)
                {
                    smComponentRegistration* reg =
                        &g_componentRegistry.registrations[k];

                    if (strcmp(type, reg->componentType) == 0 &&
                        reg->loadFunc)
                    {
                        void* component = ECS_ASSIGN_N(
                            smState.scene, newEnt, reg->componentType,
                            reg->size);
                        reg->loadFunc(component, compJson);
                        break;
                    }
                }
            }

            Json_Destroy(compJson);
        }

        Json_Destroy(entityJson);
    }
}
