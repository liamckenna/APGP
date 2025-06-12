//default.fs.glsl
#version 450 core

#define MAX_LIGHTS 10
#define MAX_MATERIALS 20
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
    int pad1;

    vec3 ambient;
    float pad2;

    vec3 diffuse;
    float pad3;

    vec3 specular;
    float pad4;

    vec3 emissive;
    float pad5;

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

uniform float ambient_intensity;
uniform samplerCube shadowCubeMap;
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform vec3 lightPos;
uniform float far_plane;
uniform vec3 view_position;
uniform int debug_mode;

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_tex_coord;

out vec4 FragColor;

vec3 GrabTextureCoordinateColor(int texture_index, vec3 fallback_color) {

    if (texture_index >= 0) {

        vec4 tex_color = texture(textures[texture_index], vec2(frag_tex_coord.x, 1.0 - frag_tex_coord.y));

        return tex_color.xyz;

    } else {

        return fallback_color;

    }

}

float GrabTextureCoordinateValue(int texture_index, float fallback_value) {

    if (texture_index >= 0) {

        float tex_value = texture(textures[texture_index], vec2(frag_tex_coord.x, 1.0 - frag_tex_coord.y)).x;

        return tex_value;

    } else {

        return fallback_value;

    }

}

float LinearizeDepth(float z_buffer, float near, float far) {

    return (near * far) / (far + z_buffer * (near - far));

}

void main() {

    vec3 view_direction = normalize(view_position - frag_position);
    vec3 normal = normalize(frag_normal);
    vec3 fragToLight = normalize(frag_position - lightPos);
    vec3 lightDir = normalize(lightPos - frag_position);

    float shadow = 0;


    if (debug_mode == 0) {

        int frag_material_index = material_index[gl_PrimitiveID];

        vec3    amb_mat =   materials[frag_material_index].ambient      * GrabTextureCoordinateColor(materials[frag_material_index].ambient_tex,    vec3(1.0));
        vec3    dif_mat =   materials[frag_material_index].diffuse      * GrabTextureCoordinateColor(materials[frag_material_index].diffuse_tex,    vec3(1.0));
        vec3    spc_mat =   materials[frag_material_index].specular     * GrabTextureCoordinateColor(materials[frag_material_index].specular_tex,   vec3(1.0));
        vec3    ems_mat =   materials[frag_material_index].emissive     * GrabTextureCoordinateColor(materials[frag_material_index].emissive_tex,   vec3(1.0));
        float   shn_mat =   materials[frag_material_index].shininess    * GrabTextureCoordinateValue(materials[frag_material_index].shininess_tex,  1.0);
        float   alf_mat =   materials[frag_material_index].alpha;

        vec3 amb_lgt = vec3(0);
        vec3 dif_lgt = vec3(0);
        vec3 spc_lgt = vec3(0);

        for (int i = 0; i < MAX_LIGHTS; i++) {

            if (lights[i].enabled == 0) continue;
            
            //light
            vec3 light_vector = lights[i].position - frag_position;
            float dist = length(light_vector);
            //if (dist > lights[i].range) continue;
            vec3 light_direction = normalize(light_vector);
            float attenuation = 1.0 / (1.0 + (dist * dist));
            vec3 light_reflection = reflect(-light_direction, normal);
            
            //ambient
            vec3 ambient = lights[i].color * lights[i].intensity;
            amb_lgt += ambient;

            //diffuse
            float alignment = max(dot(normal, light_direction), 0.0);
            vec3 diffuse = (1.0 - shadow) * alignment * lights[i].color * lights[i].intensity * attenuation;
            dif_lgt += diffuse;

            //specular
            float shine = pow(max(dot(view_direction, light_reflection), 0.0), shn_mat);
            vec3 specular = (1.0 - shadow) * shine * lights[i].color * lights[i].intensity * attenuation;
            spc_lgt += specular;

        }

        vec3 clr_end = vec3(0);

        //ambient
        vec3 amb_end = amb_lgt * amb_mat * dif_mat * ambient_intensity;
        clr_end += amb_end;

        //diffuse
        vec3 dif_end = dif_lgt * dif_mat;
        clr_end += dif_end;
        
        //specular
        vec3 spc_end = spc_lgt * spc_mat;
        clr_end += spc_end;

        //emissive
        clr_end += ems_mat;

        clr_end = clamp(clr_end, 0.0, 1.0); 
        FragColor = vec4(clr_end, alf_mat);        

    } else if (debug_mode == 1) {
    } else if (debug_mode == 2) {
    } else if (debug_mode == 3) {
    } else if (debug_mode == 4) {
    }
    
}
