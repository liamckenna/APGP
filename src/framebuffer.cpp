#include "framebuffer.h"

Framebuffer::Framebuffer() {
	// Generate and bind framebuffer
	//glGenFramebuffers(1, &framebuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//// Create and attach AccumColor texture
	//glGenTextures(1, &accum_color_tex);
	//glBindTexture(GL_TEXTURE_2D, accum_color_tex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, user->window->width, user->window->height, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene->buffers->accum_color_tex, 0);

	//// Create and attach AccumAlpha texture
	//glGenTextures(1, &scene->buffers->accum_alpha_tex);
	//glBindTexture(GL_TEXTURE_2D, scene->buffers->accum_alpha_tex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, user->window->width, user->window->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, scene->buffers->accum_alpha_tex, 0);

	//// Create and attach depth renderbuffer
	//glGenRenderbuffers(1, &scene->buffers->depth_attachment);
	//glBindRenderbuffer(GL_RENDERBUFFER, scene->buffers->depth_attachment);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, user->window->width, user->window->height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, scene->buffers->depth_attachment);

	//// Specify draw buffers
	//GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	//glDrawBuffers(2, drawBuffers);

	//// Check framebuffer completeness
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	//	std::cerr << "Framebuffer is not complete!" << std::endl;
	//}
	//else {
	//	std::cout << "Framebuffer successfully created!" << std::endl;
	//}

	// Unbind framebuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}