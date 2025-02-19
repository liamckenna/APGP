#version 450 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_tex_coord;

float rand(float seed) {
    return fract(sin(seed) * 43758.5453123) * 2.0 - 1.0;
}

void main() {
    frag_position = vec3(model * vec4(position, 1.0));

    frag_normal = mat3(transpose(inverse(model))) * normal;
    frag_tex_coord = tex_coord;
    
    gl_Position = projection * view * vec4(frag_position, 1.0);
}
