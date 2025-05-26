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

        if (component_manager.HasComponent<PointLightComponent>(entity)) {
            auto& point = component_manager.GetComponent<PointLightComponent>(entity);
            if (point.stale) {
                glm::vec3 lightPos = transform.position;
                float near_plane = point.near;
                float far_plane = point.far;
                glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
                point.shadow_projection = shadowProj;
                glm::mat4 shadowTransforms[6] = {
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1,  0,  0), glm::vec3(0, -1,  0)), // +X
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0)), // -X
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0,  1,  0), glm::vec3(0,  0,  1)), // +Y
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0, -1,  0), glm::vec3(0,  0, -1)), // -Y
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0,  0,  1), glm::vec3(0, -1,  0)), // +Z
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0,  0, -1), glm::vec3(0, -1,  0))  // -Z
                };
                for (int i = 0; i < point.light_space_matrices->length(); i++) {
                    point.light_space_matrices[i] = shadowTransforms[i];
                }
                glBindFramebuffer(GL_FRAMEBUFFER, point.depth_map_fbo);
                shader_manager.SetUniform("default", "lightPos", lightPos);
                shader_manager.SetUniform("default", "far_plane", far_plane);
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);
                glViewport(0, 0, 1024, 1024); // Set viewport to match the cubemap resolution
                glClear(GL_DEPTH_BUFFER_BIT);
                for (int i = 0; i < 6; ++i) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, point.depth_map_cube, 0);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    // Set the shader and pass the correct light-space matrix
                    shader_manager.UseShader("shadow");
                    shader_manager.SetUniform("lightSpaceMatrix", shadowTransforms[i]);
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS); // Default: stores the smallest depth
                    static_cast<RenderSystem*>(system_manager.systems[0].get())->RenderMeshes(entity_manager, component_manager, system_manager, delta_time);
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
                glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
                point.stale = false;
            }
        }

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
