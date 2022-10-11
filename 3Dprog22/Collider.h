#pragma once

#include "Include/glm/glm.hpp"
#include "Include/entt/entt.hpp"
#include "BoundingVolumes.h"
#include "ConvexHull.h"
#include <functional>
namespace CollideableType
{
	enum Type
	{
		Invalid = -1,
		Sphere = 0,
		ConvexHull = 1,
		Capsule
	};
}

struct Collideable
{
	CollideableType::Type type{ CollideableType::Invalid }; //4byte
	std::uint32_t shapeIndex{ UINT_MAX }; //4 byte

	std::function<void(entt::entity)> onHit{nullptr};
	bool bIgnorePathfinding = false;

	bool IsValid() const;

	void SetExtents(const glm::vec3& extent);
	const glm::vec3& GetExtents() const;

	void SetPosition(const glm::vec3& pos);
	const glm::vec3& GetPosition() const;

	void SetRotation(const glm::quat& newRot);
	glm::quat GetRotation() const;

	void SetTransform(const glm::mat4& transform);

	void Expand(const glm::vec3* pts, const int num);
	void ExpandToFitAABB();

	glm::mat3 GetInertiaTensor() const;
	glm::vec3 GetCenterOfMass() const;

	BoundingBox GetLocalBounds();
	BoundingBox GetWorldBounds(const glm::vec3& pos);

	void SetConvexHull(ConvexHull& hull);

	glm::vec3 operator()(glm::vec3 dir) const;

	/*Expected convex mesh only*/
	void BuildMesh(const std::vector<glm::vec3>& pts, const std::vector<unsigned>& indices);
	/*Builds convex hull from points*/
	void BuildConvex(const glm::vec3* pts, const int num);
	/*Builds K-Dop from points*/
	void BuildKDOP(const glm::vec3* pts, const int num, KDOP kDop, const KDOP_AXIS axis);
};
