#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdbool.h>

// Opaque pointer types
typedef struct smScene_t*  smSceneHandle;
typedef unsigned long long smEntityID;

// Constants
#define SM_ECS_MAX_COMPONENTS 200
#define SM_ECS_MAX_ENTITIES   1000
#define SM_ECS_INVALID_ENTITY ((smEntityID) - 1)

// Scene management
smSceneHandle smECS_CreateScene(void);
void          smECS_DestroyScene(smSceneHandle scene);
void          smECS_ClearScene(smSceneHandle scene);
int           smECS_EntityCount(smSceneHandle);

// Entity management
smEntityID smECS_AddEntity(smSceneHandle scene);
void smECS_DestroyEntity(smSceneHandle scene, smEntityID entity);
smEntityID smECS_CloneEntity(smSceneHandle scene,
                             smEntityID    sourceEntity);
bool       smECS_IsEntityValid(smEntityID entity);
smEntityID smECS_GetEntityAtIndex(smSceneHandle scene, int index);

// Component management
// We now automatically register components by size and hash
typedef size_t smComponentTypeID;

// Get a component type ID from a component type name (using string
// hashing) This is a macro that generates a unique ID for each
// component type at compile time
#define SM_ECS_COMPONENT_TYPE(component_type)                 \
    ((smComponentTypeID)((sizeof(component_type) << 16) |     \
                         (smECS_HashString(#component_type) & \
                          0xFFFF)))

#define SM_ECS_COMPONENT_TYPE_N(component_name, size)        \
    ((smComponentTypeID)((size << 16) |                      \
                         (smECS_HashString(component_name) & \
                          0xFFFF)))

// String hashing function (exposed for the macro above)
unsigned int smECS_HashString(const char* str);

// Generic component operations
void* smECS_AssignComponent(smSceneHandle scene, smEntityID entity,
                            smComponentTypeID componentTypeId,
                            size_t            componentSize);
void* smECS_GetComponent(smSceneHandle scene, smEntityID entity,
                         smComponentTypeID componentTypeId);
void  smECS_RemoveComponent(smSceneHandle scene, smEntityID entity,
                            smComponentTypeID componentTypeId);

// Convenience macros for component operations with automatic type
// handling
#define SM_ECS_ASSIGN(scene, entity, component_type)          \
    ((component_type*)smECS_AssignComponent(                  \
        scene, entity, SM_ECS_COMPONENT_TYPE(component_type), \
        sizeof(component_type)))

#define SM_ECS_ASSIGN_N(scene, entity, component_name, size) \
    (smECS_AssignComponent(                                  \
        scene, entity,                                       \
        SM_ECS_COMPONENT_TYPE_N(component_name, size), size))

#define SM_ECS_GET(scene, entity, component_type) \
    ((component_type*)smECS_GetComponent(         \
        scene, entity, SM_ECS_COMPONENT_TYPE(component_type)))

#define SM_ECS_GET_N(scene, entity, component_name, size) \
    (smECS_GetComponent(                                  \
        scene, entity,                                    \
        SM_ECS_COMPONENT_TYPE_N(component_name, size)))

#define SM_ECS_REMOVE(scene, entity, component_type) \
    smECS_RemoveComponent(scene, entity,             \
                          SM_ECS_COMPONENT_TYPE(component_type))

#define SM_ECS_REMOVE_N(scene, entity, component_name, size) \
    smECS_RemoveComponent(                                   \
        scene, entity,                                       \
        SM_ECS_COMPONENT_TYPE_N(component_name, size))

// System management
typedef void (*smSystemFunction)();
void smECS_AddSystem(smSystemFunction system, bool isEditorSystem,
                     bool isStartSystem);
void smECS_UpdateSystems(void);
void smECS_StartStartSystems(void);
void smECS_UpdateEditorSystems(void);
void smECS_StartEditorStartSystems(void);

// Entity iteration
typedef struct smEntityIterator_t* smEntityIteratorHandle;

// Creates an iterator for entities with all specified component types
smEntityIteratorHandle
                       smECS_CreateEntityIterator(smSceneHandle            scene,
                                                  const smComponentTypeID* componentTypeIds,
                                                  int                      componentCount);
smEntityIteratorHandle smECS_CreateAllEntityIterator(
    smSceneHandle scene); // Iterates all valid entities
smEntityID smECS_IteratorNext(
    smEntityIteratorHandle iterator); // Returns smECS_INVALID_ENTITY
                                      // when no more entities
void smECS_DestroyEntityIterator(smEntityIteratorHandle iterator);

// Helper macro for creating iterators with specific component types
#define SM_ECS_ITER_START(scene, ...)                              \
    do {                                                           \
        smComponentTypeID      _types[] = {__VA_ARGS__};           \
        smEntityIteratorHandle _iter = smECS_CreateEntityIterator( \
            scene, _types, sizeof(_types) / sizeof(_types[0]));    \
        smEntityID _entity;                                        \
        while ((_entity = smECS_IteratorNext(_iter)) !=            \
               SM_ECS_INVALID_ENTITY)                              \
        {

#define SM_ECS_ITER_END()               \
    }                                   \
    smECS_DestroyEntityIterator(_iter); \
    }                                   \
    while (0)

#ifdef __cplusplus
}
#endif
