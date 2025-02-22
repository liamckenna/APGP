#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

uniform samplerCube shadowCubeMap;
uniform vec3 lightPos;
uniform int faceIndex; // Which face to sample from

void main() {
    // Map 2D screen coords to 3D cube directions
    vec3 directions[6] = vec3[6](
        vec3( 1.0,  0.0,  0.0), // +X
        vec3(-1.0,  0.0,  0.0), // -X
        vec3( 0.0,  1.0,  0.0), // +Y
        vec3( 0.0, -1.0,  0.0), // -Y
        vec3( 0.0,  0.0,  1.0), // +Z
        vec3( 0.0,  0.0, -1.0)  // -Z
    );

    vec3 fragToLight = directions[faceIndex]; // Choose a face to render
    float depth = texture(shadowCubeMap, fragToLight).r; // Get depth value

    FragColor = vec4(vec3(depth), 1.0); // Convert depth to grayscale
}
