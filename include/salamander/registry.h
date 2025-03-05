#include <stdlib.h>
#include <string.h>
#include <salamander/json_api.h>
#include <salamander/ecs_api.h>
#include <salamander/state.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui/cimgui.h>
#include <cimgui/cimgui_impl.h>

// Function pointer types for component operations
typedef void (*ComponentDrawFunc)(void* component);
typedef Json (*ComponentSaveFunc)(void* component);
typedef void (*ComponentLoadFunc)(void* component, const Json json);

// Maximum number of registered components
#define MAX_REGISTERED_COMPONENTS 200
#define MAX_COMPONENT_TYPE_LENGTH 100

typedef struct
{
    ComponentDrawFunc drawFunc;
    ComponentSaveFunc saveFunc;
    ComponentLoadFunc loadFunc;
    char              componentType[MAX_COMPONENT_TYPE_LENGTH];
    int               size;
} ComponentRegistration;

typedef struct
{
    ComponentRegistration registrations[MAX_REGISTERED_COMPONENTS];
    int                   registrationCount;
} ComponentRegistry;

// Global registry instance
static ComponentRegistry g_componentRegistry = {0};

// Function to register a component
int Registry_RegisterComponent(const char* componentType, int size,
                               ComponentDrawFunc drawFunc,
                               ComponentSaveFunc saveFunc,
                               ComponentLoadFunc loadFunc)
{
    if (g_componentRegistry.registrationCount >=
        MAX_REGISTERED_COMPONENTS)
    {
        return 0; // Registration failed
    }

    ComponentRegistration* reg =
        &g_componentRegistry
             .registrations[g_componentRegistry.registrationCount];

    strncpy(reg->componentType, componentType,
            MAX_COMPONENT_TYPE_LENGTH - 1);
    reg->drawFunc = drawFunc;
    reg->saveFunc = saveFunc;
    reg->loadFunc = loadFunc;
    reg->size = size;

    return g_componentRegistry.registrationCount++;
}

// Draw registered components for a selected entity
void Registry_DrawComponents(EntityID selectedEntity)
{
    for (int i = 0; i < g_componentRegistry.registrationCount; i++)
    {
        ComponentRegistration* reg =
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

            ImVec2 vec = {-1.0f, 0.0f};
            if (igButton(buttonLabel, vec))
            {
                ECS_REMOVE_N(smState.scene, selectedEntity,
                             reg->componentType, reg->size);
            }
        }
    }
}

// Add component menu for selected entity
void Registry_AddComponentMenu(EntityID selectedEntity)
{
    for (int i = 0; i < g_componentRegistry.registrationCount; i++)
    {
        ComponentRegistration* reg =
            &g_componentRegistry.registrations[i];
        void* existingComponent =
            ECS_GET_N(smState.scene, selectedEntity,
                      reg->componentType, reg->size);

        if (igMenuItem_Bool(reg->componentType, NULL, false,
                            !existingComponent))
        {
            ECS_ASSIGN_N(smState.scene, selectedEntity,
                         reg->componentType, reg->size);
        }
    }
}

// Save all components to JSON
void Registry_SaveComponents(Json jsonObj)
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
            ComponentRegistration* reg =
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
void Registry_LoadComponents(Json jsonObj)
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
                char* type;
                Json_LoadString(compJson, "type", type);

                for (int k = 0;
                     k < g_componentRegistry.registrationCount; k++)
                {
                    ComponentRegistration* reg =
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

// Macro to simplify component registration (optional, but
// recommended)
#define REGISTER_COMPONENT(ComponentType, DrawFunc, SaveFunc,     \
                           LoadFunc)                              \
    Registry_RegisterComponent(                                   \
        #ComponentType, sizeof(ComponentType),                    \
        (ComponentDrawFunc)DrawFunc, (ComponentSaveFunc)SaveFunc, \
        (ComponentLoadFunc)LoadFunc)
