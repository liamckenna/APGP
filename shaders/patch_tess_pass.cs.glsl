//patch_tess_pass.glsl
#version 430

precision highp float;

#define NUM_VERTEX_PER_PATCH 16
#define NUM_PATCH_PER_GROUP 2
#define NUM_THREAD_PER_PATCH 16
#define GROUP_SIZE (NUM_PATCH_PER_GROUP*NUM_VERTEX_PER_PATCH)
#define NUM_TILES_PER_PATCH 9

layout( local_size_x = 32,  local_size_y = 1, local_size_z = 1 ) in;

struct Vertex { 
	vec3 position;	// position
	float pad0;		// pad after vec3
	vec2 texcoord;	// texcoord
	vec2 pad1;      // pad after vec2
	vec3 normal;	// normal
	float pad2;     // pad after vec3
};

layout(std430, binding = 0) readonly buffer VertexBuffer {
    float raw_data[];
};

layout(std430, binding = 1) writeonly buffer PatchBuffer { 
	float patch_tess_levels[ ]; // unsized array allowed at end of buffer 
};

layout(std430, binding = 2) writeonly buffer DebugBuffer { 
	vec4 debug_value[ ]; // unsized array allowed at end of buffer 
};

layout(std430, binding = 4) writeonly buffer LightDebugBuffer { 
	vec4 light_debug_value[ ]; // unsized array allowed at end of buffer 
};

layout(std430, binding = 3) buffer LightMatrixBuffer {
    mat4 light_MVPs[];
};

layout(std430, binding = 5) writeonly buffer InShadow {
	uint in_shadow[];
};

layout(std430, binding = 6) readonly buffer LaunchPoint {
	uint launch_points[];
};

layout(std430, binding = 7) writeonly buffer CurrentDepth {
	uint depth_value[];
};

layout(std430, binding = 8) writeonly buffer BufferSpan {
	uvec4 pixel_span[]; // (min x, max x, min y, max y)
};

uniform int surface_id;

mat4 light_MVP = light_MVPs[surface_id];
uint launch_point = launch_points[surface_id];

layout(binding = 0, r32ui) uniform uimage2D depth_texture;

uniform int num_vertices;
uniform float pixel_size;
uniform mat4 MVP;
uniform int light_pass;
uniform ivec2 shadow_res = ivec2(1080, 1080);
uniform uint shadow_buffer_launch_point;

uniform float slefe_lower_3_3[] = {
	// base 1
	0.0000000000,
	-.3703703704,
	-.2962962963,
	0.0000000000,
	// base 2
	0.0000000000,
	-.2962962963,
	-.3703703704,
	0.0000000000
};

uniform float slefe_upper_3_3[] = {
	// base 1
	-.0695214343,
	-.4398918047,
	-.3153515940,
	-.0087327217,
	// base 2
	-.0087327217,
	-.3153515940,
	-.4398918047,
	-.0695214343
};

// indices of the vertex of each bi-linear "tile"
uniform int tile_indices[9][4] = { 
		{ 0,1,4,5},
		{ 1,2,5,6},
		{ 2,3,6,7},
		{ 4,5,8,9},
		{ 5,6,9,10},
		{ 6,7,10,11},
		{ 8,9,12,13},
		{ 9,10,13,14},
		{ 10,11,14,15}
};

// group variable

shared vec4 cpts[GROUP_SIZE];

shared vec4 upper[GROUP_SIZE];
shared vec4 lower[GROUP_SIZE];

shared vec4 D2b[GROUP_SIZE];

shared vec3 cube_min[GROUP_SIZE];
shared vec3 cube_max[GROUP_SIZE];

shared int local_tess_level[NUM_PATCH_PER_GROUP];

// width of the flat enclosure of each bi-cubic panel
shared int local_panel_width_upper[3*NUM_PATCH_PER_GROUP];
shared int local_panel_width_lower[3*NUM_PATCH_PER_GROUP];


const float MAX_FLOAT = 10000.0f;

void add_up_basis(inout vec4 seg_lower, inout vec4 seg_upper, in vec4 d2b, in float lcoeff, in float ucoeff)
{
		if(d2b.x > 0)
		{
			seg_upper.x += lcoeff * d2b.x;
			seg_lower.x += ucoeff * d2b.x;
		}
		else
		{
			seg_upper.x += ucoeff * d2b.x;
			seg_lower.x += lcoeff * d2b.x;
		}

		if(d2b.y > 0)
		{
			seg_upper.y += lcoeff * d2b.y;
			seg_lower.y += ucoeff * d2b.y;
		}
		else
		{
			seg_upper.y += ucoeff * d2b.y;
			seg_lower.y += lcoeff * d2b.y;
		}

		if(d2b.z > 0)
		{
			seg_upper.z += lcoeff * d2b.z;
			seg_lower.z += ucoeff * d2b.z;
		}
		else
		{
			seg_upper.z += ucoeff * d2b.z;
			seg_lower.z += lcoeff * d2b.z;
		}
}

// input: slefe box
// ouput: bounding box in screen space
void calculate_bb(in vec4 sb_min, in vec4 sb_max, in mat4 mvp_matrix, out vec3 cube_min, out vec3 cube_max)
{

	cube_min = vec3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
	cube_max = vec3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);

	
	vec3 p;
	
	for (int j = 0; j < 8; ++j) {
	    vec3 p;
	    p.x = (j & 0x01) != 0 ? sb_max.x : sb_min.x;
	    p.y = (j & 0x02) != 0 ? sb_max.y : sb_min.y;
	    p.z = (j & 0x04) != 0 ? sb_max.z : sb_min.z;
	
		// transform the boc vertex
		vec4 tp = mvp_matrix * vec4(p, 1.0);
	
		vec3 ntp = (tp / tp.w).xyz;
	
		// update the clipping space bounding box
		cube_min = min(cube_min, ntp);
		cube_max = max(cube_max, ntp);
	}


	if (light_pass == 1 || mvp_matrix == light_MVP) {
		vec3 extent = cube_max - cube_min;
		uint tid = gl_GlobalInvocationID.x;
		int patch_id = int(tid / NUM_THREAD_PER_PATCH);
		int group_patch_id = int(patch_id % NUM_PATCH_PER_GROUP);
		int patch_base_idx = group_patch_id * NUM_THREAD_PER_PATCH;
		if (all(lessThanEqual(abs(extent), vec3(1e-6)))) {
		    cube_min = vec3(MAX_FLOAT);
		    cube_max = vec3(-MAX_FLOAT);
		
		    for (int i = 0; i < 16; ++i) {
		        vec4 tp = mvp_matrix * cpts[patch_base_idx + i];
		        vec3 ntp = (tp / tp.w).xyz;
		        cube_min = min(cube_min, ntp);
		        cube_max = max(cube_max, ntp);
		    }
		}
	}

	
}

vec4 cal_second_deriv (in vec4 src[GROUP_SIZE], int base_idx, int major_idx, int minor_idx, int row_maj)
{
    int idx0, idx1, idx2;

    if (row_maj ==1) {
        idx0 = base_idx + 4 * major_idx + minor_idx;
        idx1 = base_idx + 4 * major_idx + minor_idx + 1;
        idx2 = base_idx + 4 * major_idx + minor_idx + 2;
    } else {
        idx0 = base_idx + 4 * (major_idx + 0) + minor_idx;
        idx1 = base_idx + 4 * (major_idx + 1) + minor_idx;
        idx2 = base_idx + 4 * (major_idx + 2) + minor_idx;
    }

    return src[idx0] - 2.0 * src[idx1] + src[idx2];
}

void slefe_segment_pass(
    in vec4 src[GROUP_SIZE], 
    in int patch_base_idx,
    in int row, 
    in int col, 
    in int row_maj,
    in float interp,
    inout vec4 D2b[GROUP_SIZE], 
    in bool compute_deriv,
    in int d2b_offset,
    out vec4 seg_lower,
    out vec4 seg_upper
)
{
    if (compute_deriv) {
        D2b[col + row * 4 + patch_base_idx] = cal_second_deriv(src, patch_base_idx, row, col, row_maj);
    }

    barrier();

    vec4 seg = mix(
        src[row_maj == 1 ? row * 4 + patch_base_idx : col + patch_base_idx],
        src[row_maj == 1 ? row * 4 + 3 + patch_base_idx : row * 4 + col + patch_base_idx],
        interp
    );

    vec4 segment_upper = seg;
    vec4 segment_lower = seg;

	float segment_length = length(
	    src[row_maj == 1 ? row * 4 + 3 + patch_base_idx : row * 4 + col + patch_base_idx].xyz -
	    src[row_maj == 1 ? row * 4 + patch_base_idx : col + patch_base_idx].xyz
	);

	float basis_scale = segment_length * segment_length;


	//debugging
	uint tid = gl_GlobalInvocationID.x;
	int patch_id = int(tid / NUM_THREAD_PER_PATCH);
	if (row_maj == 1){
	
	}
	else {
	

	}
	

    for (int j = 0; j < 2; j++) {
		add_up_basis(
		    segment_lower, segment_upper,
		    D2b[d2b_offset + j * (row_maj == 1 ? 1 : 4) + patch_base_idx] * basis_scale,
		    slefe_lower_3_3[col + 4 * j],
		    slefe_upper_3_3[col + 4 * j]
		);
    }

    seg_lower = segment_lower;
	seg_upper = segment_upper;
}

void update_tess_level(vec3 bb_min, vec3 bb_max, float pixel_size, int group_patch_id)
{
	vec3 error_p = bb_max - bb_min;
	float max_error = max(error_p.x, max(error_p.y, error_p.z));
	float tess = min(max(3.0 * sqrt(2.0 * max_error / pixel_size), 1.0), 64.0);
	atomicMax(local_tess_level[group_patch_id], int(10000.0 * tess));
}

void check_patch_offscreen (
    int local_thread_id,
    int patch_base_idx,
    int patch_id)
{
    if (local_thread_id != 0) return;

    vec3 patch_bb_min = vec3(MAX_FLOAT);
    vec3 patch_bb_max = vec3(-MAX_FLOAT);

    for (int i = 0; i < 16; ++i)
    {
        patch_bb_min = min(patch_bb_min, cube_min[patch_base_idx + i]);
        patch_bb_max = max(patch_bb_max, cube_max[patch_base_idx + i]);
    }

    bool offscreen =
        patch_bb_max.x < -1.0 || patch_bb_min.x > 1.0 ||
        patch_bb_max.y < -1.0 || patch_bb_min.y > 1.0 ||
        patch_bb_max.z < -1.0 || patch_bb_min.z > 1.0;

    if (offscreen)
    {
        patch_tess_levels[patch_id] = 0.0; // force minimum tess
    }
}

void main(void)
{
	
	uint tid = gl_GlobalInvocationID.x;
	
	int patch_id = int(tid / NUM_THREAD_PER_PATCH);
	int group_patch_id = int(patch_id % NUM_PATCH_PER_GROUP);
	int patch_base_idx = group_patch_id * NUM_THREAD_PER_PATCH;
	
	int local_thread_id = int(tid % NUM_THREAD_PER_PATCH);
	int row = local_thread_id / 4;
	int column = local_thread_id % 4;

	// convert raw data to ctrl point info
	vec3 position = vec3(
	    raw_data[tid * 8 + 0],
	    raw_data[tid * 8 + 1],
	    raw_data[tid * 8 + 2]
	);
	
	// optional if needed
	/*
	vec2 texcoord = vec2(
	    raw_data[tid * 8 + 3],
	    raw_data[tid * 8 + 4]
	);
	
	vec3 normal = vec3(
	    raw_data[tid * 8 + 5],
	    raw_data[tid * 8 + 6],
	    raw_data[tid * 8 + 7]
	);
	*/




	
	vec4 seg_lower, seg_upper;
	vec4 final_lower, final_upper;
	vec4 extra_lower, extra_upper;

	if(tid >= num_vertices)
		return;

	// init tess_level
	local_tess_level[group_patch_id] = 0;

	// init panel width
	local_panel_width_upper[group_patch_id*3 + local_thread_id%3] = 0;
	local_panel_width_lower[group_patch_id*3 + local_thread_id%3] = 0;

	int patch_vertex_base = patch_id * NUM_VERTEX_PER_PATCH;
	int cpu_style_idx = patch_vertex_base + row * 4 + column;
	cpts[local_thread_id + patch_base_idx] = vec4(position, 1.0);

	

	barrier();

	// calc 2nd derivative on u direction
	// input vertices are row major

	bool do_deriv = column < 2;
	float u = float(column) / 3.0f;
	slefe_segment_pass(
	    cpts,
	    patch_base_idx,
	    row, column,
	    1,                              // row-major
	    float(column) / 3.0f,
	    D2b,
	    column < 2,
	    row * 4,
	    lower[local_thread_id + patch_base_idx],
	    upper[local_thread_id + patch_base_idx]
	);

	barrier();


	// calculate upper slefe of upper
	// calc 2nd derivative on u direction on v direction

	bool do_deriv_v = row < 2;
	float v = float(row) / 3.0f;
	
	slefe_segment_pass(
	    lower,
	    patch_base_idx,
	    row, column,
	    0,                              // column-major
	    float(row) / 3.0f,
	    D2b,
	    row < 2,
	    column,
	    final_lower,
	    final_upper
	);

	/* 
	* note that for bivariate the bound is bi-linear instead of linear
	* the width of the flat enclosure (triangle) of bi-linear bound 
	* should also be taken into account
	*/



	// write the upper and lower of each thread(vertex) 
	// back into the upper/lower array, which is used later
	// when computing tile center vertex. This is not necessary if 
	// only estimate width at tile coner.

	upper[local_thread_id + patch_base_idx] = final_upper;
	lower[local_thread_id + patch_base_idx] = final_lower;

	barrier();
	if (light_pass == 0) {

		float max_upper_panel_width[3] = {0,0,0};
		float max_lower_panel_width[3] = {0,0,0};

		vec4 tile_upper[2];
		vec4 tile_lower[2];
		if(local_thread_id < 9) // for each tile
		{

			int i1 = tile_indices[local_thread_id][0];
			int i2 = tile_indices[local_thread_id][1];
			int i3 = tile_indices[local_thread_id][2];
			int i4 = tile_indices[local_thread_id][3];

			vec4 u1 = (upper[i1] + upper[i4])/2.0f;
			vec4 u2 = (upper[i2] + upper[i3])/2.0f;

			vec4 l1 = (lower[i1] + lower[i4])/2.0f;
			vec4 l2 = (lower[i2] + lower[i3])/2.0f;

			tile_upper[0] = (u1 + u2) / 2.0f;
			tile_lower[0] = min(l1,l2);

			tile_upper[1] = max(u1,u2); 
			tile_lower[1] = (l1 + l2) / 2.0f;

		    /* get the 8 corner of the average center box and find the one
		     * that closest to the camera.
		     *
		     * This is determined purely by the MVP matrix.
		     */ 
			int x_range, y_range, z_range;
			if (light_pass == 0) {
				x_range = MVP[0][2] < 0 ? 1 : 0;
				y_range = MVP[1][2] < 0 ? 1 : 0;
				z_range = MVP[2][2] < 0 ? 1 : 0;
			} else {
				x_range = light_MVP[0][2] < 0 ? 1 : 0;
				y_range = light_MVP[1][2] < 0 ? 1 : 0;
				z_range = light_MVP[2][2] < 0 ? 1 : 0;
			}

		    vec4 center_upper = vec4(tile_upper[x_range].x, tile_upper[y_range].y, tile_upper[z_range].z, 0.0);
			vec4 center_lower = vec4(tile_lower[x_range].x, tile_lower[y_range].y, tile_lower[z_range].z, 0.0);

			vec3 center_bb_min, center_bb_max;
			calculate_bb(center_lower, center_upper, MVP, center_bb_min, center_bb_max);

			update_tess_level(center_bb_min, center_bb_max, pixel_size, group_patch_id);
		}
	}

	barrier();

	if (light_pass == 0) {

		vec3 bb_min, bb_max;

		calculate_bb(final_lower, final_upper, MVP, bb_min, bb_max);

		cube_min[local_thread_id + patch_base_idx] = bb_min;
		cube_max[local_thread_id + patch_base_idx] = bb_max;

		barrier();

		update_tess_level(bb_min, bb_max, pixel_size, group_patch_id);

		if(local_thread_id == 0) {
			patch_tess_levels[patch_id] = max(1.0f, local_tess_level[group_patch_id]/10000.0f);
			check_patch_offscreen(local_thread_id, patch_base_idx, patch_id);
		}

		barrier();

		if (local_thread_id == 0) {
			
			vec3 light_bb_min, light_bb_max;
			calculate_bb(final_lower, final_upper, light_MVP, light_bb_min, light_bb_max);

			float min_ndc_z = MAX_FLOAT;

			for (int j = 0; j < 8; ++j) {
			    vec3 p;
			    p.x = (j & 0x01) != 0 ? light_bb_max.x : light_bb_min.x;
			    p.y = (j & 0x02) != 0 ? light_bb_max.y : light_bb_min.y;
			    p.z = (j & 0x04) != 0 ? light_bb_max.z : light_bb_min.z;
			
			    min_ndc_z = min(min_ndc_z, p.z);
			}

			vec2 uv_min = light_bb_min.xy * 0.5 + 0.5;
			vec2 uv_max = light_bb_max.xy * 0.5 + 0.5;

			ivec2 px_min = ivec2(floor(uv_min * vec2(shadow_res)));
			ivec2 px_max = ivec2(ceil(uv_max * vec2(shadow_res)));

			px_min = clamp(px_min, ivec2(0), shadow_res - 1);
			px_max = clamp(px_max, ivec2(0), shadow_res - 1);

			ivec2 px = ivec2((px_min.x + px_max.x) / 2, (px_min.y + px_max.y) / 2);
			
			float current_depth = min_ndc_z + 1.0;
			
			uint stored = imageLoad(depth_texture, px).r;
			float stored_depth = uintBitsToFloat(stored);

			float bias = 0.005;
			int shadow = current_depth - bias > stored_depth ? 1 : 0;
			in_shadow[patch_id] = shadow;
			//patch_tess_levels[patch_id] = shadow == 1 ? 64.0 : 1.0;
			

			debug_value[patch_id].x = px.x;
			debug_value[patch_id].y = px.y;
			debug_value[patch_id].z = current_depth;
			debug_value[patch_id].w = stored_depth;

		}

	} else if (local_thread_id == 0) {
	
		vec3 light_bb_min, light_bb_max;
		calculate_bb(final_lower, final_upper, light_MVP, light_bb_min, light_bb_max);

		float min_ndc_z = MAX_FLOAT;

		for (int j = 0; j < 8; ++j) {
		    vec3 p;
		    p.x = (j & 0x01) != 0 ? light_bb_max.x : light_bb_min.x;
		    p.y = (j & 0x02) != 0 ? light_bb_max.y : light_bb_min.y;
		    p.z = (j & 0x04) != 0 ? light_bb_max.z : light_bb_min.z;
		
		    min_ndc_z = min(min_ndc_z, p.z);
		}

		vec2 uv_min = light_bb_min.xy * 0.5 + 0.5;
		vec2 uv_max = light_bb_max.xy * 0.5 + 0.5;

		ivec2 px_min = ivec2(floor(uv_min * vec2(shadow_res)));
		ivec2 px_max = ivec2(ceil(uv_max * vec2(shadow_res)));

		px_min = clamp(px_min, ivec2(0), shadow_res - 1);
		px_max = clamp(px_max, ivec2(0), shadow_res - 1);

		ivec2 px = ivec2((px_min.x + px_max.x) / 2, (px_min.y + px_max.y) / 2);
		
		float current_depth = min_ndc_z + 1.0;
		
		uint stored = imageLoad(depth_texture, px).r;
		float stored_depth = uintBitsToFloat(stored);

		float bias = 0.005;
		int shadow = current_depth - bias > stored_depth ? 1 : 0;
		in_shadow[launch_point + patch_id] = shadow;

		light_debug_value[patch_id].x = launch_point;
		light_debug_value[patch_id].y = patch_id;
		light_debug_value[patch_id].z = launch_point + patch_id;
		light_debug_value[patch_id].w = shadow;
		
		uint depth_bits = floatBitsToUint(current_depth);

		depth_value[patch_id] = depth_bits;

		pixel_span[patch_id].x = px_min.x;
		pixel_span[patch_id].y = px_max.x;
		pixel_span[patch_id].z = px_min.y;
		pixel_span[patch_id].w = px_max.y;

		
		for (int y = px_min.y; y <= px_max.y; ++y) {
			for (int x = px_min.x; x <= px_max.x; ++x) {
			    ivec2 px = ivec2(x, y);
				imageAtomicMin(depth_texture, px, depth_bits);
			}
		}
		
	}	

}

