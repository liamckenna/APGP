#include "render_system.h"
#include "system_manager.h"
#include <iostream>

RenderSystem::RenderSystem(ResourceManager& rm, ShaderManager& sm)
    : resource_manager(rm), shader_manager(sm) {
}

void RenderSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    
    //std::cout << "Main render update has started" << std::endl;

    //UpdateShadows(entity_manager, component_manager, system_manager, delta_time);

    //std::cout << "shadows updated." << std::endl;

    UpdateParaboloid(entity_manager, component_manager, system_manager, delta_time);

    UpdateProjection(entity_manager, component_manager, system_manager, delta_time);

    RenderMeshes(entity_manager, component_manager, system_manager, delta_time);
    
    RenderScreenQuad(entity_manager, component_manager, system_manager, delta_time);
    


    //RenderSurfaces(entity_manager, component_manager, system_manager, delta_time);

    //std::cout << "Main render update has finished!" << std::endl;

}

void RenderSystem::Clear() {

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void RenderSystem::UpdateShadows(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    if (shader_manager.GetActiveShader() != shader_manager.GetShaderID("default")) shader_manager.UseShader("default");
    std::vector<Entity>& point_light_entities = component_manager.GetEntitiesWithComponent<PointLightComponent>();
    for (int i = 0; i < point_light_entities.size(); i++) {
        PointLightComponent& point = component_manager.GetComponent<PointLightComponent>(point_light_entities[i]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, point.depth_map_cube);

    }
}

void RenderSystem::UpdateParaboloid(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {

    //std::cout << "Updating Paraboloid... ";

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_manager.UseShader("default");

    std::vector<Entity>& point_light_entities = component_manager.GetEntitiesWithComponent<ParaboloidPointLightComponent>();
    for (auto entity : point_light_entities) {
        auto& paraboloid = component_manager.GetComponent<ParaboloidPointLightComponent>(entity);
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, paraboloid.maps[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, paraboloid.maps[1]);
        shader_manager.SetUniform("shadowMap0", 0);
        shader_manager.SetUniform("shadowMap1", 1);
        shader_manager.SetUniform("lightPos", transform.position);
        
    }

    //std::cout << "complete!" << std::endl;
}


void RenderSystem::UpdateProjection(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    
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
}

void RenderSystem::RenderMeshes(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    for (auto entity : component_manager.GetEntitiesWithComponents<MeshComponent, TransformComponent>()) {
        auto& meshComp = component_manager.GetComponent<MeshComponent>(entity);
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);

        Mesh& mesh = resource_manager.GetMesh(meshComp.mesh_name);

        meshComp.model =
            glm::translate(glm::mat4(1.0f), transform.position) *
            glm::mat4_cast(transform.orientation) *
            glm::scale(glm::mat4(1.0f), transform.scale);

        if (shader_manager.GetActiveShader() == shader_manager.GetShaderID("default")) shader_manager.SetUniform("model", meshComp.model);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_BINDING_POINT, mesh.ssbo);
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    /*std::cout << "meshes have been rendered from ";
    if (shader_manager.GetActiveShader() == shader_manager.GetShaderID("default")) std::cout << "default shader!" << std::endl;
    else if (shader_manager.GetActiveShader() == shader_manager.GetShaderID("paraboloid")) std::cout << "paraboloid shader!" << std::endl;*/
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

void RenderSystem::RenderScreenQuad(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    shader_manager.UseShader("debug_quad");
    for (auto entity : component_manager.GetEntitiesWithComponents<ScreenComponent>()) {
        auto& screen = component_manager.GetComponent<ScreenComponent>(entity);
        glBindVertexArray(screen.vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
}