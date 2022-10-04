#pragma once
#include "Mesh.h"
#include <functional>
#include "Include/entt/entt.hpp"

struct AlphaMesh
{
	entt::entity entity; /*Only used for skeletalmeshses, static meshes is not added this*/
	glm::mat4 parentTransform;
	Mesh mesh;
};

struct AlphaPassInformation
{
	float distToCamera2{ FLT_MAX };

	AlphaMesh alphaMesh;
	std::function<void(class World*, entt::registry&, const AlphaMesh&)> callBackFunc;

	bool operator > (const AlphaPassInformation& other) const
	{
		return this->distToCamera2 < other.distToCamera2;
	}
};

