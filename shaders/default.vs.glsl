#version 450 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_tangent;
out vec2 frag_tex_coord;

void main() {

    frag_position = vec3(model * vec4(position, 1.0));

    mat3 normal_matrix = mat3(transpose(inverse(model)));
    frag_normal = normal_matrix * normal;
    frag_tangent = normal_matrix * tangent;
    frag_tex_coord = tex_coord;
    
    gl_Position = projection * view * vec4(frag_position, 1.0);

}
