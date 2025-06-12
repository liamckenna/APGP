#version 430
// #extension GL_ARB_shading_language_include : require

// #include "/setting.h"

// input data

in vec4 tePosition;
in vec4 teDu;
in vec4 teDv;


uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

// lighting
uniform vec4 light_pos = vec4(10,10,10,1.0);
uniform float light_intensity = 1.0;

// Material
uniform vec3 Ka = vec3(0.2,0.2,0.2);
uniform vec3 Kd = vec3(0.9,0.8,0.5);
uniform vec3 Ks = vec3(0.5,0.5,0.5);
uniform float Shininess = 50;


//local variables
// Ouput data
layout(location = 0) out vec4 outColor;


vec3 phongModelDiffAndSpec(vec3 in_normal, vec3 kd)
{
    vec3 n = normalize(in_normal);
	vec4 ClippingSpacePosition = vec4(tePosition.xyz,1.0);

    vec3 s = normalize((light_pos).xyz - ClippingSpacePosition.xyz);
    vec3 v = normalize(-ClippingSpacePosition.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( abs(dot(s,n)), 0.0 );
    vec3 diffuse = light_intensity * kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = light_intensity * Ks *
            pow( max( dot(r,v), 0.0 ), Shininess );

    return diffuse + spec + Ka;
	//return vec3(sDotN,sDotN,sDotN);
}


 
void main(){

	//outColor = gColor;

	vec3 tan_v = teDv.xyz;
	vec3 tan_u = teDu.xyz;
	vec3 normal = NormalMatrix * normalize(cross(tan_v,tan_u));

	outColor = vec4(phongModelDiffAndSpec(normal, Kd), 1.0);
	
}