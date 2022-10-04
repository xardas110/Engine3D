#pragma once

#include <vector>
#include <array>
#include "Include/glm/glm.hpp"
#include "Transform.h"
#include <initializer_list>
#include "BoundingVolumes.h"

enum KDOP
{
	DOP8 = 4,
	DOP10 = 5,
	DOP18 = 9,
	DOP26 = 13
};

enum KDOP_AXIS
{
	X, Y, Z
};

struct ConvexHull
{
	BoundingHullGeometry geo;
	Transform transform;
	glm::mat3 inertiaTensor;
	glm::vec3 centerOfMass;

	void BuildMesh(const std::vector<glm::vec3>& pts, const std::vector<unsigned>& incides);
	void BuildConvex(const glm::vec3* pts, const int num);
	void BuildKDOP(const glm::vec3* pts, const int num, KDOP kDop, KDOP_AXIS axis);

	void SetPosition(const glm::vec3& newPos);
	const glm::vec3& GetPosition() const;

	void AddPosition(const glm::vec3& addPos);

	void SetScale(const glm::vec3& newScale);
	const glm::vec3& GetScale() const;

	void SetRotation(const glm::quat& newRotation);
	const glm::quat& GetRotation() const;

	void SetTransform(const Transform& newTransform);
	const Transform& GetTransform() const;

	void ExpandToFitAABB();

	/*Will do a copy*/
	glm::mat3 GetInertiaTensor() const;
	glm::vec3 GetCenterOfMass() const;

	void Clear();

	BoundingBox GetLocalBounds() const;
	BoundingBox GetWorldBounds(const glm::vec3& pos) const;

	//Support function
	glm::vec3 operator()(glm::vec3 dir) const;
};