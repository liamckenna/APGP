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

    // Gets all entities that have *all* components of types T...
    template<typename... T>
    std::vector<Entity> GetEntitiesWithComponents() {
        std::vector<Entity> result;
        if constexpr (sizeof...(T) == 0) return result; // Return empty if no types provided

        // Get the first component's entity list as a base
        auto& firstPool = std::any_cast<ComponentPool<std::tuple_element_t<0, std::tuple<T...>>>&>(
            component_pools[typeid(std::tuple_element_t<0, std::tuple<T...>>)]
        );
        result = firstPool.GetAllEntities();

        // Filter entities to only those that have *all* components
        for (Entity entity : result) {
            bool hasAll = ((component_pools.find(typeid(T)) != component_pools.end() &&
                std::any_cast<ComponentPool<T>&>(component_pools[typeid(T)]).Has(entity)) && ...);

            if (!hasAll) {
                result.erase(std::remove(result.begin(), result.end(), entity), result.end());
            }
        }

        return result;
    }

};
