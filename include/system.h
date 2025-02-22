#pragma once
#include <vector>
#include <functional>
#include "entity_manager.h"
#include "component_manager.h"

//systems are universal processes that run on a per-tick basis on entities that have specific component types
class SystemManager;
class System {
public:
    virtual void Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) = 0;
};