#include "patch.h"
#include <iostream>
#include <algorithm>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1440

// buffer constants
#define MAX_NUM_PATCH 65536 

#define WINDOW_TITLE_PREFIX "Sample"

using std::vector;

using std::max;
using std::min;


using std::cout;
using std::endl;

using glm::vec3;
using glm::vec4;

// slefe coefficient for 3_3

float slefe_lower_3_3[] = {
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

float slefe_upper_3_3[] = {
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
int tile_indices[9][4] = {
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

// indices of the bi-linear "tile" that each vertex belongs
int tile_indices2[16][4] = {
	{0,0,0,0},
	{0,1,1,1},
	{1,2,2,2},
	{2,2,2,2},
	{0,3,3,3},
	{0,1,3,4},
	{1,2,4,5},
	{2,5,5,5},
	{3,6,6,6},
	{3,4,6,7},
	{4,5,7,8},
	{7,8,8,8},
	{6,6,6,6},
	{6,7,7,7},
	{7,8,8,8},
	{8,8,8,8}
};



/*
univariet slefe caluclation for cubic bezier with 3 segments
*/
void uni_slefe_cubic(const float* coeff, int stride, float* upper, float* lower, int stride_out)
{

	int deg = 3;
	int seg = 3;

	int bas = 2;   // num of basis (also num of 2nd differences)
	int pts = 4;   // num of breaking points in slefe

	float D2b[2];  // 2nd difference array
	float left_end, right_end;

	int i, j;

	// compute the 2nd difference of the input function
	for (i = 0; i < bas; i++)
		D2b[i] = coeff[i * stride] - 2 * coeff[(i + 1) * stride] + coeff[(i + 2) * stride];

	// compute the function bounds according to P, M, and D2b

	left_end = coeff[0 * stride];   // save two end points
	right_end = coeff[deg * stride];

	// compute each break point
	for (i = 0; i <= seg; i++) {
		double u = (double)i / seg;
		int    loc = i * stride_out;  // location at the result array

		// 1. initialize to linear average of two end points
		upper[loc] = (1 - u) * left_end + u * right_end;
		lower[loc] = (1 - u) * left_end + u * right_end;

		// 2. add contribution from every coefficients
		for (j = 0; j < bas; j++) {
			if (D2b[j] > 0) {
				upper[loc] += slefe_lower_3_3[j * pts + i] * D2b[j];
				lower[loc] += slefe_upper_3_3[j * pts + i] * D2b[j];
			}
			else {
				upper[loc] += slefe_upper_3_3[j * pts + i] * D2b[j];
				lower[loc] += slefe_lower_3_3[j * pts + i] * D2b[j];
			}
		}
	}

}


vector<float> determine_patch_tess_levels(Vertex* vertices, int num_patches, int degu, int degv, const glm::mat4& MVP, float pixel_size)
{
	// calculate the optimal tessellation factor for cubic patches via SLEFE

	vector<float> tess_levels(num_patches);

	// not used here
	int ordu = degu + 1;
	int ordv = degv + 1;

	float upper[4 * 4 * 3], lower[4 * 4 * 3];

	for (int patch_idx = 0; patch_idx < num_patches; patch_idx++)
	{

		/* calculate the slefe on each coordinate */

		for (int i = 0; i < 3; i++) // for each coordinate
		{
			float upper_local[4 * 4], lower_local[4 * 4], temp[4 * 4 * 3]; // temp is for nothing

			// u direction
			for (int j = 0; j < 4; j++)
				uni_slefe_cubic(&vertices[patch_idx * 16 + j * 4].position[i], sizeof(Vertex) / sizeof(float), &upper_local[j * 4], &lower_local[j * 4], 1);


			// v direction
			for (int j = 0; j < 4; j++) {
				uni_slefe_cubic(&upper_local[j], 4, &upper[j * 3 + i], &temp[0], 4 * 3);
				uni_slefe_cubic(&lower_local[j], 4, &temp[0], &lower[j * 3 + i], 4 * 3);
			}

		}


		/* calculate clipping space bounding box for the segment vertex */
		vec3 cube_min[16];
		vec3 cube_max[16];

		float patch_max_error = 0.0;

		for (int i = 0; i < 16; i++)
		{

			cube_min[i] = vec3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
			cube_max[i] = vec3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);

			// 8 vertice of the slefe box 
			for (unsigned int j = 0; j < 8; j++)
			{
				vec3 p;

				if (j & 0x01)
					p.x = upper[i * 3 + 0];
				else
					p.x = lower[i * 3 + 0];

				if (j & 0x02)
					p.y = upper[i * 3 + 1];
				else
					p.y = lower[i * 3 + 1];

				if (j & 0x04)
					p.z = upper[i * 3 + 2];
				else
					p.z = lower[i * 3 + 2];

				// transform the box vertex
				vec4 tp = MVP * vec4(p, 1.0);
				vec3 ntp = (tp / tp.w);

				// update the clipping space bounding box
				cube_min[i] = glm::min(cube_min[i], ntp);
				cube_max[i] = glm::max(cube_max[i], ntp);
			} // for each vertex of slefe box

			vec3 error_p = cube_max[i] - cube_min[i];
			float point_max_error = max(error_p.x, max(error_p.y, error_p.z));
			patch_max_error = max(point_max_error, patch_max_error);

		} // for each segment point

		/*
		* note that for bivariate the bound is bi-linear instead of linear
		* the width of the flat enclosure (triangle) of bi-linear bound
		* should also be taken into account. This could be done by adding
		* a segment vertex at each tile's center.
		*/


		for (int i = 0; i < 9; i++) // for each tile
		{
			float tile_upper[2 * 3], tile_lower[2 * 3];

			int i1 = tile_indices[i][0];
			int i2 = tile_indices[i][1];
			int i3 = tile_indices[i][2];
			int i4 = tile_indices[i][3];

			for (int j = 0; j < 3; j++) // for each coordinate, calculate width for both upper bonud and lower bound
			{
				float u1 = (upper[i1 * 3 + j] + upper[i4 * 3 + j]) / 2.0f;
				float u2 = (upper[i2 * 3 + j] + upper[i3 * 3 + j]) / 2.0f;

				float l1 = (lower[i1 * 3 + j] + lower[i4 * 3 + j]) / 2.0f;
				float l2 = (lower[i2 * 3 + j] + lower[i3 * 3 + j]) / 2.0f;

				/*
				* The triangluation will fall into range [min(l1,l2), max(u1,u2],
				* but this bound is too lose.
				* Depend on the triangulation, there are two smaller range that
				* could be used to bound the triangulation, but we don't know
				* the actual triangulation. So we need to try both of them,
				* for each coordinate. This end up with 2^3 = 8 slefe boxes.
				*/

				tile_upper[0 * 3 + j] = (u1 + u2) / 2.0f;
				tile_lower[0 * 3 + j] = std::min(l1, l2);

				tile_upper[1 * 3 + j] = std::max(u1, u2);
				tile_lower[1 * 3 + j] = (l1 + l2) / 2.0f;
			}

			/* get the 8 corner of the average center box and find the one
			 * that closest to the camera.
			 *
			 * This is determined purely by the MVP matrix.
			 */

			{
				int x_range = MVP[0][2] < 0 ? 1 : 0;
				int y_range = MVP[1][2] < 0 ? 1 : 0;
				int z_range = MVP[2][2] < 0 ? 1 : 0;


				/* construct the slefe box for the tile center vertex */
				vec3 tile_cube_min = vec3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
				vec3 tile_cube_max = vec3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);

				for (unsigned int j = 0; j < 8; j++)
				{
					vec3 p;

					if (j & 0x01)
						p.x = tile_upper[x_range * 3 + 0];
					else
						p.x = tile_lower[x_range * 3 + 0];

					if (j & 0x02)
						p.y = tile_upper[y_range * 3 + 1];
					else
						p.y = tile_lower[y_range * 3 + 1];

					if (j & 0x04)
						p.z = tile_upper[z_range * 3 + 2];
					else
						p.z = tile_lower[z_range * 3 + 2];

					// transform the box vertex
					vec4 tp = MVP * vec4(p, 1.0);
					vec3 ntp = (tp / tp.w);

					// update the clipping space bounding box
					tile_cube_min = glm::min(tile_cube_min, ntp);
					tile_cube_max = glm::max(tile_cube_max, ntp);
				} // for each vertex of slefe box

				vec3 error_p = tile_cube_max - tile_cube_min;
				float point_max_error = max(error_p.x, max(error_p.y, error_p.z));
				patch_max_error = max(point_max_error, patch_max_error);
			}
		}

		tess_levels[patch_idx] = std::min(std::max(float(3 * sqrt(2 * patch_max_error / pixel_size)), 1.0f), 64.0f);
	} // for each patch

	return tess_levels;
}
