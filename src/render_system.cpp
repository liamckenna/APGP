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
        auto& directional_light = component_manager.GetComponent<DirectionalLightComponent>(entity);
        
        if (is_first_frame) {
            glGenBuffers(1, &light_mvp_buffer);
        }

        glBindTexture(GL_TEXTURE_2D, directional_light.depth_texture);
        GLuint clearVal = glm::floatBitsToUint(1.0f);
        glClearTexImage(directional_light.depth_texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &clearVal);
        
        glm::vec3 pos = glm::vec3(0, 10, 0);
        glm::vec3 dir = glm::normalize(glm::vec3(0, -1, 0));
        light_transform.SetPosition(pos);
        light_transform.SetDirection(dir);



        glm::mat4 proj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
        glm::mat4 view = glm::lookAt(light_transform.position, glm::vec3(0),
            light_transform.orientation * glm::vec3(0, 1, 0));


        //visualize directional light view
        //ViewMatrix = view;
        //ProjectionMatrix = proj;

        std::vector<glm::mat4> light_mvps;

        for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);
            auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
            Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

            surfaceComp.model =
                glm::translate(glm::mat4(1.0f), transform.position) *
                glm::mat4_cast(transform.orientation) *
                glm::scale(glm::mat4(1.0f), transform.scale);

            glm::mat4 light_MVP = proj * view * surfaceComp.model;
            light_mvps.push_back(light_MVP);
        }

        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_mvp_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, light_mvps.size() * sizeof(glm::mat4), light_mvps.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, light_mvp_buffer);

    }
    
    for (auto entity : component_manager.GetEntitiesWithComponents<DirectionalLightComponent, TransformComponent>()) {
        auto& light_transform = component_manager.GetComponent<TransformComponent>(entity);
        auto& directional_light = component_manager.GetComponent<DirectionalLightComponent>(entity);


        if (is_first_frame) {
            std::cout << "FIRST FRAME" << std::endl;
            glGenBuffers(1, &launch_point_buffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_buffer);
 
            std::vector<GLuint> launch_point;
            launch_point.push_back(0);
            int i = 0;
            for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
                auto& transform = component_manager.GetComponent<TransformComponent>(entity);
                auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
                Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

                uint patch_count = (surface.vertices.size() + 15) / 16;
                launch_point.push_back(launch_point[i] + patch_count);
                i++;
            }

            std::cout << "LAUNCH POINT CPU DATA" << std::endl;
            for (int j = 0; j < launch_point.size(); j++) {
                std::cout << j << ": " << launch_point[j] << std::endl;
            }

            GLsizeiptr bytes = launch_point.size() * sizeof(GLuint);
            glBufferData(GL_SHADER_STORAGE_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);


            void* mapped = glMapBufferRange(
                GL_SHADER_STORAGE_BUFFER,
                0, bytes,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
            );
            std::memcpy(mapped, launch_point.data(), bytes);
           
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, launch_point_buffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, launch_point.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);


            for (size_t idx = 0; idx < launch_point.size(); ++idx) {
                glBufferSubData(
                    GL_SHADER_STORAGE_BUFFER,
                    idx * sizeof(GLuint),
                    sizeof(GLuint),
                    &launch_point[idx]
                );
            }
            std::vector<uint> launch_point_data(launch_point.size());
            glGetBufferSubData(
                GL_SHADER_STORAGE_BUFFER,
                0,
                sizeof(uint) * launch_point.size(),
                launch_point_data.data()
            );
            std::cout << "-- LAUNCH POINT DATA --" << std::endl;
            for (int i = 0; i < launch_point_data.size(); i++) {
                std::cout << i << ": " << launch_point_data[i] << std::endl;
            }
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        int i = 0;
        for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);
            auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
            Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

            if (surfaceComp.surface_name == "test_surface") {
                transform.SetDirection(glm::vec3(0, 1, 0));
            }

            surface_renderer.WriteDepthBuffer(surface.vbo, launch_point_buffer, directional_light.depth_texture,
                directional_light.patch_depth_buffer, directional_light.patch_span_buffer, light_mvp_buffer,
                surface.vertices.size(), i, 2.0f / 1080, shader_manager);
            i++;
        }

        i = 0;
        for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);
            auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
            Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);
                
            surface_renderer.ReadDepthBuffer(directional_light.patch_depth_buffer, directional_light.patch_span_buffer, directional_light.patch_shadow_buffer,
                launch_point_buffer, directional_light.depth_texture, i, surface.vertices.size(), shader_manager);
            i++;
        }
    }
}

void RenderSystem::RenderSurfaces(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    GLuint depth_texture;

    GLuint patch_shadow_buffer;

    for (auto entity : component_manager.GetEntitiesWithComponents<DirectionalLightComponent, TransformComponent>()) {
        auto& directional_light = component_manager.GetComponent<DirectionalLightComponent>(entity);
        depth_texture = directional_light.depth_texture;
        patch_shadow_buffer = directional_light.patch_shadow_buffer;
    }

    int i = 0;
    for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>()) {
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);
        auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
        Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

        surfaceComp.model =
            glm::translate(glm::mat4(1.0f), transform.position) *
            glm::mat4_cast(transform.orientation) *
            glm::scale(glm::mat4(1.0f), transform.scale);

        ModelMatrix = surfaceComp.model;

        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        surface_renderer.UpdatePatchTessLevels(surface.vbo, surface.patch_buffer, MVP, surface.vertices.size(), i, 2.0f / 1080, shader_manager);
        surface_renderer.RenderSurface(ModelMatrix, ViewMatrix, ProjectionMatrix, glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.9, 0.8, 0.2), glm::vec3(0.5, 0.5, 0.5),
            surface.vao, surface.vbo, surface.ebo, surface.patch_buffer, patch_shadow_buffer, launch_point_buffer, surface.vertices.size(), i, shader_manager);
        i++;
    }
    is_first_frame = false;
}