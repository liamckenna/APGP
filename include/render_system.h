#pragma once
#include <GL/glew.h>
#include "system.h"
#include "entity_manager.h"
#include "component_manager.h"
#include "ecs_components.h"
#include "resource_manager.h"
#include "shader_manager.h"

class RenderSystem : public System {
    ResourceManager& resource_manager;
    ShaderManager& shader_manager;

public:
    RenderSystem(ResourceManager& rm, ShaderManager& sm);
    void Update(EntityManager& entity_manager, ComponentManager& component_manager, float delta_time);
};
