#include "light_system.h"
#include "flat_light.h"
#include <iostream>

LightSystem::LightSystem() {
    Init();
}

void LightSystem::Init() {
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(FlatLight) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
}
void LightSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, float delta_time) {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);

    auto view = component_manager.GetEntitiesWithComponents<LightComponent, TransformComponent>();
    int index = 0;

    for (auto entity : view) {
        auto& light = component_manager.GetComponent<LightComponent>(entity);
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);

        if (!light.stale) continue;

        FlatLight fl;
        fl.position = transform.position;
        fl.intensity = light.intensity;
        fl.color = light.color;
        fl.enabled = light.active ? 1 : 0;  // **Mark inactive instead of deleting**

        std::size_t offset = index * sizeof(FlatLight);

        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(FlatLight), &fl);

        light.stale = false;
        index++;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}
