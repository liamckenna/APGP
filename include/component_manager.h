#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <any>
#include "component_pool.h"
#include "ecs_components.h"

using Entity = uint32_t;

//component manager keeps track of all the component pools and what data type they track

class ComponentManager {

    std::unordered_map<std::type_index, std::any> component_pools;

public:

    //adds component of type T to an entity
    template<typename T>
    void AddComponent(Entity entity, const T& component) {
        if (component_pools.find(typeid(T)) == component_pools.end()) {
            component_pools[typeid(T)] = ComponentPool<T>();
        }
        std::any_cast<ComponentPool<T>&>(component_pools[typeid(T)]).Add(entity, component);
    }
    
    //gets a component of type T of an entity
    template<typename T>
    T& GetComponent(Entity entity) {
        return std::any_cast<ComponentPool<T>&>(component_pools[typeid(T)]).Get(entity);
    }

    //removes a component of type T from an entity
    template<typename T>
    void RemoveComponent(Entity entity) {
        if (component_pools.find(typeid(T)) != component_pools.end()) {
            std::any_cast<ComponentPool<T>&>(component_pools[typeid(T)]).Remove(entity);
        }
    }
    
    //check if an entity has component of type T
    template<typename T>
    bool HasComponent(Entity entity) {
        return component_pools.find(typeid(T)) != component_pools.end() &&
            std::any_cast<ComponentPool<T>&>(component_pools[typeid(T)]).Has(entity);
    }

    //gets all entities with a component of type T
    template<typename T>
    std::vector<Entity>& GetEntitiesWithComponent() {
        return std::any_cast<ComponentPool<T>&>(component_pools[typeid(T)]).GetAllEntities();
    }
};
