#version 450 core

layout(triangles, fractional_odd_spacing, ccw) in;

in vec3 tc_frag_position[];
in vec3 tc_frag_normal[];
in vec3 tc_frag_tangent[];
in vec2 tc_frag_tex_coord[];

patch in vec3 E01;
patch in vec3 E10;
patch in vec3 E12;
patch in vec3 E21;
patch in vec3 E20;
patch in vec3 E02;
patch in vec3 N01;
patch in vec3 N12;
patch in vec3 N20;
patch in vec3 Center;

out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_tangent;
out vec2 frag_tex_coord;


uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 UVW = gl_TessCoord;
	float U = UVW.x;
	float V = UVW.y;
	float W = UVW.z;


	vec3 P0 = tc_frag_position[0];
	vec3 P1 = tc_frag_position[1];
	vec3 P2 = tc_frag_position[2];

	vec3 N0 = normalize(tc_frag_normal[0]);
	vec3 N1 = normalize(tc_frag_normal[1]);
	vec3 N2 = normalize(tc_frag_normal[2]);

	vec3 pos = (U * U * U * P0) + (V * V * V * P1) + (W * W * W * P2)
				+ (3.0 * U * U * V * E01) + (3.0 * U * V * V * E10)
				+ (3.0 * V * V * W * E12) + (3.0 * V * W * W * E21)
				+ (3.0 * U * W * W * E20) + (3.0 * U * U * W * E02)
				+ (6.0 * U * W * V * Center);

	vec3 nrm = N0 * U * U + N1 * V * V + N2 * W * W
             + 2 * U * V * N01 + 2 * V * W * N12 + 2 * U * W * N20;
	nrm = normalize(nrm);
    vec3 tan = U * tc_frag_tangent[0]  + V * tc_frag_tangent[1]  + W * tc_frag_tangent[2];
    vec2 uv  = U * tc_frag_tex_coord[0] + V * tc_frag_tex_coord[1] + W * tc_frag_tex_coord[2];

	frag_position  = pos;
    frag_normal    = nrm;
    frag_tangent   = tan;
    frag_tex_coord = uv;

    gl_Position = projection * view * vec4(pos, 1.0);
}
