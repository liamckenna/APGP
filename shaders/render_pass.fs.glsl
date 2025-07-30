#version 430
// #extension GL_ARB_shading_language_include : require

// #include "/setting.h"

// input data

layout(std430, binding = 3) buffer LightMVPBuffer {
    mat4 light_mvps[];
};

layout(r32ui, binding = 0) uniform uimage2D depth_texture;

uniform int surface_id;

in vec4 tePosition;
in vec4 teDu;
in vec4 teDv;
flat in uint tePatch;

uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

uniform mat4 ModelViewInverse;

// lighting
uniform vec4 light_pos = vec4(0,10,0,1.0);
uniform vec3 light_dir = vec3(0, -1, 0);
uniform float light_intensity = 1.0;

// Material
uniform vec3 Ka = vec3(0.2,0.2,0.2);
uniform vec3 Kd = vec3(0.9,0.8,0.5);
uniform vec3 Ks = vec3(0.5,0.5,0.5);
uniform float Shininess = 50;


//local variables
// Ouput data
layout(location = 0) out vec4 outColor;

bool inShadow (mat4 light_mvp) {
    vec4 worldPos = ModelViewInverse * tePosition;
    vec4 light_clip_pos = light_mvps[surface_id] * worldPos;
    vec3 light_ndc = light_clip_pos.xyz / light_clip_pos.w;
    vec2 shadow_uv = light_ndc.xy * 0.5 + 0.5;
    shadow_uv = clamp(shadow_uv, 0.0, 1.0);

    ivec2 texSize = imageSize(depth_texture);
    ivec2 texel_coord = ivec2(shadow_uv * texSize);
    texel_coord = clamp(texel_coord, ivec2(0), texSize - 1);

    uint raw_depth = imageLoad(depth_texture, texel_coord).r;
    float closestDepth = uintBitsToFloat(raw_depth);
    float currentDepth = light_ndc.z * 0.5 + 0.5;
    float bias = 0.0025;
    bool shadow = currentDepth - bias > closestDepth;
    return shadow;
}
 
vec3 phongModelDiffAndSpec(vec3 in_normal, vec3 kd)
{
    vec3 n = normalize(in_normal);
	vec4 ClippingSpacePosition = vec4(tePosition.xyz,1.0);

    vec3 s = normalize(-light_dir);
    vec3 v = normalize(-ClippingSpacePosition.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( abs(dot(s,n)), 0.0 );
    vec3 diffuse = light_intensity * kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = light_intensity * Ks *
            pow( max( dot(r,v), 0.0 ), Shininess );
    
    if (inShadow(light_mvps[surface_id])) {
        diffuse = vec3(0);
        spec = vec3(0);
    }

    return diffuse + spec + Ka;
}
 
void main()
{
	vec3 tan_v = teDv.xyz;
	vec3 tan_u = teDu.xyz;
	vec3 normal = NormalMatrix * normalize(cross(tan_v,tan_u));

	outColor = vec4(phongModelDiffAndSpec(normal, Kd), 1.0);	
}