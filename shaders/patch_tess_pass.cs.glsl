#version 430

precision highp float;

#define NUM_VERTEX_PER_PATCH 16
#define NUM_PATCH_PER_GROUP 2
#define NUM_THREAD_PER_PATCH 16
#define GROUP_SIZE (NUM_PATCH_PER_GROUP*NUM_VERTEX_PER_PATCH)
#define NUM_TILES_PER_PATCH 9

layout( local_size_x = 32,  local_size_y = 1, local_size_z = 1 )   in;

struct Vertex { 
	vec4 XYZW;	// position
	vec4 RGBA;	// color
	vec4 ST;	// tex_coord
	//vec4 Du;	// tex_coord
	//vec4 Dv;	// tex_coord
};
 
layout(std430, binding = 0) readonly buffer InputBuffer { 
	Vertex Vertices[ ]; // unsized array allowed at end of buffer 
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
	cpts[local_thread_id + patch_base_idx] = Vertices[tid].XYZW;	

	barrier();

	// calc 2nd derivative on u direction
	// input vertices are row major

	if(column < 2)
	{
		D2b[column + row*4 + patch_base_idx] = cpts[4*row + column + patch_base_idx] 
								- 2.0f * cpts[4*row + column + 1 + patch_base_idx]
								+ cpts[4*row + column + 2 + patch_base_idx];
	}

	barrier();

	// calculate the upper and lower slefe for each row
	float u = float(column)/3.0f;
	seg_upper = (1-u) * cpts[row*4 + patch_base_idx] + u*cpts[row*4 + 3 + patch_base_idx];
	seg_lower = seg_upper;

	for(int j = 0; j < 2; j++) // for each base
	{

		add_up_basis(seg_lower, seg_upper, D2b[row*4 + j + patch_base_idx], 
					slefe_lower_3_3[column + 4*j], slefe_upper_3_3[column + 4*j]);
	}
	
	upper[local_thread_id + patch_base_idx] = seg_upper;
	lower[local_thread_id + patch_base_idx] = seg_lower;

	barrier();


	// calculate upper slefe of upper
	// calc 2nd derivative on u direction on v direction

	if(row < 2)
	{
		D2b[column + row*4 + patch_base_idx] = upper[4*row + column + patch_base_idx] 
								- 2.0f * upper[4*(row + 1) + column + patch_base_idx]
								+ upper[4*(row + 2) + column + patch_base_idx];
	}
	barrier();

	float v = float(row)/3.0f;
	seg_upper = (1-v) * upper[column + patch_base_idx] + v * upper[row*4 + column + patch_base_idx];
	seg_lower = seg_upper;

	for(int j = 0; j < 2; j++) // for each base
	{

		add_up_basis(seg_lower, seg_upper, D2b[j*4 + column + patch_base_idx], 
					slefe_lower_3_3[column + 4*j], slefe_upper_3_3[column + 4*j]);
	}
	
	final_upper = seg_upper;
	barrier();

	// calculate lower slefe of lower
	// calc 2nd derivative on v direction

	if(row < 2)
	{
		D2b[column + row*4 + patch_base_idx] = lower[4*row + column + patch_base_idx] 
								- 2.0f * lower[4*(row + 1) + column + patch_base_idx]
								+ lower[4*(row + 2) + column + patch_base_idx];
	}
	barrier();

	v = float(row)/3.0f;
	seg_upper = (1-v) * lower[column + patch_base_idx] + v*lower[row*4 + column + patch_base_idx];
	seg_lower = seg_upper;

	for(int j = 0; j < 2; j++) // for each base
	{

		add_up_basis(seg_lower, seg_upper, D2b[j*4 + column + patch_base_idx], 
					slefe_lower_3_3[column + 4*j], slefe_upper_3_3[column + 4*j]);
	}
	
	final_lower = seg_lower;
	barrier();

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

			vec3 error_p = center_bb_max - center_bb_min;
			float center_point_max_error = max(error_p.x,max(error_p.y, error_p.z));
			atomicMax(local_tess_level[group_patch_id], int(10000*min(max(3*sqrt(2*center_point_max_error/pixel_size), 1.0f), 64.0f)));

		}
	}

	barrier();

	// calculate bounding box in screen space
	vec3 bb_min, bb_max;
	cal_screen_bb(final_lower, final_upper, bb_min, bb_max);
	

	cube_min[local_thread_id + patch_base_idx] = bb_min;
	cube_max[local_thread_id + patch_base_idx] = bb_max;

	barrier();

	{
		vec3 error_p = bb_max - bb_min;
		float point_max_error = max(error_p.x,max(error_p.y, error_p.z));
		atomicMax(local_tess_level[group_patch_id], int(10000*min(max(3*sqrt(2*point_max_error/pixel_size), 1.0f), 64.0f)));
		if (gl_LocalInvocationID.x==0) {
			patch_tess_levels[patch_id] = point_max_error;
	}


	if(local_thread_id == 0)
		patch_tess_levels[patch_id] = max(1.0f, local_tess_level[group_patch_id]/10000.0f);

	

	}

}

