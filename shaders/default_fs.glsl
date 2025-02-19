#version 450 core
#define MAX_LIGHTS 10
#define MAX_MATERIALS 20
#define MAX_TEXTURES 10

struct Light {
	int enabled;
    int index;
	float intensity;
	float pad1;

	vec3 position;
	float pad2;

	vec3 color;
	float pad3;
};

struct Material
{
    int index;
    float shininess;
    float alpha;
    int pad1;

    vec3 ambient;
    float pad2;

    vec3 diffuse;
    float pad3;

    vec3 specular;
    float pad4;

    int ambient_tex;
    int diffuse_tex;
    int specular_tex;
    int emissive_tex;
    
    int shininess_tex;
    int bump_tex;
    int displacement_tex;
    int reflection_tex;
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

uniform vec3 view_position;
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_tex_coord;

out vec4 FragColor;

vec3 GrabTextureCoordinateColor(int texture_index, vec3 fallback_color) {
    if (texture_index >= 0) {
        vec4 texColor = texture(textures[texture_index], vec2(frag_tex_coord.x, 1.0 - frag_tex_coord.y));
        return texColor.xyz;
    } else {
        return fallback_color;
    }
}

void main() {
    int frag_material_index = material_index[gl_PrimitiveID];

    vec3 diffuse_value = materials[frag_material_index].diffuse * GrabTextureCoordinateColor(materials[frag_material_index].diffuse_tex, vec3(1.0));


    vec3 lighting = vec3(0.0);
    
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].enabled == 0) continue; // Skip inactive lights

        vec3 lightDir = normalize(lights[i].position - frag_position);
        float diff = max(dot(normalize(frag_normal), lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * lights[i].intensity;

        lighting += diffuse;
    }
    
    //if (materials[frag_material_index].alpha_tex != materials[frag_material_index].ambient_tex) {
    //    diffuse_color = diffuse_color * GrabTextureCoordinateColor(materials[frag_material_index].diffuse_tex);
    //}

    FragColor = vec4(diffuse_value * lighting, 1.0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
