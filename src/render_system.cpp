#include "render_system.h"
#include "system_manager.h"
#include <iostream>

RenderSystem::RenderSystem(ResourceManager& rm, ShaderManager& sm)
    : resource_manager(rm), shader_manager(sm) {
}

void RenderSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    if (shader_manager.GetActiveShader() != shader_manager.GetShaderID("default")) shader_manager.UseShader("default");
    std::vector<Entity>& point_light_entities = component_manager.GetEntitiesWithComponent<PointLightComponent>();
    for (int i = 0; i < point_light_entities.size(); i++) {
        PointLightComponent& point = component_manager.GetComponent<PointLightComponent>(point_light_entities[i]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, point.depth_map_cube);
        
        //shader_manager.SetUniform("shadowCubeMap", 1); // Use texture unit 1
        glUniform1i(glGetUniformLocation(shader_manager.GetActiveShader(), "shadowCubeMap"), 1);

    }
    

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint lightBlockIndex = glGetUniformBlockIndex(shader_manager.GetActiveShader(), "LightBlock");
    glUniformBlockBinding(shader_manager.GetActiveShader(), lightBlockIndex, LIGHT_BINDING_POINT);

    for (auto entity : component_manager.GetEntitiesWithComponent<CameraComponent>()) {
        if (component_manager.HasComponent<PrimaryCameraComponent>(entity)) {
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);
            auto& camera = component_manager.GetComponent<CameraComponent>(entity);
            if (transform.stale) {
                camera.view = glm::lookAt(transform.position, transform.position + (transform.orientation * glm::vec3(0, 0, -1)),
                    transform.orientation * glm::vec3(0, 1, 0));
                shader_manager.SetUniform("view", camera.view);
                shader_manager.SetUniform("view_position", transform.position);
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

    RenderMeshes(entity_manager, component_manager, system_manager, delta_time);


    RenderSurfaces(entity_manager, component_manager, system_manager, delta_time);


}

void RenderSystem::RenderMeshes(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    for (auto entity : component_manager.GetEntitiesWithComponents<MeshComponent, TransformComponent>()) {
        auto& meshComp = component_manager.GetComponent<MeshComponent>(entity);
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);

        Mesh& mesh = resource_manager.GetMesh(meshComp.mesh_name);

        //if (!meshComp.enabled) continue; // Skip inactive meshes

        /*if (transform.stale)
        {
            meshComp.model =
                glm::translate(glm::mat4(1.0f), transform.position) *
                glm::mat4_cast(transform.orientation) *
                glm::scale(glm::mat4(1.0f), transform.scale);
            transform.stale = false;
        }*/

        meshComp.model =
            glm::translate(glm::mat4(1.0f), transform.position) *
            glm::mat4_cast(transform.orientation) *
            glm::scale(glm::mat4(1.0f), transform.scale);

        shader_manager.SetUniform("model", meshComp.model);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_BINDING_POINT, mesh.ssbo);
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void RenderSystem::RenderSurfaces(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    
    for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
        auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);

        Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

        surfaceComp.model =
            glm::translate(glm::mat4(1.0f), transform.position) *
            glm::mat4_cast(transform.orientation) *
            glm::scale(glm::mat4(1.0f), transform.scale);

        shader_manager.SetUniform("model", surfaceComp.model);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_1D, surface.connectivity_texture);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, surface.patch_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, surface.patch_buffer);

        glBindVertexArray(surface.vao);
        glBindBuffer(GL_ARRAY_BUFFER, surface.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface.ebo);

        glPatchParameteri(GL_PATCH_VERTICES, surface.patch_size);
        
        glDrawElements(GL_PATCHES, surface.vertices.size(), GL_UNSIGNED_INT, 0);

    }
}