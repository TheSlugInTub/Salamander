#include <salamander/json_api.h>
#include <salamander/ecs_api.h>
#include <salamander/state.h>
#include <salamander/imgui_layer.h>

// Function pointer types for component operations
typedef void (*smComponentDrawFunc)(void* component);
typedef Json (*smComponentSaveFunc)(void* component);
typedef void (*smComponenLoadFunc)(void* component, const Json json);

// Maximum number of registered components
#define SM_MAX_REGISTERED_COMPONENTS 200
#define SM_MAX_COMPONENT_TYPE_LENGTH 100

typedef struct
{
    smComponentDrawFunc drawFunc;
    smComponentSaveFunc saveFunc;
    smComponenLoadFunc  loadFunc;
    char                componentType[SM_MAX_COMPONENT_TYPE_LENGTH];
    int                 size;
} smComponentRegistration;

typedef struct
{
    smComponentRegistration
        registrations[SM_MAX_REGISTERED_COMPONENTS];
    int registrationCount;
} smComponentRegistry;

// Global registry instance
extern smComponentRegistry g_componentRegistry;

// Function to register a component
void smRegistry_RegisterComponent(const char* componentType, int size,
                                  smComponentDrawFunc drawFunc,
                                  smComponentSaveFunc saveFunc,
                                  smComponenLoadFunc  loadFunc);

// Draw registered components for a selected entity
void smRegistry_DrawComponents(EntityID selectedEntity);

// Save all components to JSON
void smRegistry_SaveComponents(Json jsonObj);

// Load all components from JSON
void smRegistry_LoadComponents(Json jsonObj);

// Macro to simplify component registration (optional, but
// recommended)
#define SM_REGISTER_COMPONENT(ComponentType, DrawFunc, SaveFunc, \
                              LoadFunc)                          \
    smRegistry_RegisterComponent(                                \
        #ComponentType, sizeof(ComponentType),                   \
        (smComponentDrawFunc)DrawFunc,                           \
        (smComponentSaveFunc)SaveFunc, (smComponenLoadFunc)LoadFunc)
