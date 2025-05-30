#pragma once
#include <vector>
#include <memory>
#include "system.h"

//system manager handles all systems and calls their update function
class SystemManager {

    

public:
    
    std::vector<std::unique_ptr<System>> systems;
    //register a new system to be run every 
    template<typename T, typename... Args>
    T& AddSystem(Args&&... args) {
        systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return *static_cast<T*>(systems.back().get());
    }

    void Update(EntityManager& entity_manager, ComponentManager& component_manager, float delta_time) {
        if (systems.size() > 0) {
            for (int i = 1; i < systems.size(); i++) {
                systems[i]->Update(entity_manager, component_manager, *this, delta_time);
            }
            systems[0]->Update(entity_manager, component_manager, *this, delta_time); //render system must go last but be index 0
        }
        
    }
};