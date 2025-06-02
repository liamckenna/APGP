#include "light_system.h"
#include "flat_light.h"
#include <iostream>
#include "system_manager.h"
#include "render_system.h"

LightSystem::LightSystem(ShaderManager& sm) : shader_manager(sm) {
    Init();
}

void LightSystem::Init() {
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(FlatLight) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);


}

void LightSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);

    auto lights = component_manager.GetEntitiesWithComponents<LightComponent, TransformComponent>();
    int index = 0;

    for (auto entity : lights) {
        auto& light = component_manager.GetComponent<LightComponent>(entity);
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);

        if (!light.stale) continue;

        FlatLight fl;
        fl.enabled = 1;
        fl.index = index;
        fl.intensity = light.intensity;
        fl.range = light.range;
        fl.position = transform.position;
        fl.color = light.color;

        std::size_t offset = index * sizeof(FlatLight);

        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(FlatLight), &fl);

        light.stale = false;
        index++;
    }


    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}
