#version 450 core

#define MAX_LIGHTS 10
#define MAX_MATERIALS 100
#define MAX_TEXTURES 10

struct Light {
    int enabled;
    int index;
    float intensity;
    float range;

    vec3 position;
    float pad2;

    vec3 color;
    float pad3;
};

struct Material {
    int index;
    float shininess;
    float alpha;
    float pad1;

    vec3 diffuse;
    float metallic;

    vec3 specular;
    float roughness;

    vec3 emissive;
    float pad4;

    int diffuse_tex;
    int specular_tex;
    int emissive_tex;
    int shininess_tex;

    int normal_tex;
    int roughness_tex;
    int metallic_tex;
    int ao_tex;

    int bump_tex;
    int displacement_tex;
    int reflection_tex;
    int opacity_tex;
};

layout(std140, binding = 0) uniform LightBlock {
    Light lights[MAX_LIGHTS];
};

layout(std140, binding = 1) uniform MaterialBlock {
    Material materials[MAX_MATERIALS];
};

layout(binding = 2) uniform sampler2D textures[MAX_TEXTURES];

layout(std430, binding = 3) buffer MaterialBuffer {
    int material_index[];
};

uniform float ambient_intensity;
uniform vec3 view_position;

in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_tangent;
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

float GrabTextureCoordinateValue(int texture_index, float fallback_value) {
    if (texture_index >= 0) {
        return texture(textures[texture_index], vec2(frag_tex_coord.x, 1.0 - frag_tex_coord.y)).x;
    } else {
        return fallback_value;
    }
}

vec3 FresnelSchlick(float cos_theta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float DistributionGGX(float NdotH, float roughness) {
    float a  = roughness * roughness;
    float a2 = a * a;
    float d  = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (3.14159265 * d * d);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

void main() {
    int frag_material_index = material_index[gl_PrimitiveID];
    Material material = materials[frag_material_index];

    vec3  base_color = GrabTextureCoordinateColor(material.diffuse_tex,    material.diffuse);
    float roughness  = GrabTextureCoordinateValue(material.roughness_tex,  material.roughness);
    float metallic   = GrabTextureCoordinateValue(material.metallic_tex,   material.metallic);
    float ao         = GrabTextureCoordinateValue(material.ao_tex,         1.0);
    vec3  emissive   = GrabTextureCoordinateColor(material.emissive_tex,   material.emissive);
    float opacity    = GrabTextureCoordinateValue(material.opacity_tex,    material.alpha);

    vec3 N;
    if (material.normal_tex >= 0) {
        vec3 sampled  = GrabTextureCoordinateColor(material.normal_tex, vec3(0.5, 0.5, 1.0));
        vec3 N_vertex = normalize(frag_normal);
        vec3 T        = normalize(frag_tangent - dot(frag_tangent, N_vertex) * N_vertex);
        vec3 B        = cross(N_vertex, T);
        mat3 TBN      = mat3(T, B, N_vertex);
        N = normalize(TBN * (sampled * 2.0 - 1.0));
    } else {
        N = normalize(frag_normal);
    }

    vec3 V  = normalize(view_position - frag_position);
    vec3 F0 = mix(vec3(0.04), base_color, metallic);

    vec3 total_diffuse  = vec3(0.0);
    vec3 total_specular = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].enabled == 0) continue;

        vec3  light_vec   = lights[i].position - frag_position;
        float dist        = length(light_vec);
        vec3  L           = normalize(light_vec);
        vec3  H           = normalize(V + L);
        float attenuation = 1.0 / (1.0 + dist * dist);
        vec3  radiance    = lights[i].color * lights[i].intensity * attenuation;

        // fresnel
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        // normal distribution
        float NDF = DistributionGGX(max(dot(N, H), 0.0), roughness);

        // geometry
        float G = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness) *
                  GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);

        // cook-torrance specular
        vec3 specular = (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);

        // diffuse (energy-conserving — metals have none)
        vec3 kD      = (vec3(1.0) - F) * (1.0 - metallic);
        vec3 diffuse = kD * base_color / 3.14159265;

        float NdotL = max(dot(N, L), 0.0);
        total_diffuse  += diffuse  * radiance * NdotL;
        total_specular += specular * radiance * NdotL;
    }

    vec3 ambient = vec3(ambient_intensity) * base_color * ao;
    vec3 color   = ambient + total_diffuse + total_specular + emissive;

    // discard fully transparent fragments so they don't write to the depth buffer
    // (prevents transparent surfaces like cornea from occluding geometry behind them)
    if (opacity < 0.01) discard;

    // tone mapping (Reinhard)
    color = color / (color + vec3(1.0));

    // gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, opacity);
}
