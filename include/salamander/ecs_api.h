#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdbool.h>

// Opaque pointer types
typedef struct Scene_t*    SceneHandle;
typedef unsigned long long EntityID;

// Constants
#define ECS_MAX_COMPONENTS 200
#define ECS_MAX_ENTITIES   1000
#define ECS_INVALID_ENTITY ((EntityID) - 1)

// Scene management
SceneHandle ECS_CreateScene(void);
void        ECS_DestroyScene(SceneHandle scene);
void        ECS_ClearScene(SceneHandle scene);
int         ECS_EntityCount(SceneHandle);

// Entity management
EntityID ECS_AddEntity(SceneHandle scene);
void     ECS_DestroyEntity(SceneHandle scene, EntityID entity);
EntityID ECS_CloneEntity(SceneHandle scene, EntityID sourceEntity);
bool     ECS_IsEntityValid(EntityID entity);
EntityID ECS_GetEntityAtIndex(SceneHandle scene, int index);

// Component management
// We now automatically register components by size and hash
typedef size_t ComponentTypeID;

// Get a component type ID from a component type name (using string
// hashing) This is a macro that generates a unique ID for each
// component type at compile time
#define ECS_COMPONENT_TYPE(component_type)              \
    ((ComponentTypeID)((sizeof(component_type) << 16) | \
                       (ECS_HashString(#component_type) & 0xFFFF)))

#define ECS_COMPONENT_TYPE_N(component_name, size) \
    ((ComponentTypeID)((size << 16) |              \
                       (ECS_HashString(component_name) & 0xFFFF)))

// String hashing function (exposed for the macro above)
unsigned int ECS_HashString(const char* str);

// Generic component operations
void* ECS_AssignComponent(SceneHandle scene, EntityID entity,
                          ComponentTypeID componentTypeId,
                          size_t          componentSize);
void* ECS_GetComponent(SceneHandle scene, EntityID entity,
                       ComponentTypeID componentTypeId);
void  ECS_RemoveComponent(SceneHandle scene, EntityID entity,
                          ComponentTypeID componentTypeId);

// Convenience macros for component operations with automatic type
// handling
#define ECS_ASSIGN(scene, entity, component_type)          \
    ((component_type*)ECS_AssignComponent(                 \
        scene, entity, ECS_COMPONENT_TYPE(component_type), \
        sizeof(component_type)))

#define ECS_ASSIGN_N(scene, entity, component_name, size)            \
    (ECS_AssignComponent(scene, entity,                              \
                         ECS_COMPONENT_TYPE_N(component_name, size), \
                         size))

#define ECS_GET(scene, entity, component_type) \
    ((component_type*)ECS_GetComponent(        \
        scene, entity, ECS_COMPONENT_TYPE(component_type)))

#define ECS_GET_N(scene, entity, component_name, size) \
    (ECS_GetComponent(scene, entity,                   \
                      ECS_COMPONENT_TYPE_N(component_name, size)))

#define ECS_REMOVE(scene, entity, component_type) \
    ECS_RemoveComponent(scene, entity,            \
                        ECS_COMPONENT_TYPE(component_type))

#define ECS_REMOVE_N(scene, entity, component_name, size) \
    ECS_RemoveComponent(scene, entity,                    \
                        ECS_COMPONENT_TYPE_N(component_name, size))

// System management
typedef void (*SystemFunction)(void);
void ECS_AddSystem(SystemFunction system, bool isEditorSystem,
                   bool isStartSystem);
void ECS_UpdateSystems(void);
void ECS_StartStartSystems(void);
void ECS_UpdateEditorSystems(void);
void ECS_StartEditorStartSystems(void);

// Entity iteration
typedef struct EntityIterator_t* EntityIteratorHandle;

// Creates an iterator for entities with all specified component types
EntityIteratorHandle
                     ECS_CreateEntityIterator(SceneHandle            scene,
                                              const ComponentTypeID* componentTypeIds,
                                              int                    componentCount);
EntityIteratorHandle ECS_CreateAllEntityIterator(
    SceneHandle scene); // Iterates all valid entities
EntityID ECS_IteratorNext(
    EntityIteratorHandle
        iterator); // Returns ECS_INVALID_ENTITY when no more entities
void ECS_DestroyEntityIterator(EntityIteratorHandle iterator);

// Helper macro for creating iterators with specific component types
#define ECS_ITER_START(scene, ...)                              \
    do {                                                        \
        ComponentTypeID      _types[] = {__VA_ARGS__};          \
        EntityIteratorHandle _iter = ECS_CreateEntityIterator(  \
            scene, _types, sizeof(_types) / sizeof(_types[0])); \
        EntityID _entity;                                       \
        while ((_entity = ECS_IteratorNext(_iter)) !=           \
               ECS_INVALID_ENTITY)                              \
        {

#define ECS_ITER_END()                \
    }                                 \
    ECS_DestroyEntityIterator(_iter); \
    }                                 \
    while (0)

#ifdef __cplusplus
}
#endif
