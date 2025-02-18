#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <GL/glew.h>

struct TransformComponent {
    glm::vec3 position = glm::vec3(0.f);
    glm::quat orientation = glm::quat(1.f, 0.f, 0.f, 0.f);
    glm::vec3 scale = glm::vec3(1.f);
};

struct CameraComponent {
    bool enabled = true;
    float fov = 60.f;
    float aspect_ratio = 8.f / 6.f;
    float near = 0.1f;
    float far = 100.f;
    bool perspective = true;
};

struct PrimaryCameraComponent {
    bool enabled = true;
};

struct LightComponent {
    bool enabled = true;
    glm::vec3 color = glm::vec3(1.f);
    float intensity = 1.f;
};

struct MeshComponent {
    bool enabled = true;
    std::string mesh_filepath = "";
    GLuint vao = 0;
    GLuint vbo = 0;
};

struct MaterialComponent {
    bool enabled = true;
    std::string material_filepath = "";
    std::string material_name = "default";
    glm::vec3 diffuse = glm::vec3(1.f);
    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 specular = glm::vec3(1.f);
    glm::vec3 emissive = glm::vec3(0.f);
    float shininess = 32.f;
    float roughness = 0.5f;
    float opacity = 1.f;
    float metallic = 0.f;
    float refractive_index = 1.f;
    int texture_ids[12] = { 0 };
};
