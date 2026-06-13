#include "render_system.h"
#include "system_manager.h"
#include <iostream>

#include "universal_vars.h"

RenderSystem::RenderSystem(ResourceManager& rm, ShaderManager& sm)
    : resource_manager(rm), shader_manager(sm) {
    surface_renderer = SurfaceRenderer();
}

void RenderSystem::Update(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time)
{
    SetRenderDestination(entity_manager, component_manager, system_manager, delta_time);

    if (pbr) shader_manager.UseShader("pbr");
    else     shader_manager.UseShader("default");

    Clear();
    UpdateProjection(entity_manager, component_manager, system_manager, delta_time);
    RenderMeshes(entity_manager, component_manager, system_manager, delta_time);

    RenderScreenQuad(entity_manager, component_manager, system_manager, delta_time);
}

void RenderSystem::SetRenderDestination(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time)
{
    auto scene_target_entity = component_manager.GetEntitiesWithComponent<SceneTargetComponent>()[0];
    auto screen_entitites = component_manager.GetEntitiesWithComponent<ScreenComponent>();
    auto info_entity = component_manager.GetEntitiesWithComponent<ScreenInfoComponent>()[0];
    
    auto& info_component = component_manager.GetComponent<ScreenInfoComponent>(info_entity);
    int render_w = (int)(info_component.width * render_scale);
    int render_h = (int)(info_component.height * render_scale);

    auto& scene_target_component = component_manager.GetComponent<SceneTargetComponent>(scene_target_entity);
    if (scene_target_component.width != render_w || scene_target_component.height != render_h)
    {
        scene_target_component.Resize(render_w, render_h);
    }
    for (auto screen_entity : screen_entitites)
    {
        auto& screen_component = component_manager.GetComponent<ScreenComponent>(screen_entity);
        if (screen_component.width != render_w || screen_component.height != render_h)
        {
            screen_component.Resize(render_w, render_h);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, scene_target_component.fbo);
    glViewport(0, 0, scene_target_component.width, scene_target_component.height);
}

void RenderSystem::Clear()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void RenderSystem::UpdateProjection(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time)
{
    for (auto entity : component_manager.GetEntitiesWithComponent<CameraComponent>())
    {
        if (component_manager.HasComponent<PrimaryCameraComponent>(entity)) {
            auto& transform = component_manager.GetComponent<TransformComponent>(entity);
            auto& camera = component_manager.GetComponent<CameraComponent>(entity);
            if (transform.stale)
            {
                camera.view = glm::lookAt(transform.position, transform.position + (transform.orientation * glm::vec3(0, 0, -1)),
                    transform.orientation * glm::vec3(0, 1, 0));
                ViewMatrix = camera.view;
                transform.stale = false;
            }
            if (camera.stale)
            {
                if (camera.perspective)
                {
                    camera.projection = glm::perspective(glm::radians(camera.fov), camera.aspect_ratio, camera.near, camera.far);
                }
                else
                {
                    float size = camera.fov * 0.5f;
                    camera.projection = glm::ortho(-size * camera.aspect_ratio, size * camera.aspect_ratio,
                        -size, size, camera.near, camera.far);
                }
                ProjectionMatrix = camera.projection;
                camera.stale = false;
            }
            shader_manager.SetUniform("view", camera.view);
            shader_manager.SetUniform("view_position", transform.position);
            shader_manager.SetUniform("projection", camera.projection);

            auto scene_target_entity = component_manager.GetEntitiesWithComponent<SceneTargetComponent>()[0];
            auto& scene_target = component_manager.GetComponent<SceneTargetComponent>(scene_target_entity);
            glm::mat4 current_view_proj = camera.projection * camera.view;
            scene_target.prev_view_proj = scene_target.view_proj;
            scene_target.view_proj = current_view_proj;
            scene_target.inv_view_proj = glm::inverse(current_view_proj);
        }
    }
}

void RenderSystem::RenderMeshes(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time)
{
    for (auto entity : component_manager.GetEntitiesWithComponents<MeshComponent, TransformComponent>())
    {
        auto& meshComp = component_manager.GetComponent<MeshComponent>(entity);
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);

        Mesh& mesh = resource_manager.GetMesh(meshComp.mesh_name);

        meshComp.model =
            glm::translate(glm::mat4(1.0f), transform.position) *
            glm::mat4_cast(transform.orientation) *
            glm::scale(glm::mat4(1.0f), transform.scale);

        shader_manager.SetUniform("model", meshComp.model);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_BINDING_POINT, mesh.ssbo);
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}


void RenderSystem::RenderScreenQuad(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time)
{
    auto scene_target_entity = component_manager.GetEntitiesWithComponent<SceneTargetComponent>()[0];
    auto& scene_target_component = component_manager.GetComponent<SceneTargetComponent>(scene_target_entity);
    auto screen_entities = component_manager.GetEntitiesWithComponents<ScreenComponent>();

    glDisable(GL_DEPTH_TEST);

    int window_w = 0, window_h = 0;
    auto info_entities = component_manager.GetEntitiesWithComponent<ScreenInfoComponent>();
    if (!info_entities.empty())
    {
        auto& info = component_manager.GetComponent<ScreenInfoComponent>(info_entities[0]);
        window_w = info.width;
        window_h = info.height;
    }
    
    if (!screen_entities.empty())
    {
        auto& first_screen_component = component_manager.GetComponent<ScreenComponent>(screen_entities[0]);
        glBindFramebuffer(GL_FRAMEBUFFER, first_screen_component.fbo);
        glViewport(0, 0, first_screen_component.width, first_screen_component.height);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_w, window_h);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    shader_manager.UseShader("default_screen_quad");
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene_target_component.colorTexture);
    shader_manager.SetUniform("screenTexture", 0);
    glBindVertexArray(scene_target_component.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    if (screen_entities.empty()) return;

    for (size_t i = 0; i < screen_entities.size(); i++)
    {
        auto& curr = component_manager.GetComponent<ScreenComponent>(screen_entities[i]);

        if (i + 1 < screen_entities.size())
        {
            auto& next = component_manager.GetComponent<ScreenComponent>(screen_entities[i + 1]);
            glBindFramebuffer(GL_FRAMEBUFFER, next.fbo);
            glViewport(0, 0, next.width, next.height);
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window_w, window_h);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        if (!shader_manager.UseShader(curr.shader_name) || !curr.enabled) shader_manager.UseShader("default_screen_quad");
        else
        {
            for (auto input : curr.scene_inputs)
            {
                if (input == "depthTexture")
                {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, scene_target_component.depthTexture);
                    shader_manager.SetUniform("depthTexture", 1);
                }
                else if (input == "prevViewProj")
                {
                    shader_manager.SetUniform("prevViewProj", scene_target_component.prev_view_proj);
                }
                else if (input == "invViewProj")
                {
                    shader_manager.SetUniform("invViewProj", scene_target_component.inv_view_proj);
                }
                else if (input == "deltaTime")
                {
                    shader_manager.SetUniform("deltaTime", delta_time);
                }
                else if (input == "screenSize")
                {
                    shader_manager.SetUniform("screenWidth", curr.width);
                    shader_manager.SetUniform("screenHeight", curr.height);
                }
                else if (input == "screenWidth")
                {
                    shader_manager.SetUniform("screenWidth", curr.width);
                }
                else if (input == "screenHeight")
                {
                    shader_manager.SetUniform("screenHeight", curr.height);
                }
            }
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, curr.colorTexture);
        shader_manager.SetUniform("screenTexture", 0);
        glBindVertexArray(curr.vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::RenderSurfaces(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    for (auto entity : component_manager.GetEntitiesWithComponents<SurfaceComponent, TransformComponent>())
    {
        auto& transform = component_manager.GetComponent<TransformComponent>(entity);
        auto& surfaceComp = component_manager.GetComponent<SurfaceComponent>(entity);
        Surface& surface = resource_manager.GetSurface(surfaceComp.surface_name);

        surfaceComp.model =
            glm::translate(glm::mat4(1.0f), transform.position) *
            glm::mat4_cast(transform.orientation) *
            glm::scale(glm::mat4(1.0f), transform.scale);

        ModelMatrix = surfaceComp.model;

        surface_renderer.renderSurface(&surface, 1440, is_first_frame, use_compute, glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.9, 0.8, 0.2), glm::vec3(0.5, 0.5, 0.5), ViewMatrix * ModelMatrix, ProjectionMatrix, shader_manager);
    }
    is_first_frame = false;
}