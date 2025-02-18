#pragma once
#include "entity_manager.h"
#include "component_manager.h"
#include "system_manager.h"


//the entity component system (ecs) manager provides a simple API that abstracts manager functions

class ECSManager {
    EntityManager entity_manager;
    ComponentManager component_manager;
    SystemManager system_manager;

public:
    
    //creates a new entity instance
    Entity CreateEntity() {
        return entity_manager.CreateEntity();
    }

    //destroys an entity instance
    void DestroyEntity(Entity entity) {
        entity_manager.DestroyEntity(entity);
    }

    //adds a component to an entity
    template<typename T>
    void AddComponent(Entity entity, const T& component) {
        component_manager.AddComponent(entity, component);
    }

    //gets a component instance from an entity
    template<typename T>
    T& GetComponent(Entity entity) {
        return component_manager.GetComponent<T>(entity);
    }

    //removes a component from an entity
    template<typename T>
    void RemoveComponent(Entity entity) {
        component_manager.RemoveComponent<T>(entity);
    }

    //registers a new system
    template<typename T, typename... Args>
    T& AddSystem(Args&&... args) {
        return system_manager.AddSystem<T>(std::forward<Args>(args)...);
    }

    //run update function for all systems
    void Update(float delta_time) {
        system_manager.Update(entity_manager, component_manager, delta_time);
    }
};