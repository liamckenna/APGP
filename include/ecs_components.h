#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <GL/glew.h>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

struct TransformComponent {
    glm::vec3 position = glm::vec3(0.f);
    glm::quat orientation = glm::quat(1.f, 0.f, 0.f, 0.f);
    glm::vec3 scale = glm::vec3(1.f);
    bool stale = true;

    void TranslateForward(float distance, float delta_time) {
        position += (delta_time * distance) * (orientation * glm::vec3(0, 0, -1));
        stale = true;
    };
    void TranslateBackward(float distance, float delta_time) {
        position += (delta_time * distance) * (orientation * glm::vec3(0, 0, 1));
        stale = true;
    };
    void TranslateLeft(float distance, float delta_time) {
        position += (delta_time * distance) * (orientation * glm::vec3(-1, 0, 0));
        stale = true;
    };
    void TranslateRight(float distance, float delta_time) {
        position += (delta_time * distance) * (orientation * glm::vec3(1, 0, 0));
        stale = true;
    };
    void TranslateDown(float distance, float delta_time) {
        position += (delta_time * distance) * (glm::vec3(0, -1, 0));
        stale = true;
    };
    void TranslateUp(float distance, float delta_time) {
        position += (delta_time * distance) * (glm::vec3(0, 1, 0));
        stale = true;
    };
    void RotateYaw(float angle, float delta_time) {
        glm::quat yaw = glm::angleAxis(glm::radians(angle * delta_time), glm::vec3(0.f, 1.f, 0.f));
        orientation = yaw * orientation;
        stale = true;
    };
    void RotatePitch(float angle, float delta_time) {
        orientation = glm::rotate(orientation, glm::radians(angle * delta_time), glm::vec3(1.f, 0.f, 0.f));
        stale = true;
    };
    void RotateRoll(float angle, float delta_time) {
        glm::vec3 forward = orientation * glm::vec3(0.f, 0.f, -1.f);
        glm::quat roll = glm::angleAxis(glm::radians(angle * delta_time), forward);
        orientation = roll * orientation;
        stale = true;
    };
};

struct CameraComponent {
    bool enabled = true;
    float fov = 60.f;
    float aspect_ratio = 8.f / 6.f;
    float near = 0.1f;
    float far = 100.f;
    bool perspective = true;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.f);
    bool stale = true;
};

struct PrimaryCameraComponent {
    bool enabled = true;
    float velocity = 5.f;
};

struct LightComponent {
    bool enabled = true;
    int index = -1;
    float intensity = 1.f;
    glm::vec3 color = glm::vec3(1.f);
    bool stale = true;
};

struct PointLightComponent {
    float near = 0.1f;
    float far = 1000.f;
    glm::mat4 shadow_projection = glm::mat4(1.f);
    glm::mat4 light_space_matrices[6] = {glm::mat4(1.f)};
    GLuint depth_map_fbo;
    GLuint depth_map_cube;
    bool stale = true;
    PointLightComponent() { 
        glGenTextures(1, &depth_map_cube);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map_cube);
        // Create an empty depth texture for each face of the cubemap
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
                1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        
        glGenFramebuffers(1, &depth_map_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);

        // Attach the cubemap to the framebuffer’s depth attachment
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map_cube, 0);

        // Disable color rendering, as we only need depth
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Error: Shadow framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
    };
};

struct MeshComponent {
    bool enabled = true;
    std::string mesh_name = "";
    GLuint vao = 0;
    GLuint vbo = 0;
    glm::mat4 model = glm::mat4(1.0f);
};

struct DebugComponent {
    bool enabled = true;
};