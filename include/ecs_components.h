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
    glm::mat4 model = glm::mat4(1.0f);
};

struct iPASSComponent {

    glm::mat4 ModelViewMatrix;
    glm::mat4 ProjectionMatrix;

    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    
    bool use_compute;
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