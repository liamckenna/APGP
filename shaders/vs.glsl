#version 330 core

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform int shading_mode;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoord;
layout(location = 4) in int material_index;
layout(location = 5) in int draw_mode;


out vec3 frag_position;
out vec3 frag_normal;
out vec4 frag_color;
out vec2 frag_tex_coords;
flat out int frag_material_index;
flat out int frag_draw_mode;
flat out vec3 no_interpolation_normal;


void main() {
    vec3 normal_vec = normalize(mat3(transpose(inverse(M))) * normal);  //Correct normal for non-uniform scaling
    vec3 position_vec = vec3(M * vec4(position, 1.0));  //Transform position by the model matrix

    //Pass transformed position and normal to the next stage
    frag_position = position_vec;
    frag_normal = normal_vec;
    frag_color = color;
    frag_tex_coords = texCoord;
    frag_material_index = material_index;
    frag_draw_mode = draw_mode;
    no_interpolation_normal = normal_vec;
    
    
    gl_Position = MVP * vec4(position, 1.0);  //Final position in screen space
}