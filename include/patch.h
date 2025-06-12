#pragma once
#include "vertex.h"
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include <vector>

const float MAX_FLOAT = 10000.0f;

std::vector<float> determine_patch_tess_levels(Vertex* vertices, int num_patches, int degu, int degv, const glm::mat4& MVP, float pixel_size);


