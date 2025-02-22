#pragma once
#include <GL/glew.h>
#include <vector>
#include "system.h"
#include "entity_manager.h"
#include "component_manager.h"
#include "shader_manager.h"
#include "ecs_components.h"
#include "util.h"

class LightSystem : public System {
    GLuint ubo;
    GLuint binding_point = LIGHT_BINDING_POINT;
    ShaderManager& shader_manager;

public:
    LightSystem(ShaderManager& sm);
    void Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time);
    void Init();
};