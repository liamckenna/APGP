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

    void SetPosition(glm::vec3 destination) {
        position = destination;
        stale = true;
    }

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
    float range = 10.f;
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
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        float bc[4] = { 1,1,1,1 };
        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, bc);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

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

class ShaderManager;
class SystemManager;
class EntityManager;
class ComponentManager;

struct ParaboloidPointLightComponent {
    GLuint maps[2];
    GLuint dp_fbo;
    GLuint depth_rb;
    ParaboloidPointLightComponent() {
        std::cout << "paraboloid light created... ";
        glGenFramebuffers(1, &dp_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, dp_fbo);

        glGenTextures(2, maps);
    
        for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, maps[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 1024, 1024, 0, GL_RED, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, maps[i], 0);
        }

        glGenRenderbuffers(1, &depth_rb);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
        std::cout << " successfully!" << std::endl;
    }

    void RenderShadows(ShaderManager& shader_manager, SystemManager& system_manager, EntityManager& entity_manager, ComponentManager& component_manager, TransformComponent& light_transform, float delta_time);

    bool stale = true;
};

struct MeshComponent {
    bool enabled = true;
    std::string mesh_name = "";
    GLuint vao = 0;
    GLuint vbo = 0;
    glm::mat4 model = glm::mat4(1.0f);
};

struct SurfaceComponent {
    bool enabled = true;
    std::string surface_name = "";
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint patch_buffer = 0;
    glm::mat4 model = glm::mat4(1.0f);
};

struct DebugComponent {
    bool enabled = true;
};

struct ScreenComponent {

    GLuint vao;
    GLuint vbo;

    ScreenComponent() {
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
        };
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        // positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // texcoords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};