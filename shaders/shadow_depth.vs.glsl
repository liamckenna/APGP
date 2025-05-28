// shadow_depth.vs.glsl
#version 450 core

layout(location = 0) in vec3 aPos;

// projection * view for this cubemap face
uniform mat4 lightSpaceMatrix;  
// model matrix of the object
uniform mat4 model;            

void main() {
    // standard depth‐only pass
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
