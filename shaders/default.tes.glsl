#version 450 core

layout(quads, fractional_even_spacing, cw) in;

uniform int surface_type;

// per-control-point data from TCS
in  vec3 tc_frag_position[];
in  vec3 tc_frag_normal[];
in  vec2 tc_frag_tex_coord[];

// passed to your fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_tex_coord;

// your camera matrices
uniform mat4 view;
uniform mat4 projection;

// evaluate a cubic Bézier
vec3 bezier3(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
    float u = 1.0 - t;
    return p0*(u*u*u)
         + p1*(3.0*u*u*t)
         + p2*(3.0*u*t*t)
         + p3*(t*t*t);
}

// derivative of a cubic Bézier
vec3 bezier3Deriv(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
    float u = 1.0 - t;
    // 3 [ (1−t)²(p1−p0) + 2(1−t)t(p2−p1) + t²(p3−p2) ]
    return 3.0 * ( u*u*(p1 - p0)
                 + 2.0*u*t*(p2 - p1)
                 +   t*t*(p3 - p2) );
}

// same for vec2 UVs
vec2 bezier3(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float t) {
    float u = 1.0 - t;
    return p0*(u*u*u)
         + p1*(3.0*u*u*t)
         + p2*(3.0*u*t*t)
         + p3*(t*t*t);
}

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // 1) sample along u for each of the 4 rows
    vec3  uPos[4];
    vec3  uDer[4];
    vec2  uUV [4];
    for (int i = 0; i < 4; ++i) {
        int idx = i*4;
        uPos[i] = bezier3(
            tc_frag_position[idx+0],
            tc_frag_position[idx+1],
            tc_frag_position[idx+2],
            tc_frag_position[idx+3],
            u
        );
        uDer[i] = bezier3Deriv(
            tc_frag_position[idx+0],
            tc_frag_position[idx+1],
            tc_frag_position[idx+2],
            tc_frag_position[idx+3],
            u
        );
        uUV[i]  = bezier3(
            tc_frag_tex_coord[idx+0],
            tc_frag_tex_coord[idx+1],
            tc_frag_tex_coord[idx+2],
            tc_frag_tex_coord[idx+3],
            u
        );
    }

    // 2) blend those along v for position & UV
    vec3 posWorld = bezier3(uPos[0], uPos[1], uPos[2], uPos[3], v);
    frag_position = posWorld;
    frag_tex_coord = bezier3(uUV[0], uUV[1], uUV[2], uUV[3], v);

    // 3) compute partials ∂P/∂u and ∂P/∂v
    // ∂P/∂u = blend the u-derivatives across v
    vec3 dPdu = bezier3(uDer[0], uDer[1], uDer[2], uDer[3], v);
    // ∂P/∂v = derivative along v of the uPos samples
    vec3 dPdv = bezier3Deriv(uPos[0], uPos[1], uPos[2], uPos[3], v);

    // 4) normal
    frag_normal = normalize(cross(dPdu, dPdv));

    // 5) project
    gl_Position = projection * view * vec4(posWorld, 1.0);
}
