#pragma once
#include <unordered_map>
#include <string>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <typeindex>
#include <glm/glm.hpp>

typedef unsigned int uint;

enum PROJECTION_TYPES {
    ORTHOGRAPHIC = 0xFF,
    PERSPECTIVE = 0X00,
};

#define LIGHT_BINDING_POINT     0
#define MATERIAL_BINDING_POINT  1
#define TEXTURE_BINDING_POINT   2


inline void ToLowerCase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

inline std::string ToLowerCaseCopy(const std::string& str) {
    std::string lowerStr = str;
    ToLowerCase(lowerStr);
    return lowerStr;
}

const static std::unordered_map<std::type_index, GLenum> GL_TYPES = {
    { typeid(int), GL_INT},
    { typeid(unsigned int), GL_UNSIGNED_INT},
    { typeid(float),     GL_FLOAT },
    { typeid(glm::vec2), GL_FLOAT_VEC2 },
    { typeid(glm::vec3), GL_FLOAT_VEC3 },
    { typeid(glm::vec4), GL_FLOAT_VEC4 },
    { typeid(glm::mat2), GL_FLOAT_MAT2 },
    { typeid(glm::mat3), GL_FLOAT_MAT3 },
    { typeid(glm::mat4), GL_FLOAT_MAT4 }
};

inline GLenum FetchGLTypeEnum(std::type_index key) {
    auto it = GL_TYPES.find(key);
    return (it != GL_TYPES.end()) ? it->second : GL_INVALID_ENUM;
}

const static std::unordered_map<std::string, GLenum> GL_ENUMS = {
    //default
    {"default", GL_INVALID_ENUM},

    //draw modes
    {"gl_points", GL_POINTS},
    {"gl_lines", GL_LINES},
    {"gl_line_strip", GL_LINE_STRIP},
    {"gl_line_loop", GL_LINE_LOOP},
    {"gl_triangles", GL_TRIANGLES},
    {"gl_triangle_strip", GL_TRIANGLE_STRIP},
    {"gl_triangle_fan", GL_TRIANGLE_FAN},

    //depth functions
    {"false", GL_NEVER},
    {"<", GL_LESS},
    {"==", GL_EQUAL},
    {"<=", GL_LEQUAL},
    {">", GL_GREATER},
    {"!=", GL_NOTEQUAL},
    {">=", GL_GEQUAL},
    {"true", GL_ALWAYS},

    //cull face
    {"front", GL_FRONT},
    {"back", GL_BACK},
    {"front and back", GL_FRONT_AND_BACK},

    //front face
    {"cw", GL_CW},
    {"ccw", GL_CCW},

    //projection types
    {"perspective", PROJECTION_TYPES::PERSPECTIVE},
    {"orthographic", PROJECTION_TYPES::ORTHOGRAPHIC}

};

inline GLenum FetchGLenum(std::string key) {
    ToLowerCase(key);
    auto it = GL_ENUMS.find(key);
    return (it != GL_ENUMS.end()) ? it->second : GL_INVALID_ENUM;
}

const static std::unordered_map<std::string, int> GLFW_ENUMS = {
    //default
    {"default", GLFW_INVALID_ENUM},
    
    //gl profiles
    {"core", GLFW_OPENGL_CORE_PROFILE},
    {"compat", GLFW_OPENGL_COMPAT_PROFILE},
    {"any", GLFW_OPENGL_ANY_PROFILE}
};

inline int FetchGLFWenum(std::string key) {
    ToLowerCase(key);
    auto it = GLFW_ENUMS.find(key);
    return (it != GLFW_ENUMS.end()) ? it->second : GLFW_INVALID_ENUM;
}

std::string GetExecutableDirectory();