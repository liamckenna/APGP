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

uniform int num_vertices;
uniform float pixel_size;
uniform mat4 MVP;

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
void cal_screen_bb(in vec4 sb_min, in vec4 sb_max, out vec3 cube_min, out vec3 cube_max)
{

	cube_min = vec3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
	cube_max = vec3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);

	// 8 vertice of the slefe box 
	for(unsigned int j = 0; j < 8; j++)
	{
		vec3 p;

		if( (j & 0x01)  != 0)
			p.x = sb_max.x;
		else
			p.x = sb_min.x;

		if( (j & 0x02) != 0)
			p.y = sb_max.y;
		else
			p.y = sb_min.y;

		if( (j & 0x04) != 0)
			p.z = sb_max.z;
		else
			p.z = sb_min.z;

		// transform the boc vertex
		vec4 tp = MVP * vec4(p, 1.0);

		vec3 ntp = (tp / tp.w).xyz;

		// update the clipping space bounding box
		cube_min = min(cube_min, ntp);
		cube_max = max(cube_max, ntp);
	}
}

vec4 cal_second_deriv(in vec4 src[GROUP_SIZE], int base_idx, int major_idx, int minor_idx, int row_maj)
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

vec4 slefe_segment_pass(
    in vec4 src[GROUP_SIZE], 
    in int patch_base_idx,
    in int row, 
    in int col, 
    in int row_maj,
    in float interp,
    inout vec4 D2b[GROUP_SIZE], 
    in bool compute_deriv,
    in int d2b_offset)
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

    vec4 seg_upper = seg;
    vec4 seg_lower = seg;

    for (int j = 0; j < 2; j++) {
        add_up_basis(
            seg_lower, seg_upper,
            D2b[d2b_offset + j * (row_maj == 1 ? 1 : 4) + patch_base_idx],
            slefe_lower_3_3[col + 4 * j].x,
            slefe_upper_3_3[col + 4 * j].x
        );
    }

    return row_maj == 1 ? seg_upper : seg_lower; // or return both if needed
}

void update_tess_level(vec3 bb_min, vec3 bb_max, float pixel_size, int group_patch_id)
{
	vec3 error_p = bb_max - bb_min;
	float max_error = max(error_p.x, max(error_p.y, error_p.z));
	float tess = min(max(3.0 * sqrt(2.0 * max_error / pixel_size), 1.0), 64.0);
	atomicMax(local_tess_level[group_patch_id], int(10000.0 * tess));
}


void main(void)
{
	uint tid = gl_GlobalInvocationID.x;
	int patch_id = int(tid / NUM_THREAD_PER_PATCH);
	int group_patch_id = int(patch_id % NUM_PATCH_PER_GROUP);
	int local_thread_id = int(tid % NUM_THREAD_PER_PATCH);

	int row = local_thread_id / 4;
	int column = local_thread_id % 4;
	
	vec4 seg_lower, seg_upper;
	vec4 final_lower, final_upper;

	if(tid >= num_vertices)
		return;

	int patch_base_idx = group_patch_id * NUM_THREAD_PER_PATCH;

	// init tess_level
	local_tess_level[group_patch_id] = 0;

	// init panel width
	local_panel_width_upper[group_patch_id*3 + local_thread_id%3] = 0;
	local_panel_width_lower[group_patch_id*3 + local_thread_id%3] = 0;

	// copy control points into shared array
	//CHANGE
	vec3 position = vec3(
	    raw_data[tid * 8 + 0],
	    raw_data[tid * 8 + 1],
	    raw_data[tid * 8 + 2]
	);
	
	// optional if needed
	vec2 texcoord = vec2(
	    raw_data[tid * 8 + 3],
	    raw_data[tid * 8 + 4]
	);
	
	vec3 normal = vec3(
	    raw_data[tid * 8 + 5],
	    raw_data[tid * 8 + 6],
	    raw_data[tid * 8 + 7]
	);
	

	int patch_vertex_base = patch_id * NUM_VERTEX_PER_PATCH;
	int cpu_style_idx = patch_vertex_base + row * 4 + column;
	cpts[local_thread_id + patch_base_idx] = vec4(position, 1.0);


	barrier();

	// calc 2nd derivative on u direction
	// input vertices are row major

	bool do_deriv = column < 2;
	float u = float(column) / 3.0f;
	upper[local_thread_id + patch_base_idx] = slefe_segment_pass(
		cpts,				// src array
		patch_base_idx,		
		row, column,		
		1,					// row major
		u,					// interpolation factor
		D2b,				// derivative output buffer
		do_deriv,
		row * 4				// D2b offset (row major)
	);
	lower[local_thread_id + patch_base_idx] = upper[local_thread_id + patch_base_idx];


	barrier();


	// calculate upper slefe of upper
	// calc 2nd derivative on u direction on v direction

	bool do_deriv_v = row < 2;
	float v = float(row) / 3.0f;
	
	final_upper = slefe_segment_pass(
	    upper,              // src array
	    patch_base_idx,
	    row, column,        //
	    0,                  // column major
	    v,                  // interpolation factor
	    D2b,                // derivative output buffer
	    do_deriv_v,
	    column              // D2b offset (column major)
	);



	barrier();

	// calculate lower slefe of lower
	// calc 2nd derivative on v direction

	bool do_deriv_v_lower = row < 2;
	v = float(row) / 3.0f;
	
	final_lower = slefe_segment_pass(
	    lower,              // source array
	    patch_base_idx,
	    row, column,        // major = row, minor = column
	    0,                  // column-major
	    v,                  // interpolation factor
	    D2b,                // derivative output buffer
	    do_deriv_v_lower,
	    column              // D2b offset (column-major)
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
		{
			int x_range = MVP[0][2] < 0 ? 1 : 0;
			int y_range = MVP[1][2] < 0 ? 1 : 0;
			int z_range = MVP[2][2] < 0 ? 1 : 0;

            vec4 center_upper = vec4(tile_upper[x_range].x, tile_upper[y_range].y, tile_upper[z_range].z, 0.0);
			vec4 center_lower = vec4(tile_lower[x_range].x, tile_lower[y_range].y, tile_lower[z_range].z, 0.0);

			vec3 center_bb_min, center_bb_max;
			cal_screen_bb(center_lower, center_upper, center_bb_min, center_bb_max);

			update_tess_level(center_bb_min, center_bb_max, pixel_size, group_patch_id);
		}
	}

	barrier();

	// calculate bounding box in screen space
	vec3 bb_min, bb_max;
	cal_screen_bb(final_lower, final_upper, bb_min, bb_max);
	

	cube_min[local_thread_id + patch_base_idx] = bb_min;
	cube_max[local_thread_id + patch_base_idx] = bb_max;

	barrier();

	update_tess_level(bb_min, bb_max, pixel_size, group_patch_id);

	if(local_thread_id == 0)
		patch_tess_levels[patch_id] = max(1.0f, local_tess_level[group_patch_id]/10000.0f);
}

