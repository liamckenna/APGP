#include "render_system.h"
#include "system_manager.h"
#include <iostream>

#include "universal_vars.h"

RenderSystem::RenderSystem(ResourceManager& rm, ShaderManager& sm)
    : resource_manager(rm), shader_manager(sm) {
    surface_renderer = SurfaceRenderer();
}

void RenderSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    if (shader_manager.GetActiveShader() != shader_manager.GetShaderID("surface")) shader_manager.UseShader("surface");
    Clear();

    UpdateProjection(entity_manager, component_manager, system_manager, delta_time);

    //RenderMeshes(entity_manager, component_manager, system_manager, delta_time);
    
    //RenderScreenQuad(entity_manager, component_manager, system_manager, delta_time);

    RenderSurfaceLighting(entity_manager, component_manager, system_manager, delta_time);

    RenderSurfaces(entity_manager, component_manager, system_manager, delta_time);


    //std::cout << "Main render update has finished!" << std::endl;

}

void RenderSystem::Clear() {

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
                ViewMatrix = camera.view;
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
                ProjectionMatrix = camera.projection;
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

void RenderSystem::RenderSurfaceLighting(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {
    

    for (auto entity : component_manager.GetEntitiesWithComponents<DirectionalLightComponent, TransformComponent>()) {
        auto& light_transform = component_manager.GetComponent<TransformComponent>(entity);
        auto& dirLightComp = component_manager.GetComponent<DirectionalLightComponent>(entity);
        glm::vec3 pos = glm::vec3(0, 50, 0);
        glm::vec3 dir = glm::normalize(glm::vec3(0, -1, 0));
        light_transform.SetPosition(pos);
        light_transform.SetDirection(dir);

        glm::mat4 proj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
        glm::mat4 view = glm::lookAt(light_transform.position, glm::vec3(0),
            light_transform.orientation * glm::vec3(0, 1, 0));

        //ProjectionMatrix = proj;
        //ViewMatrix = view;

        for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);
            auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
            Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

            if (surfaceComp.surface_name == "test_surface") transform.SetDirection(glm::vec3(0, 1, 0));

            surfaceComp.model =
                glm::translate(glm::mat4(1.0f), transform.position) *
                glm::mat4_cast(transform.orientation) *
                glm::scale(glm::mat4(1.0f), transform.scale);

            ModelMatrix = surfaceComp.model;



            surface_renderer.renderSurface(&surface, 1440, is_first_frame, use_compute, true, 
                glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.9, 0.8, 0.2), glm::vec3(0.5, 0.5, 0.5), dirLightComp.patch_buffer, 
                view * ModelMatrix, proj, shader_manager);

        }

    }

}

void RenderSystem::RenderSurfaces(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);


    for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);
        auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
        Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

        surfaceComp.model =
            glm::translate(glm::mat4(1.0f), transform.position) *
            glm::mat4_cast(transform.orientation) *
            glm::scale(glm::mat4(1.0f), transform.scale);

        ModelMatrix = surfaceComp.model;
        //bool is use_compute
        surface_renderer.renderSurface(&surface, 1440, is_first_frame, use_compute, false, 
            glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.9, 0.8, 0.2), glm::vec3(0.5, 0.5, 0.5), 0, 
            ViewMatrix * ModelMatrix, ProjectionMatrix, shader_manager);
    }
    is_first_frame = false;
}