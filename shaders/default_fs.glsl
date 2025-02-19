#version 450 core
#define MAX_LIGHTS 10
#define MAX_MATERIALS 20
#define MAX_TEXTURES 10

struct Light {
    vec3 position;
    float intensity;
    vec3 color;
    int enabled;
};

struct Material {
    int index;
    int pad1;
    int pad2;
    int pad3;

    vec3 ambient;
    float pad4;

    vec3 diffuse;
    float pad5;

    vec3 specular;
    float pad6;

    float shininess;
    float alpha;
    float pad7;
    float pad8;

    int ambient_tex;
    int diffuse_tex;
    int specular_tex;
    int emissive_tex;

    int alpha_tex;
    int decal_tex;
    int shininess_tex;
    int bump_tex;

    int displacement_tex;
    int reflection_tex;
    int padX1;
    int padX2;
};

layout (std140, binding = 0) uniform LightBlock {
    Light lights[MAX_LIGHTS];
};

layout(std140, binding = 1) uniform MaterialBlock {
    Material materials[MAX_MATERIALS];
};

layout(binding = 2) uniform sampler2D textures[MAX_TEXTURES];

layout(std430, binding = 3) buffer MaterialBuffer {
    int material_index[];
};

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_tex_coord;

out vec4 FragColor;

vec3 GrabTextureCoordinateColor(int texture_index) {
    vec4 texColor = texture(textures[texture_index], vec2(frag_tex_coord.x, frag_tex_coord.y));
    return texColor.xyz;
}

void main() {
    int frag_material_index = material_index[gl_PrimitiveID];
    vec3 lighting = vec3(0.0);
    
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].enabled == 0) continue; // Skip inactive lights

        vec3 lightDir = normalize(lights[i].position - frag_position);
        float diff = max(dot(normalize(frag_normal), lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * lights[i].intensity;

        lighting += diffuse;
    }
    vec3 diffuse_color = vec3(materials[frag_material_index].diffuse);
    if (frag_material_index == 0) diffuse_color = GrabTextureCoordinateColor(materials[frag_material_index].diffuse_tex);
    
    //if (materials[frag_material_index].alpha_tex != materials[frag_material_index].ambient_tex) {
    //    diffuse_color = diffuse_color * GrabTextureCoordinateColor(materials[frag_material_index].diffuse_tex);
    //}

    FragColor = vec4(diffuse_color, 1.0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
