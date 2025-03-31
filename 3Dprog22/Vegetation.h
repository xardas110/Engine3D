#pragma once

#include "StaticMesh.h"
#include "Material.h"
#include "Include/glm/glm.hpp"
#include "Transform.h"
#include <vector>

#define VEG_MAX_LODS 5
#define VEG_MAX_LOD_SHADOW 0

constexpr float VEG_LOD_RANGES[VEG_MAX_LODS] =
{
	500,
	1000,
	2000,
	4000,
	8000
};

struct Variant
{
	std::vector<glm::mat4> transforms;
	StaticMesh LOD[VEG_MAX_LODS];
	int size[VEG_MAX_LODS];
};

struct Vegetation
{
	MaterialInstanced atlas;
	MaterialInstanced billboard;
	std::vector<Variant> variants;
};

