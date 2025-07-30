#version 430
layout(location = 0) in vec3 inPosition;
uniform mat4 MVP_MATRIX;

void main() {
    gl_Position = MVP_MATRIX * vec4(inPosition, 1.0);
}