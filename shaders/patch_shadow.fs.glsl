#version 430
layout(r32ui, binding = 0) uniform uimage2D shadowMap;

void main() {
    uint encodedDepth = floatBitsToUint(gl_FragCoord.z);
    ivec2 coord = ivec2(gl_FragCoord.xy);
    imageAtomicMin(shadowMap, coord, encodedDepth);
}
