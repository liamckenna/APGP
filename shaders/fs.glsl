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

layout(std140, binding = 0) uniform LightData {
    Light lights[MAX_LIGHTS];
};

layout(std140, binding = 1) uniform MaterialBuffer {
    Material materials[MAX_MATERIALS];
};

layout(binding = 2) uniform sampler2D textures[MAX_TEXTURES];


uniform int light_count;

in vec3 frag_position;
in vec3 frag_normal;
in vec4 frag_color;
in vec2 frag_tex_coords;
flat in int frag_material_index;
flat in int frag_draw_mode;
flat in vec3 no_interpolation_normal;

out vec4 FragColor;

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

void main() {
    Material material = materials[frag_material_index];
    
    vec3 diffuse_value              = GrabTextureCoordinateColor(material.dif_texture_index, material.dif_color);       //used
    vec3 normal_value               = GrabTextureCoordinateColor(material.nrm_texture_index, frag_normal.xyz);          //used
    vec3 bump_value                 = GrabTextureCoordinateColor(material.bmp_texture_index, vec3(0.0));                //NOT used
    vec3 specular_value             = GrabTextureCoordinateColor(material.spc_texture_index, material.spc_color);       //used
    float roughness_value           = GrabTextureCoordinateValue(material.rgh_texture_index, material.roughness);       //NOT used
    float metallic_value            = GrabTextureCoordinateValue(material.met_texture_index, material.metallic);        //NOT used
    vec3 displacement_value         = GrabTextureCoordinateColor(material.dsp_texture_index, vec3(0.0));                //NOT used
    float ambient_occlusion_value   = GrabTextureCoordinateValue(material.aoc_texture_index, 0.0);                      //NOT used
    float opacity_value             = GrabTextureCoordinateValue(material.opc_texture_index, material.opacity);         //NOT used
    vec3 emissive_value             = GrabTextureCoordinateColor(material.ems_texture_index, material.ems_color);       //NOT used
    float height_value              = GrabTextureCoordinateValue(material.hgt_texture_index, 0.0);                      //NOT used
    vec3 ambient_value              = material.amb_color;                                                               //used
    float shininess_value           = material.shininess;                                                               //used

    
    if (frag_draw_mode != 4) { //wireframe
        FragColor = vec4(diffuse_value, frag_color.a);
    } else if (shading_mode == 1) {
		FragColor = frag_color;
	} else if (shading_mode == 2 || shading_mode == 5) { //smooth shading (Phong), with & w/o texture
		if (shading_mode == 5) diffuse_value = material.dif_color;
		
        vec3 finalLighting = vec3(0.0);
        vec3 norm = normalize(frag_normal);

        for (int i = 0; i < light_count; i++) {
            if (lights[i].is_active == 0) continue;
            vec3 light_position = lights[i].position;
            float light_strength = lights[i].strength;
            vec3 light_color = lights[i].color;

            float adjustedStrength = light_strength / 100.0;
            float distance = length(light_position - frag_position);
            float attenuation = adjustedStrength / (distance * distance);

            vec3 lightDir = normalize(light_position - frag_position);
            vec3 viewDir = normalize(camera_position - frag_position);

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * light_color * diffuse_value * attenuation;

            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess_value);
            vec3 specular = spec * light_color * specular_value * attenuation;  

            finalLighting += diffuse + specular;
        }

        vec3 ambient;
        if (diffuse_value == vec3(0.0)) ambient = ambient_value * ambient_intensity * diffuse_value;
        else ambient = ambient_value * ambient_intensity * normalize(diffuse_value);

        vec3 lighting = ambient + finalLighting;

        vec3 finalColor = lighting;

        finalColor = min(finalColor, vec3(1.0));

        FragColor = vec4(finalColor, frag_color.a);
		
	} else if (shading_mode == 3) { //visualize Normals
		FragColor = vec4(normalize(frag_normal) * 0.5 + 0.5, 1.0);
	} else if (shading_mode == 4) { //visualize UVs
		FragColor = vec4(frag_tex_coords, 0.0, 1.0);
	} else {
        FragColor = frag_color;
    }
}
