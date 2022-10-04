#pragma once
#include "GJK.h"
#include "Include/glm/glm.hpp"
#include "CollisionUtility.h"
#include "ContactManifold.h"
#include <iostream>

struct EPIATri
{
	uint32 a;
	uint32 b;
	uint32 c;

	uint32 edgeOppositeA;
	uint32 edgeOppositeB;
	uint32 edgeOppositeC;

	glm::vec3 normal;
	float distanceToOrigin;
};

struct EPAEdge
{
	uint32 a;
	uint32 b;

	union
	{
		struct
		{
			uint32 triangleA;
			uint32 triangleB;
		};
		uint32 triangles[2];
	};
};

struct EPATriangleInfo
{
	glm::vec3 normal;
	float distanceToOrigin;
};

struct EPASimplex
{
	// TODO: Find better array sizes.
	GJKSupportPoint points[1024];
	EPIATri triangles[1024];
	EPAEdge edges[1024];

	uint32 activeTrianglesMask[1024 / 32];
	uint32 numTriangles;
	uint32 numPoints;
	uint32 numEdges;

	bool IsTriangleActive(uint32 index);
	void SetTriangleActive(uint32 index);
	void SetTriangleInactive(uint32 index);
	uint32 PushPoint(const GJKSupportPoint& a);
	uint32 PushTriangle(uint32 a, uint32 b, uint32 c, uint32 edgeOppositeA, uint32 edgeOppositeB, uint32 edgeOppositeC, EPATriangleInfo info);
	uint32 PushEdge(uint32 a, uint32 b, uint32 triangleA, uint32 triangleB);
	uint32 FindTriangleClosestToOrigin();
	bool AddNewPointAndUpdate(const GJKSupportPoint& newPoint);

	static EPATriangleInfo GetTriangleInfo(const GJKSupportPoint& a, const GJKSupportPoint& b, const GJKSupportPoint& c);
};

enum epa_status
{
	epa_none, // Not returned from EPA. Only for debug output (signals that function was not called).
	epa_success,
	epa_out_of_memory,
	epa_max_num_iterations_reached,
};

static const char* epaReturnNames[] =
{
	"None",
	"Success",
	"Out of memory",
	"Max num iterations reached",
};

// Regardless of the return type, this function always returns the best approximation of the collision info in outResult.
// The caller can decide whether to use the result or not.
template <typename shapeA_t, typename shapeB_t>
static epa_status IntersectEPA(const GJKSimplex& gjkSimplex, const shapeA_t& shapeA, const shapeB_t& shapeB, ContactManifold& outContact, uint32 maxNumIterations = 20)
{
	// http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
	// http://uu.diva-portal.org/smash/get/diva2:343820/FULLTEXT01 page 23+

	assert(gjkSimplex.numPoints == 4);

	EPASimplex epaSimplex;
	epaSimplex.numTriangles = 0;
	epaSimplex.numPoints = 0;
	epaSimplex.numEdges = 0;
	memset(epaSimplex.activeTrianglesMask, 0, sizeof(epaSimplex.activeTrianglesMask));

	epaSimplex.PushPoint(gjkSimplex.a);
	epaSimplex.PushPoint(gjkSimplex.b);
	epaSimplex.PushPoint(gjkSimplex.c);
	epaSimplex.PushPoint(gjkSimplex.d);

	epaSimplex.PushTriangle(0, 1, 3, 4, 3, 0, EPASimplex::GetTriangleInfo(gjkSimplex.a, gjkSimplex.b, gjkSimplex.d));
	epaSimplex.PushTriangle(1, 2, 3, 5, 4, 1, EPASimplex::GetTriangleInfo(gjkSimplex.b, gjkSimplex.c, gjkSimplex.d));
	epaSimplex.PushTriangle(2, 0, 3, 3, 5, 2, EPASimplex::GetTriangleInfo(gjkSimplex.c, gjkSimplex.a, gjkSimplex.d));
	epaSimplex.PushTriangle(0, 2, 1, 1, 0, 2, EPASimplex::GetTriangleInfo(gjkSimplex.a, gjkSimplex.c, gjkSimplex.b));

	epaSimplex.PushEdge(0, 1, 0, 3);
	epaSimplex.PushEdge(1, 2, 1, 3);
	epaSimplex.PushEdge(2, 0, 2, 3);
	epaSimplex.PushEdge(0, 3, 2, 0);
	epaSimplex.PushEdge(1, 3, 0, 1);
	epaSimplex.PushEdge(2, 3, 1, 2);

	uint32 closestIndex = 0;

	epa_status returnCode = epa_max_num_iterations_reached;

	for (uint32 iteration = 0; iteration < maxNumIterations; ++iteration)
	{
		closestIndex = epaSimplex.FindTriangleClosestToOrigin();
		EPIATri& tri = epaSimplex.triangles[closestIndex];

		GJKSupportPoint a = support(shapeA, shapeB, tri.normal);

		float d = dot(a.minkowski, tri.normal);
		if (d - tri.distanceToOrigin < 0.01f)
		{
			// Success.
			returnCode = epa_success;
			break;
		}

		if (!epaSimplex.AddNewPointAndUpdate(a))
		{
			//std::cout << "EPA out of memory.\n";
			returnCode = epa_out_of_memory;
			break;
		}
	}

	EPIATri& tri = epaSimplex.triangles[closestIndex];

	GJKSupportPoint& a = epaSimplex.points[tri.a];
	GJKSupportPoint& b = epaSimplex.points[tri.b];
	GJKSupportPoint& c = epaSimplex.points[tri.c];

	glm::vec3 barycentricCoords = ColUtil::Barycentric(a.minkowski, b.minkowski, c.minkowski, tri.normal * tri.distanceToOrigin);
	glm::vec3 pointA = barycentricCoords.x * a.shapeAPoint + barycentricCoords.y * b.shapeAPoint + barycentricCoords.z * c.shapeAPoint;
	glm::vec3 pointB = barycentricCoords.x * a.shapeBPoint + barycentricCoords.y * b.shapeBPoint + barycentricCoords.z * c.shapeBPoint;

	outContact.ptOnA = pointA;
	outContact.ptOnB = pointB;
	outContact.normal = glm::normalize(tri.normal);
	outContact.depth = tri.distanceToOrigin;

	return returnCode;
}


