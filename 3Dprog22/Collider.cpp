#include "Collider.h"

#include "Include/glm/gtx/component_wise.hpp"
#include "Include/glm/glm.hpp"
#include "Include/glm/gtc/quaternion.hpp"
#include "PhysicsSystem.h"
#include <iostream>

bool Collideable::IsValid() const
{
	if (type == CollideableType::Invalid) return false;
	if (shapeIndex == UINT_MAX) return false;
	return true;
}

void Collideable::SetExtents(const glm::vec3& extent)
{
	auto* ps = PhysicsSystem::Get();

	if (type == CollideableType::Sphere)
	{
		float max = glm::compMax(extent);
		ps->GetNarrowPhaseData().mSpheres[shapeIndex].SetRadius(max);
	}
	else if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].SetScale(extent);
	}
	else if (type == CollideableType::Capsule)
	{
		float max = glm::max(extent.z, extent.x);
		ps->GetNarrowPhaseData().mCapsules[shapeIndex].SetRadius(max * 0.5f);
		ps->GetNarrowPhaseData().mCapsules[shapeIndex].SetA(glm::vec3(0.f, -extent.y, 0.f));
		ps->GetNarrowPhaseData().mCapsules[shapeIndex].SetB(glm::vec3(0.f, extent.y, 0.f));
	}
}

const glm::vec3& Collideable::GetExtents() const
{
	auto* ps = PhysicsSystem::Get();

	if (type == CollideableType::Sphere)
	{
		return glm::vec3(ps->GetNarrowPhaseData().mSpheres[shapeIndex].GetRadius());
	}
	else if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].GetScale();
	}
	else if (type == CollideableType::Capsule)
	{
		return glm::vec3(ps->GetNarrowPhaseData().mCapsules[shapeIndex].GetRadius());
	}
	return glm::vec3(0.f);
}

void Collideable::SetPosition(const glm::vec3& pos)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::Sphere)
	{
		return ps->GetNarrowPhaseData().mSpheres[shapeIndex].SetCenter(pos);
	}
	else if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].SetPosition(pos);
	}
	else if (type == CollideableType::Capsule)
	{
		ps->GetNarrowPhaseData().mCapsules[shapeIndex].SetCenter(pos);
	}
}

const glm::vec3& Collideable::GetPosition() const
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::Sphere)
	{
		return ps->GetNarrowPhaseData().mSpheres[shapeIndex].GetCenter();
	}
	else if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].GetPosition();
	}
	else if (type == CollideableType::Capsule)
	{
		return ps->GetNarrowPhaseData().mCapsules[shapeIndex].GetCenter();
	}
	return glm::vec3(0.f);
}

void Collideable::SetRotation(const glm::quat& newRot)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].SetRotation(newRot);
	}
	else if (type == CollideableType::Capsule)
	{
		ps->GetNarrowPhaseData().mCapsules[shapeIndex].SetRotation(newRot);
	}
}

glm::quat Collideable::GetRotation() const
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].GetRotation();
	}
	return glm::identity<glm::quat>();
}

void Collideable::SetTransform(const glm::mat4& transform)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		Transform trans; trans.SetTransform(transform);
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].SetTransform(trans);
	}
}

void Collideable::Expand(const glm::vec3* pts, const int num)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].geo.aabb.Expand(pts, num);
	}
}

void Collideable::ExpandToFitAABB()
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].ExpandToFitAABB();
	}
}

glm::mat3 Collideable::GetInertiaTensor() const
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::Sphere)
	{
		return ps->GetNarrowPhaseData().mSpheres[shapeIndex].GetInertiaTensor();
	}
	else if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].GetInertiaTensor();
	}
	else if (type == CollideableType::Capsule)
	{
		return ps->GetNarrowPhaseData().mCapsules[shapeIndex].GetInertiaTensor();
	}
	return glm::identity<glm::mat3>();
}

glm::vec3 Collideable::GetCenterOfMass() const
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::Sphere)
	{
		return ps->GetNarrowPhaseData().mSpheres[shapeIndex].GetCenterOfMass();
	}
	else if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].GetCenterOfMass();
	}
	else if (type == CollideableType::Capsule)
	{
		return ps->GetNarrowPhaseData().mCapsules[shapeIndex].GetCenterOfMass();
	}
	return glm::vec3(0.f);
}

BoundingBox Collideable::GetLocalBounds()
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::Sphere)
	{
		return ps->GetNarrowPhaseData().mSpheres[shapeIndex].GetLocalBounds();
	}
	else if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].GetLocalBounds();
	}
	else if (type == CollideableType::Capsule)
	{
		return ps->GetNarrowPhaseData().mCapsules[shapeIndex].GetLocalBounds();
	}
	std::cout << "Get local bounds for collider failed " << std::endl;
	return BoundingBox(glm::vec3(0.f), glm::vec3(0.f));
}

BoundingBox Collideable::GetWorldBounds(const glm::vec3& pos)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::Sphere)
	{
		return ps->GetNarrowPhaseData().mSpheres[shapeIndex].GetWorldBounds(pos);
	}
	else if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].GetWorldBounds(pos);
	}
	else if (type == CollideableType::Capsule)
	{
		return ps->GetNarrowPhaseData().mCapsules[shapeIndex].GetWorldBounds(pos);
	}
	std::cout << "Get world bounds for collider failed " << std::endl;
	return BoundingBox(glm::vec3(0.f), glm::vec3(0.f));
}

void Collideable::SetConvexHull(ConvexHull& hull)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex] = hull;
	}
}

glm::vec3 Collideable::operator()(glm::vec3 dir) const
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::Sphere)
	{
		return ps->GetNarrowPhaseData().mSpheres[shapeIndex](dir);
	}
	else if (type == CollideableType::ConvexHull)
	{
		return ps->GetNarrowPhaseData().mConvexHulls[shapeIndex](dir);
	}
	else if (type == CollideableType::Capsule)
	{
		return ps->GetNarrowPhaseData().mCapsules[shapeIndex](dir);
	}

	std::cout << "Collidable support function not found " << shapeIndex << std::endl;
	return glm::vec3(0.f);
}

void Collideable::BuildMesh(const std::vector<glm::vec3>& pts, const std::vector<unsigned>& indices)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].BuildMesh(pts, indices);
	}
}

void Collideable::BuildConvex(const glm::vec3* pts, const int num)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].BuildConvex(pts, num);
	}
}

void Collideable::BuildKDOP(const glm::vec3* pts, const int num, KDOP dop, const KDOP_AXIS axis)
{
	auto* ps = PhysicsSystem::Get();
	if (type == CollideableType::ConvexHull)
	{
		ps->GetNarrowPhaseData().mConvexHulls[shapeIndex].BuildKDOP(pts, num, dop, axis);
	}
}
