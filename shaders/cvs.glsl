#version 430 core

layout(location = 0) in vec2 in_position;  // Quad position in NDC
layout(location = 1) in vec2 in_tex_coords; // Texture coordinates

out vec2 tex_coords;
out vec3 normal;
void main() {
    gl_Position = vec4(in_position, 0.0, 1.0); // Map directly to screen space
    tex_coords = in_tex_coords;               // Pass texture coordinates
    normal = vec3(0.0, 0.0, -1.0); // Normal facing the camera
}