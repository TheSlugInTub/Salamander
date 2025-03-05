#include <salamander/ecs_api.h>
#include <salamander/ecs.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstring>

// Store the Scene struct as an opaque pointer
struct Scene_t {
    Scene cppScene;
    std::unordered_map<ComponentTypeID, int> componentTypeMap; // Maps our ComponentTypeID to internal IDs
    int nextInternalId;
    
    Scene_t() : nextInternalId(1) {} // Start at 1 to avoid conflicts with 0
};

// Custom structure for the entity iterator
struct EntityIterator_t {
    Scene* pScene;
    std::vector<int> internalComponentTypes;
    size_t currentIndex;
    std::vector<EntityID> matchingEntities;
    bool iterateAll;
};

extern "C" {

// FNV-1a hash function for strings
unsigned int ECS_HashString(const char* str) {
    unsigned int hash = 2166136261u; // FNV offset basis
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 16777619u; // FNV prime
    }
    return hash;
}

SceneHandle ECS_CreateScene(void) {
    SceneHandle handle = new Scene_t();
    return handle;
}

void ECS_DestroyScene(SceneHandle scene) {
    if (scene) {
        scene->cppScene.Clear();
        delete scene;
    }
}

void ECS_ClearScene(SceneHandle scene) {
    if (scene) {
        scene->cppScene.Clear();
        scene->componentTypeMap.clear();
        scene->nextInternalId = 1;
    }
}

int ECS_EntityCount(SceneHandle scene) {
    if (scene) {
        return scene->cppScene.entities.size();
    }
    return 0;
}

EntityID ECS_AddEntity(SceneHandle scene) {
    if (!scene) return ECS_INVALID_ENTITY;
    return scene->cppScene.AddEntity();
}

void ECS_DestroyEntity(SceneHandle scene, EntityID entity) {
    if (scene && IsEntityValid(entity)) {
        scene->cppScene.DestroyEntity(entity);
    }
}

EntityID ECS_CloneEntity(SceneHandle scene, EntityID sourceEntity) {
    if (!scene || !IsEntityValid(sourceEntity)) return ECS_INVALID_ENTITY;
    return scene->cppScene.CloneEntity(sourceEntity);
}

bool ECS_IsEntityValid(EntityID entity) {
    return IsEntityValid(entity);
}

EntityID ECS_GetEntityAtIndex(SceneHandle scene, int index) {
    return scene->cppScene.entities[index].id;
}

// Get or create the internal component ID for a component type
static int GetOrCreateInternalComponentId(SceneHandle scene, ComponentTypeID externalTypeId, size_t componentSize) {
    // Check if we already have this component type registered
    auto it = scene->componentTypeMap.find(externalTypeId);
    if (it != scene->componentTypeMap.end()) {
        return it->second;
    }
    
    // Otherwise, create a new internal ID
    int internalId = scene->nextInternalId++;
    scene->componentTypeMap[externalTypeId] = internalId;
    
    // Ensure the component pools are large enough
    if (scene->cppScene.componentPools.size() <= internalId) {
        scene->cppScene.componentPools.resize(internalId + 1, nullptr);
    }
    
    // Create the component pool if it doesn't exist
    if (!scene->cppScene.componentPools[internalId]) {
        scene->cppScene.componentPools[internalId] = new ComponentPool(componentSize);
    }
    
    return internalId;
}

void* ECS_AssignComponent(SceneHandle scene, EntityID entity, ComponentTypeID componentTypeId, size_t componentSize) {
    if (!scene || !IsEntityValid(entity)) return NULL;
    
    // Get the internal component ID, creating it if necessary
    int internalId = GetOrCreateInternalComponentId(scene, componentTypeId, componentSize);
    
    Scene* pScene = &scene->cppScene;
    EntityIndex entityIndex = GetEntityIndex(entity);
    
    // Check if the entity already has this component
    if (entityIndex < pScene->entities.size() && 
        pScene->entities[entityIndex].mask.test(internalId)) {
        // Component already exists, just return the pointer
        return pScene->componentPools[internalId]->get(entityIndex);
    }
    
    // Component doesn't exist, create it
    void* componentMemory = pScene->componentPools[internalId]->get(entityIndex);
    
    // Initialize the memory to zero
    std::memset(componentMemory, 0, componentSize);
    
    // Set the component bit
    pScene->entities[entityIndex].mask.set(internalId);
    
    return componentMemory;
}

void* ECS_GetComponent(SceneHandle scene, EntityID entity, ComponentTypeID componentTypeId) {
    if (!scene || !IsEntityValid(entity)) return NULL;
    
    // Check if we know this component type
    auto it = scene->componentTypeMap.find(componentTypeId);
    if (it == scene->componentTypeMap.end()) {
        return NULL; // Component type not registered
    }
    
    int internalId = it->second;
    Scene* pScene = &scene->cppScene;
    EntityIndex entityIndex = GetEntityIndex(entity);
    
    // Check if the entity has this component
    if (entityIndex >= pScene->entities.size() || 
        !pScene->entities[entityIndex].mask.test(internalId) ||
        internalId >= pScene->componentPools.size() ||
        !pScene->componentPools[internalId]) {
        return NULL;
    }
    
    return pScene->componentPools[internalId]->get(entityIndex);
}

void ECS_RemoveComponent(SceneHandle scene, EntityID entity, ComponentTypeID componentTypeId) {
    if (!scene || !IsEntityValid(entity)) return;
    
    // Check if we know this component type
    auto it = scene->componentTypeMap.find(componentTypeId);
    if (it == scene->componentTypeMap.end()) {
        return; // Component type not registered
    }
    
    int internalId = it->second;
    Scene* pScene = &scene->cppScene;
    EntityIndex entityIndex = GetEntityIndex(entity);
    
    // Check if the entity is valid
    if (entityIndex >= pScene->entities.size() || pScene->entities[entityIndex].id != entity) {
        return;
    }
    
    // Reset the component bit
    pScene->entities[entityIndex].mask.reset(internalId);
}

void ECS_AddSystem(SystemFunction system, bool isEditorSystem, bool isStartSystem) {
    AddSystem(system, isEditorSystem, isStartSystem);
}

void ECS_UpdateSystems(void) {
    UpdateSystems();
}

void ECS_StartStartSystems(void) {
    StartStartSystems();
}

void ECS_UpdateEditorSystems(void) {
    UpdateEditorSystems();
}

void ECS_StartEditorStartSystems(void) {
    StartEditorStartSystems();
}

EntityIteratorHandle ECS_CreateEntityIterator(SceneHandle scene, const ComponentTypeID* componentTypeIds, int componentCount) {
    if (!scene) return NULL;
    
    EntityIterator_t* iterator = new EntityIterator_t();
    iterator->pScene = &scene->cppScene;
    iterator->currentIndex = 0;
    iterator->iterateAll = false;
    
    // Map external component types to internal IDs
    for (int i = 0; i < componentCount; i++) {
        auto it = scene->componentTypeMap.find(componentTypeIds[i]);
        if (it != scene->componentTypeMap.end()) {
            iterator->internalComponentTypes.push_back(it->second);
        }
    }
    
    // If any component type wasn't found, no entities will match
    if (iterator->internalComponentTypes.size() != componentCount) {
        return iterator; // Will return no entities
    }
    
    // Build component mask
    ComponentMask mask;
    for (int internalId : iterator->internalComponentTypes) {
        mask.set(internalId);
    }
    
    // Pre-collect matching entities for iteration
    for (size_t i = 0; i < iterator->pScene->entities.size(); i++) {
        const Scene::EntityDesc& desc = iterator->pScene->entities[i];
        if (IsEntityValid(desc.id) && mask == (mask & desc.mask)) {
            iterator->matchingEntities.push_back(desc.id);
        }
    }
    
    return iterator;
}

EntityIteratorHandle ECS_CreateAllEntityIterator(SceneHandle scene) {
    if (!scene) return NULL;
    
    EntityIterator_t* iterator = new EntityIterator_t();
    iterator->pScene = &scene->cppScene;
    iterator->currentIndex = 0;
    iterator->iterateAll = true;
    
    // Pre-collect valid entities
    for (size_t i = 0; i < iterator->pScene->entities.size(); i++) {
        const Scene::EntityDesc& desc = iterator->pScene->entities[i];
        if (IsEntityValid(desc.id)) {
            iterator->matchingEntities.push_back(desc.id);
        }
    }
    
    return iterator;
}

EntityID ECS_IteratorNext(EntityIteratorHandle iterator) {
    if (!iterator) return ECS_INVALID_ENTITY;
    
    if (iterator->currentIndex < iterator->matchingEntities.size()) {
        return iterator->matchingEntities[iterator->currentIndex++];
    }
    
    return ECS_INVALID_ENTITY;
}

void ECS_DestroyEntityIterator(EntityIteratorHandle iterator) {
    delete iterator;
}

} // extern "C"
