#pragma once
#include "BoundingVolumes.h"
#include "Include/glm/gtc/quaternion.hpp"
#include "Include/glm/gtx/norm.hpp"
#include "ContactManifold.h"
#include <iostream>

struct GJKSupportPoint
{
	glm::vec3 shapeAPoint;
	glm::vec3 shapeBPoint;
	glm::vec3 minkowski;

	GJKSupportPoint() {}
	
	GJKSupportPoint(const glm::vec3& a, const glm::vec3& b)
	{
		shapeAPoint = a;
		shapeBPoint = b;
		minkowski = a - b;
	}
};

struct GJKSimplex
{
	GJKSupportPoint a; // This is only used for the epa.
	GJKSupportPoint b, c, d;
	uint32 numPoints = 0;
};

template <typename shapeA_t, typename shapeB_t>
static GJKSupportPoint support(const shapeA_t& a, const shapeB_t& b, const glm::vec3& dir)
{
	glm::vec3 farthestInA = a(dir);
	glm::vec3 farthestInB = b(-dir);
	return GJKSupportPoint(farthestInA, farthestInB);
}

static inline glm::vec3 CrossABA(const glm::vec3& a, const glm::vec3& b)
{
	return glm::cross(glm::cross(a, b), a);
}

enum GJKStatus
{
	gjk_stop,
	gjk_dont_stop,
	gjk_unexpected_error, // Happens very very seldom. I don't know the reason yet, but instead of asserting we return this. I figure it's better than crashing.
};

template <typename shapeA_t, typename shapeB_t>
static bool IntersectGJK(const shapeA_t& shapeA, const shapeB_t& shapeB, GJKSimplex& outSimplex)
{
	// http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/

	GJKStatus updateGJKSimplex(GJKSimplex& s, const GJKSupportPoint& a, glm::vec3& dir);

	glm::vec3 dir(1.f, 0.1f, -0.2f); // Arbitrary.

	// First point.
	outSimplex.c = support(shapeA, shapeB, dir);
	if (glm::dot(outSimplex.c.minkowski, dir) < 0.f)
	{
		return false;
	}

	// Second point.
	dir = -outSimplex.c.minkowski;
	outSimplex.b = support(shapeA, shapeB, dir);
	if (glm::dot(outSimplex.b.minkowski, dir) < 0.f)
	{
		return false;
	}

	dir = CrossABA(outSimplex.c.minkowski - outSimplex.b.minkowski, -outSimplex.b.minkowski);
	outSimplex.numPoints = 2;

	int maxInterations = 40;

	int numIt = 0;
	while (true && numIt < maxInterations)
	{
		if (glm::length2(dir) < 0.0001f)
		{
			return false;
		}

		GJKSupportPoint a = support(shapeA, shapeB, dir);
		if (glm::dot(a.minkowski, dir) < 0.f)
		{
			return false;
		}

		GJKStatus success = updateGJKSimplex(outSimplex, a, dir);
		if (success == gjk_stop)
		{
			assert(outSimplex.numPoints == 3);

			outSimplex.a = a;
			outSimplex.numPoints = 4;
			break;
		}
		else if (success == gjk_unexpected_error)
		{
			return false; // Return 'no collision' in the case of an unexpected error.
		}
		numIt++;
	}

	return true;
}


