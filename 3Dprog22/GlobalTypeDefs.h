#pragma once
#include "Include/glm/vec3.hpp"

#define MAX_CASCADES 4 //Shader will compile with the same value, no need to change in shaders as well.

#define SHADOW_GRID_SUB 1
#define MAX_SHADOW_GRIDS powf(4, SHADOW_GRID_SUB)

#define MAX_INSTANCES 450000

#define MAX_PARTICLES 30000

#define SSAO_KERNEL_SIZE 32
#define SSAO_NOISE_SIZE 4

#define DOF_UC_SIZE 16

#define GB_WEIGHTS_SIZE 5

#define PATH_DIRECTORY "../3Dprog22/"
#define PATH_ASSETS "../3Dprog22/Assets/"
#define PATH_MODELS PATH_ASSETS + "/Models/"

constexpr float SMI_LOD_RANGE[4] =
{
	50,
	100,
	350,
	3000
};

namespace FrustumCorners
{
	enum FrustumCorners
	{
		LeftBotNear,
		RightBotNear,
		RightTopNear,
		LeftTopNear,
		LeftBotFar,
		RightBotFar,
		RightTopFar,
		LeftTopFar,
		Size
	};
	using FrustumArray = std::array<glm::vec3, FrustumCorners::Size>;
}
