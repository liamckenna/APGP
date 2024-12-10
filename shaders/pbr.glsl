#version 430 core
#define MAX_LIGHTS 100
#define MAX_MATERIALS 100
#define MAX_TEXTURES 10

struct Light {
    vec3 position;
    float strength;
    vec3 color;
    int is_active;
};

struct Material {
    int index;				// 4 bytes
    int tris;				// 4 bytes
	int edges; 			    // 4 bytes
	float refractive_index;	// 4 bytes (total 16 bytes)

    vec3 dif_color;	        // 12 bytes
    float pad2;			    // 4 bytes (total 16 bytes)

    vec3 amb_color;	        // 12 bytes
    float pad3;				// 4 bytes padding (total 16 bytes)

    vec3 spc_color;	        // 12 bytes
    float pad4;				// 4 bytes padding (total 16 bytes)

    vec3 ems_color;	        // 12 bytes
	float pad5;				// 4 bytes padding (total 16 bytes)

    float shininess;		// 4 bytes
    float roughness;		// 4 bytes
    float opacity;			// 4 bytes
    float metallic;			// 4 bytes (total 16 bytes)

    int dif_texture_index;	// 4 bytes
    int nrm_texture_index;	// 4 bytes
    int bmp_texture_index;	// 4 bytes
    int spc_texture_index;	// 4 bytes (total 16 bytes)

    int rgh_texture_index;	// 4 bytes
    int dsp_texture_index;	// 4 bytes
    int aoc_texture_index;	// 4 bytes
    int opc_texture_index;	// 4 bytes (total 16 bytes)
    int ems_texture_index;	// 4 bytes
    int hgt_texture_index;	// 4 bytes
    int met_texture_index;	// 4 bytes
    int pad8;				// 4 bytes padding (total 16 bytes)
};

uniform vec3 camera_position;
uniform int shading_mode;
uniform int phong_exponent;
uniform float ambient_intensity;
uniform int light_count;

layout(std140, binding = 0) uniform LightData {
    Light lights[MAX_LIGHTS];
};

layout(std140, binding = 1) uniform MaterialBuffer {
    Material materials[MAX_MATERIALS];
};

layout(binding = 2) uniform sampler2D textures[MAX_TEXTURES];


in vec3 frag_position;
in vec3 frag_normal;
in vec4 frag_color;
in vec2 frag_tex_coords;
flat in int frag_material_index;
flat in int frag_draw_mode;
flat in vec3 no_interpolation_normal;

layout(location = 0) out vec4 AccumColor;
layout(location = 1) out float AccumAlpha;

vec3 GrabTextureCoordinateColor(int texture_index, vec3 fallback_color) {
    if (texture_index >= 0) {
        vec4 texColor = texture(textures[texture_index], vec2(frag_tex_coords.x, 1.0 - frag_tex_coords.y));
        return texColor.xyz;
    } else {
        return fallback_color;
    }
}

float GrabTextureCoordinateValue(int texture_index, float fallback_value) {
    if (texture_index >= 0) {
        vec4 texColor = texture(textures[texture_index], vec2(frag_tex_coords.x, 1.0 - frag_tex_coords.y));
        return texColor.x;
    } else {
        return fallback_value;
    }
}

vec3 FresnelSchlick(float cos_theta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}

float DistributionGGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (3.14159 * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

void main() {
    Material material = materials[frag_material_index];
    
    vec3 base_color                 = GrabTextureCoordinateColor(material.dif_texture_index, material.dif_color);       //used
    vec3 normal_value               = GrabTextureCoordinateColor(material.nrm_texture_index, frag_normal.xyz);          //used
    vec3 bump_value                 = GrabTextureCoordinateColor(material.bmp_texture_index, vec3(0.0));                //NOT used
    vec3 specular_value             = GrabTextureCoordinateColor(material.spc_texture_index, material.spc_color);       //used
    float roughness_value           = GrabTextureCoordinateValue(material.rgh_texture_index, material.roughness);       //NOT used
    float metallic_value            = GrabTextureCoordinateValue(material.met_texture_index, material.metallic);        //NOT used
    vec3 displacement_value         = GrabTextureCoordinateColor(material.dsp_texture_index, vec3(0.0));                //NOT used
    float ambient_occlusion_value   = GrabTextureCoordinateValue(material.aoc_texture_index, 1.0);                      //NOT used
    float opacity_value             = GrabTextureCoordinateValue(material.opc_texture_index, material.opacity);         //NOT used
    vec3 emissive_value             = GrabTextureCoordinateColor(material.ems_texture_index, material.ems_color);       //NOT used
    float height_value              = GrabTextureCoordinateValue(material.hgt_texture_index, 0.0);                      //NOT used
    vec3 ambient_value              = material.amb_color;                                                               //used
    float shininess_value           = material.shininess;
    float refractive_index_value    = material.refractive_index;

    roughness_value = 1.0 - sqrt(shininess_value / 1000.0);
    
    vec3 N = normalize(normal_value);
    vec3 V = normalize(camera_position - frag_position);
    vec3 total_diffuse_lighting = vec3(0.0);
    vec3 total_specular_lighting = vec3(0.0);
    float diffuse_weight = opacity_value;
    float specular_weight = 1.0;
    //using cook-torrance brdf model
    for (int i = 0; i < light_count; i++) {

        if (lights[i].is_active == 0) continue; //skip inactive lights

        //light properties
        vec3 L = normalize(lights[i].position - frag_position); //light direction vector
        vec3 H = normalize(V + L); //halfway vector
        vec3 light_color = lights[i].color;
        float light_intensity = lights[i].strength;

        //fresnel term (schlick approximation)
        vec3 F0 = mix(vec3(0.04), base_color, metallic_value); //base reflectance
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        //normal distribution function
        float NDF = DistributionGGX(max(dot(N, H), 0.0), roughness_value);

        //geometry
        float G =   GeometrySchlickGGX(max(dot(N, V), 0.0), roughness_value) *
                    GeometrySchlickGGX(max(dot(N, L), 0.0), roughness_value);

        //specular
        vec3 specular = (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);

        //diffuse
        vec3 kD = vec3(1.0) - F; //energy conservation
        kD *= 1.0 - metallic_value; //metals don't have diffuse reflection
        vec3 diffuse = kD * base_color / 3.14159;

        //lighting contribution
        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse_lighting = diffuse * light_color * light_intensity * NdotL;
        vec3 specular_lighting = specular * light_color * light_intensity * NdotL;
        
        total_diffuse_lighting += diffuse_lighting;
        total_specular_lighting + specular_lighting;
    }

    //emissive
    total_diffuse_lighting += emissive_value;
    
    //refraction
    vec3 refracted_direction = refract(-V, N, 1.0 / refractive_index_value);
    vec3 refracted_color = vec3(0.0, 0.2, 0.5); //placeholder for environment map

    
    vec3 weighted_diffuse = total_diffuse_lighting * diffuse_weight;
    vec3 weighted_specular = total_specular_lighting * specular_weight;
    vec3 weighted_refraction = refracted_color * (1.0 - opacity_value);
    
    //vec3 weighted_total = weighted_diffuse + weighted_specular + weighted_refraction;
    vec3 weighted_total = weighted_diffuse + weighted_specular;

    // Outputs
    AccumColor = vec4(weighted_total, opacity_value);
    AccumAlpha = opacity_value;
    //AccumColor = vec4(1.0, 0.0, 0.0, 1.0);
}
