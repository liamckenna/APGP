#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <GL/glew.h>

struct TransformComponent {
    glm::vec3 position = glm::vec3(0.f);
    glm::quat orientation = glm::quat(1.f, 0.f, 0.f, 0.f);
    glm::vec3 scale = glm::vec3(1.f);
    bool stale = true;
};

struct CameraComponent {
    bool enabled = true;
    float fov = 60.f;
    float aspect_ratio = 8.f / 6.f;
    float near = 0.1f;
    float far = 100.f;
    bool perspective = true;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    bool stale = true;
};

struct PrimaryCameraComponent {
    bool enabled = true;
};

struct LightComponent {
    bool enabled = true;
    int index = -1;
    float intensity = 1.f;
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