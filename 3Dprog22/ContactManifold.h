#pragma once
#include "Include/glm/glm.hpp"
#include "Include/entt/entt.hpp"

struct ContactManifold
{
	glm::vec3 ptOnA; glm::vec3 ptLsOnA;
	glm::vec3 ptOnB; glm::vec3 ptLsOnB;

	glm::vec3 normal;
	float depth;

	entt::entity aId;
	entt::entity bId;
};

struct CollisionPair
{
	entt::entity a{ entt::null };
	entt::entity b{ entt::null };

	bool operator == (const CollisionPair& rhs) const
	{
		return (((a == rhs.a) && (b == rhs.b)) || ((a == rhs.b) && (b == rhs.a)));
	}
	bool operator != (const CollisionPair& rhs) const
	{
		return !(*this == rhs);
	}
};
