//patch_depth_buffer.cs.glsl
#version 430

layout(local_size_x = 1, local_size_y = 1) in;

layout(std430, binding = 0) buffer DepthValues {
    uint depth_values[];
};
layout(std430, binding = 1) buffer SpanValues {
    uvec4 span_values[];
};
layout(std430, binding = 2) buffer LaunchPoints {
    uint launch_points[];
};
layout(std430, binding = 3) buffer ShadowValues {
    uint shadow_values[];
};
layout(std430, binding = 4) buffer DebugBuffer {
    vec4 debug[];
};


layout(r32ui, binding = 0) uniform uimage2D depth_texture;

uniform int global_patch_id;

uniform int surface;

uniform int access_mode; //0 = WRITE, 1 = READ

void main() {
    
    if (access_mode == 0) {
        //pass in our patch_id as a uniform for these calls as one patch is dispatched at a time when writing
        uvec2 offset = span_values[global_patch_id].xz;
        uint depth = depth_values[global_patch_id];

        ivec2 px = ivec2(offset) + ivec2(gl_GlobalInvocationID.xy);

        imageAtomicMin(depth_texture, px, depth);
    } else if (access_mode == 1) {
        //must compute patch_id here. uniform is not an option since multiple patches are dispatched at a time when reading
        uint local_patch_id = gl_GlobalInvocationID.x;
        uint launch_point = launch_points[surface];
        uint patch_id = launch_point + local_patch_id;
        uint center_x = (span_values[patch_id].x + span_values[patch_id].y) / 2;
        uint center_y = (span_values[patch_id].z + span_values[patch_id].w) / 2;
        ivec2 patch_center = ivec2(center_x, center_y);

        uint stored = imageLoad(depth_texture, patch_center).r;
		float stored_depth = uintBitsToFloat(stored);

        uint current = depth_values[patch_id];
        float current_depth = uintBitsToFloat(current);
        
        float bias = 0.005;
		uint shadow = current_depth - bias > stored_depth ? 1 : 0;
		shadow_values[patch_id] = shadow;

        debug[local_patch_id].x = patch_id;
        debug[local_patch_id].y = uintBitsToFloat(depth_values[patch_id]);
        debug[local_patch_id].z = current_depth;
        debug[local_patch_id].w = stored_depth;
    }
}
