#pragma once
#include "Include/glm/glm.hpp"
#include "Include/entt/entt.hpp"
#include "CollisionUtility.h"
#include "ConvexHull.h"
#include "ContactManifold.h"
#include "EPA.h"
#include "RigidBody.h"

struct NarrowPhase;

template<class T, class Y>
bool IntersectConvexPoly(const T& A, RigidBody& aBody, const Y& B, RigidBody& bBody, ContactManifold& outContact)
{
	GJKSimplex simplex;
	if (IntersectGJK(A, B, simplex))
	{
		if (IntersectEPA(simplex, A, B, outContact) == epa_status::epa_success)
		{ 
			outContact.ptLsOnA = aBody.WsToLs(outContact.ptOnA);
			outContact.ptLsOnB = bBody.WsToLs(outContact.ptOnB);
			return true;
		}
	}

	return false;
}

bool IntersectSphereTriangle(const BoundingSphere& s, const Triangle& t, ContactManifold& outContact);

namespace Collision
{
	bool Intersect(
		const Collideable& aCol, RigidBody& aBody,
		const Collideable& bCol, RigidBody& bBody,
		const NarrowPhase& data, ContactManifold& outContact);

	bool Intersect(
		const BoundingSphere& A, RigidBody& aBody,
		const BoundingSphere& B, RigidBody& bBody,
		ContactManifold& outContact);	
}
