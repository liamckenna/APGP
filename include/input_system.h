#pragma once
#include "system.h"
#include "entity_manager.h"
#include "component_manager.h"
#include "input_manager.h"
#include "util.h"

class InputSystem : public System {
    InputManager& input_manager;

public:
    InputSystem(InputManager& im);
    void Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time);
};