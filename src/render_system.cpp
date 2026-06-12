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
    auto screen_entities = component_manager.GetEntitiesWithComponents<ScreenComponent>();
    auto info_entities = component_manager.GetEntitiesWithComponent<ScreenInfoComponent>();
    if (!info_entities.empty())
    {
        auto& info = component_manager.GetComponent<ScreenInfoComponent>(info_entities[0]);
        int render_w = (int)(info.width * render_scale);
        int render_h = (int)(info.height * render_scale);
        for (auto entity : screen_entities)
        {
            auto& screen = component_manager.GetComponent<ScreenComponent>(entity);
            if (screen.width != render_w || screen.height != render_h)
            {
                screen.Resize(render_w, render_h);
            }
        }
    }

    if (screen_entities.empty())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        auto& first = component_manager.GetComponent<ScreenComponent>(screen_entities[0]);
        glBindFramebuffer(GL_FRAMEBUFFER, first.fbo);
        glViewport(0, 0, first.width, first.height);
    }
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
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}


void RenderSystem::RenderScreenQuad(EntityManager& entity_manager, ComponentManager& component_manager, SystemManager& system_manager, float delta_time)
{
    auto screen_entities = component_manager.GetEntitiesWithComponents<ScreenComponent>();
    if (screen_entities.empty()) return;

    int window_w = 0, window_h = 0;
    auto info_entities = component_manager.GetEntitiesWithComponent<ScreenInfoComponent>();
    if (!info_entities.empty())
    {
        auto& info = component_manager.GetComponent<ScreenInfoComponent>(info_entities[0]);
        window_w = info.width;
        window_h = info.height;
    }

    glDisable(GL_DEPTH_TEST);

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
        if (!shader_manager.UseShader(curr.shader_name)) shader_manager.UseShader("default_screen_quad");
        if (shader_manager.GetActiveShader() == shader_manager.GetShaderID("fxaa"))
        {
            if (!fxaa) shader_manager.UseShader("default_screen_quad");
            else 
            {
                shader_manager.SetUniform("screenWidth", curr.width);
                shader_manager.SetUniform("screenHeight", curr.height);
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