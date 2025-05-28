#version 450 core

// consume 16 control points, emit 16
layout(vertices = 16) out;

// from VS
in vec3 frag_position[];
in vec3 frag_normal[];
in vec2 frag_tex_coord[];

// to TES
out vec3 tc_frag_position[];
out vec3 tc_frag_normal[];
out vec2 tc_frag_tex_coord[];

// gl_Position passthrough
in  gl_PerVertex { vec4 gl_Position; } gl_in[];
out gl_PerVertex { vec4 gl_Position; } gl_out[];

uniform int surface_type;

void main() {

    // copy clip‐space pos
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // copy our per‐vertex varyings
    tc_frag_position[gl_InvocationID] = frag_position[gl_InvocationID];
    tc_frag_normal[gl_InvocationID]   = frag_normal[gl_InvocationID];
    tc_frag_tex_coord[gl_InvocationID]= frag_tex_coord[gl_InvocationID];

    // only once, set tess levels
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = 8.0;
        gl_TessLevelInner[1] = 8.0;
        gl_TessLevelOuter[0] = 8.0;
        gl_TessLevelOuter[1] = 8.0;
        gl_TessLevelOuter[2] = 8.0;
        gl_TessLevelOuter[3] = 8.0;
    }
}
