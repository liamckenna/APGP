#include "ecs_components.h"
#include "shader_manager.h"
#include "render_system.h"
#include "system_manager.h"
#include "entity_manager.h"
#include "component_manager.h"



void ParaboloidPointLightComponent::RenderShadows(ShaderManager& shader_manager, SystemManager& system_manager, EntityManager& entity_manager, ComponentManager& component_manager, TransformComponent& light_transform, float delta_time) {

	glBindFramebuffer(GL_FRAMEBUFFER, dp_fbo);
	
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, 1024, 1024);

	for (int i = 0; i < 2; i++) {

		GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
		glDrawBuffers(1, &attachment);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// adjust culling
		if (i == 0) { glCullFace(GL_BACK); }
		else { glCullFace(GL_FRONT); }
		glEnable(GL_CULL_FACE);

		shader_manager.UseShader("paraboloid");
		shader_manager.SetUniform("z_direction", i);
		shader_manager.SetUniform("lightPos", light_transform.position);

		static_cast<RenderSystem*>(system_manager.systems[0].get())->RenderMeshes(entity_manager, component_manager, system_manager, delta_time);

		glDisable(GL_CULL_FACE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	shader_manager.UseShader("default");

}