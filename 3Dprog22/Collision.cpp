#include "Collision.h"
#include "Include/glm/gtx/component_wise.hpp"
#include "Include/glm/glm.hpp"
#include "Include/glm/gtc/quaternion.hpp"
#include "PhysicsSystem.h"
#include "Entity.h"
#include "EPA.h"
#include "Collider.h"
#include "Debug.h"
#include "RenderDebugger.h"

bool Collision::Intersect(
	const BoundingSphere& A, RigidBody& aBody,
	const BoundingSphere& B, RigidBody& bBody,
	ContactManifold& outContact)
{
	const glm::vec3 aWs = aBody.GetPos();
	const glm::vec3 bWs = bBody.GetPos();
	
	glm::vec3 direction = bWs - aWs;

	float distance = glm::length(direction);
	float radiusSum = A.GetRadius() + B.GetRadius();

	if (radiusSum < distance) return false;

	outContact.depth = radiusSum - distance;
	outContact.normal = direction / distance;
	outContact.ptOnA = aWs + outContact.normal * A.GetRadius();
	outContact.ptOnB = bWs - outContact.normal * B.GetRadius();
	outContact.ptLsOnA = aBody.WsToLs(outContact.ptOnA);
	outContact.ptLsOnB = bBody.WsToLs(outContact.ptOnB);

	return true;
}


bool Collision::Intersect(
	const Collideable& aCol, RigidBody& aBody,
	const Collideable& bCol, RigidBody& bBody,
	const NarrowPhase& data, ContactManifold& outContact)
{
	outContact.aId = aBody.GetEntityId();
	outContact.bId = bBody.GetEntityId();

	if (aCol.type == CollideableType::Sphere && bCol.type == CollideableType::Sphere)
	{
		return Intersect(data.mSpheres[aCol.type], aBody, data.mSpheres[bCol.type], bBody, outContact);
	}

	return IntersectConvexPoly(aCol, aBody, bCol, bBody, outContact);
}

bool IntersectSphereTriangle(const BoundingSphere& s, const Triangle& t, ContactManifold& outContact)
{
	glm::vec3 cp = ColUtil::ClosestPoint(t, s.GetCenter());

	glm::vec3 dir = s.GetCenter() - cp;
	float dist2 = glm::dot(dir, dir);
	float radius2 = s.GetRadius() * s.GetRadius();

	if (dist2 > radius2)
	{
		return false;
	}

	float dist = glm::length(dir);

	outContact.normal = dir / dist;
	outContact.ptOnA = s.GetCenter() - outContact.normal * s.GetRadius();
	outContact.ptOnB = cp;
	
	return true;
}
