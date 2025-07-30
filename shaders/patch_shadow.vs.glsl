#version 430
layout(location = 0) in vec3 inPosition;

layout(std430, binding = 0) buffer LightMVPBuffer {
    mat4 light_mvps[];
};

uniform int surface_id;

void main() {
    gl_Position = light_mvps[surface_id] * vec4(inPosition, 1.0);
}