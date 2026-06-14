#version 450 core

layout(vertices = 3) out;

in vec3 frag_position[];
in vec3 frag_normal[];
in vec3 frag_tangent[];
in vec2 frag_tex_coord[];

out vec3 tc_frag_position[];
out vec3 tc_frag_normal[];
out vec3 tc_frag_tangent[];
out vec2 tc_frag_tex_coord[];

patch out vec3 E01;
patch out vec3 E10;
patch out vec3 E12;
patch out vec3 E21;
patch out vec3 E20;
patch out vec3 E02;
patch out vec3 N01;
patch out vec3 N12;
patch out vec3 N20;
patch out vec3 Center;

uniform vec3 view_position;

int MAX_LEVEL = 4;
int MIN_LEVEL = 1;
float NEAR_DISTANCE = 0.1;
float FAR_DISTANCE = 3.0;

void main()
{
    tc_frag_position[gl_InvocationID]  = frag_position[gl_InvocationID];
    tc_frag_normal[gl_InvocationID]    = frag_normal[gl_InvocationID];
    tc_frag_tangent[gl_InvocationID]   = frag_tangent[gl_InvocationID];
    tc_frag_tex_coord[gl_InvocationID] = frag_tex_coord[gl_InvocationID];

    if (gl_InvocationID == 0)
    {
        float levels[3];
        for (int i = 0; i < 3; i++)
        {
            int other_idx[2];
            if (i == 0)
            {
                other_idx[0] = 1;
                other_idx[1] = 2;
            }
            else if (i == 1)
            {
                other_idx[0] = 2;
                other_idx[1] = 0;
            }
            else if (i == 2)
            {
                other_idx[0] = 0;
                other_idx[1] = 1;
            }
            vec3 measuring_point = (frag_position[other_idx[0]] + frag_position[other_idx[1]]) / 2.0;
            float view_distance = clamp(abs(length(measuring_point - view_position)), NEAR_DISTANCE, FAR_DISTANCE);
            float normalized_distance = (view_distance - NEAR_DISTANCE) / (FAR_DISTANCE - NEAR_DISTANCE);
            float inverted_distance = (normalized_distance - 1.0) * -1.0;
            levels[i] = inverted_distance * (MAX_LEVEL - MIN_LEVEL) + MIN_LEVEL;
            gl_TessLevelOuter[i] = levels[i];
        }
        float inner_level = max(max(levels[0], levels[1]), levels[2]);
        gl_TessLevelInner[0] = inner_level;

        vec3 P0 = frag_position[0];
        vec3 P1 = frag_position[1];
        vec3 P2 = frag_position[2];
        vec3 N0 = normalize(frag_normal[0]);
        vec3 N1 = normalize(frag_normal[1]);
        vec3 N2 = normalize(frag_normal[2]);

        E01 = (2.0 * P0 + P1 - dot(P1 - P0, N0) * N0) / 3.0;
	    E10 = (2.0 * P1 + P0 - dot(P0 - P1, N1) * N1) / 3.0;

	    E12 = (2.0 * P1 + P2 - dot(P2 - P1, N1) * N1) / 3.0;
	    E21 = (2.0 * P2 + P1 - dot(P1 - P2, N2) * N2) / 3.0;

	    E20 = (2.0 * P2 + P0 - dot(P0 - P2, N2) * N2) / 3.0;
	    E02 = (2.0 * P0 + P2 - dot(P2 - P0, N0) * N0) / 3.0;
	    
	    vec3 Edge = (E01 + E10 + E12 + E21 + E20 + E02) / 6.0;
	    vec3 Vert = (P0 + P1 + P2) / 3.0;
	    Center = Edge + (Edge - Vert) / 2.0;

        N01 = normalize(N0 + N1 - (2.0*dot(P1-P0, N0+N1)/dot(P1-P0, P1-P0)) * (P1-P0));
        N12 = normalize(N1 + N2 - (2.0*dot(P2-P1, N1+N2)/dot(P2-P1, P2-P1)) * (P2-P1));
        N20 = normalize(N2 + N0 - (2.0*dot(P0-P2, N2+N0)/dot(P0-P2, P0-P2)) * (P0-P2));
    }
}
