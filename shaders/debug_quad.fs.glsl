#version 450 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform int debug_mode; // 1=UV, 2=depth, 3=side, 0=shadow

void main() {
    vec2 uv = TexCoord;
    vec2 xy = (uv - 0.5) * 2.0;
    float r2 = dot(xy, xy);
    if (r2 > 1.0) {
        // outside the hemisphere disk
        FragColor = vec4(1.0);
        return;
    }

    // reconstruct paraboloid vector length
    float z = (1.0 - r2) / (1.0 + r2);
    float dist = 2.0 / (1.0 + z);
    vec3 v = vec3(xy * dist, z * dist);
    int side = (v.z >= 0.0) ? 0 : 1;

    float mapDepth = (side == 0)
        ? texture(shadowMap0, uv).r
        : texture(shadowMap1, uv).r;
    float bias = 0.25 * length(v);
    float shadow = (mapDepth + bias < length(v)) ? 1.0 : 0.0;

    if (debug_mode == 1) {
        FragColor = vec4(uv, 0.0, 1.0);
    } else if (debug_mode == 2) {
        FragColor = vec4(mapDepth, mapDepth, mapDepth, 1.0);
    } else if (debug_mode == 3) {
        FragColor = (side == 0) ? vec4(1,0,0,1) : vec4(0,1,0,1);
    } else {
        FragColor = vec4(vec3(1.0 - shadow), 1.0);
    }
}
