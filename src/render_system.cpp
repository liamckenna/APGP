#include "render_system.h"
#include <iostream>

RenderSystem::RenderSystem(ResourceManager& rm, ShaderManager& sm)
    : resource_manager(rm), shader_manager(sm) {
}

void RenderSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, float delta_time) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint lightBlockIndex = glGetUniformBlockIndex(shader_manager.GetActiveShader(), "LightBlock");
    glUniformBlockBinding(shader_manager.GetActiveShader(), lightBlockIndex, LIGHT_BINDING_POINT);

    
    for (auto entity : component_manager.GetEntitiesWithComponent<CameraComponent>()) {
        if (component_manager.HasComponent<PrimaryCameraComponent>(entity)) {
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);
            auto& camera = component_manager.GetComponent<CameraComponent>(entity);
            if (transform.stale) {
                camera.view = glm::lookAt(transform.position, glm::vec3(0.f),
                    transform.orientation * glm::vec3(0.f, 1.f, 0.f));
                shader_manager.SetUniform("view", camera.view);
                transform.stale = false;
            }
            if (camera.stale) {
                if (camera.perspective) {
                    camera.projection = glm::perspective(glm::radians(camera.fov), camera.aspect_ratio, camera.near, camera.far);
                }
                else {
                    float size = camera.fov * 0.5f;
                    camera.projection = glm::ortho(-size * camera.aspect_ratio, size * camera.aspect_ratio,
                        -size, size, camera.near, camera.far);
                }
                shader_manager.SetUniform("projection", camera.projection);
                camera.stale = false;
            }
        }
    }


    for (auto entity : component_manager.GetEntitiesWithComponents<MeshComponent, TransformComponent>()) {
            auto& meshComp = component_manager.GetComponent<MeshComponent>(entity);
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);

            Mesh& mesh = resource_manager.GetMesh(meshComp.mesh_name);

            if (!meshComp.active) continue; // Skip inactive meshes

            if (transform.stale) {
                meshComp.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.f));
                transform.stale = false;
            }
            shader_manager.SetUniform("model", meshComp.model);

            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
}
