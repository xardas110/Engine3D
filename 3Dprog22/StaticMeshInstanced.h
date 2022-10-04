#pragma once
#include "StaticMesh.h"
#include "Material.h"
#include "Include/glm/glm.hpp"
#include "Transform.h"

#define SMI_MAX_LODS 4

struct StaticMeshInstanced
{
	std::vector<glm::mat4> transforms;
	StaticMesh LOD[SMI_MAX_LODS];
	int size[SMI_MAX_LODS];

	int GetNumLods() const
	{
		int result{ 0 };
		for (size_t i = 0; i < SMI_MAX_LODS; i++)
		{
			if (LOD[i].IsValid())
			{
				result++;
			}
		}
		return result;
	}
};

