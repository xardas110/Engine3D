#include "EPA.h"

#define arraysize(arr) \
	sizeof(arr) / sizeof(arr[0])

EPATriangleInfo EPASimplex::GetTriangleInfo(const GJKSupportPoint& a, const GJKSupportPoint& b, const GJKSupportPoint& c)
{
	EPATriangleInfo result;
	result.normal = normalize(cross(b.minkowski - a.minkowski, c.minkowski - a.minkowski));
	result.distanceToOrigin = dot(result.normal, a.minkowski);
	return result;
}

bool EPASimplex::IsTriangleActive(uint32 index)
{
	uint32 wordIndex = index / 32;
	uint32 bitIndex = index % 32;

	return (activeTrianglesMask[wordIndex] & (1 << bitIndex)) != 0;
}

void EPASimplex::SetTriangleActive(uint32 index)
{
	assert(!IsTriangleActive(index));

	uint32 wordIndex = index / 32;
	uint32 bitIndex = index % 32;

	activeTrianglesMask[wordIndex] |= (1 << bitIndex);
}

void EPASimplex::SetTriangleInactive(uint32 index)
{
	assert(IsTriangleActive(index));

	uint32 wordIndex = index / 32;
	uint32 bitIndex = index % 32;

	activeTrianglesMask[wordIndex] &= ~(1 << bitIndex);
}

uint32 EPASimplex::PushPoint(const GJKSupportPoint& a)
{
	if (numPoints >= arraysize(points))
	{
		return UINT32_MAX;
	}

	uint32 index = numPoints++;
	points[index] = a;
	return index;
}

uint32 EPASimplex::PushTriangle(uint32 a, uint32 b, uint32 c, uint32 edgeOppositeA, uint32 edgeOppositeB, uint32 edgeOppositeC, EPATriangleInfo info)
{
	if (numTriangles >= arraysize(triangles))
	{
		return UINT32_MAX;
	}

	uint32 index = numTriangles++;
	SetTriangleActive(index);

	EPIATri& tri = triangles[index];
	tri.a = a;
	tri.b = b;
	tri.c = c;
	tri.edgeOppositeA = edgeOppositeA;
	tri.edgeOppositeB = edgeOppositeB;
	tri.edgeOppositeC = edgeOppositeC;
	tri.normal = info.normal;
	tri.distanceToOrigin = info.distanceToOrigin;
		
	return index;
}

uint32 EPASimplex::PushEdge(uint32 a, uint32 b, uint32 triangleA, uint32 triangleB)
{
	if (numEdges >= arraysize(edges))
	{
		return UINT32_MAX;
	}

	uint32 index = numEdges++;
	EPAEdge& edge = edges[index];
	edge.a = a;
	edge.b = b;
	edge.triangleA = triangleA;
	edge.triangleB = triangleB;
	return index;
}

uint32 EPASimplex::FindTriangleClosestToOrigin()
{
	uint32 closest = -1;
	float minDistance = FLT_MAX;
	for (uint32 i = 0; i < numTriangles; ++i)
	{
		if (IsTriangleActive(i))
		{
			EPIATri& tri = triangles[i];
			if (tri.distanceToOrigin < minDistance)
			{
				minDistance = tri.distanceToOrigin;
				closest = i;
			}
		}
	}

	assert(closest != -1);

	return closest;
}

bool EPASimplex::AddNewPointAndUpdate(const GJKSupportPoint& newPoint)
{
	// This function removes all triangles which point towards the new point and replaces them with a triangle fan connecting the new point to the simplex.

	uint8 edgeReferences[arraysize(edges)] = { 0 };

	for (uint32 i = 0; i < numTriangles; ++i)
	{
		if (IsTriangleActive(i))
		{
			EPIATri& tri = triangles[i];
			float d = dot(tri.normal, newPoint.minkowski - points[tri.a].minkowski);
			if (d > 0.f)
			{
				// Remove triangle and mark edges.
#define REFERENCE_EDGE(e) assert(e < numEdges); ++edgeReferences[e];

				REFERENCE_EDGE(tri.edgeOppositeA);
				REFERENCE_EDGE(tri.edgeOppositeB);
				REFERENCE_EDGE(tri.edgeOppositeC);

				SetTriangleInactive(i);
#undef REFERENCE_EDGE
			}
		}
	}

	uint32 borderEdgeIndices[128];
	uint32 numBorderEdges = 0;

	for (uint32 i = 0; i < numEdges; ++i)
	{
		assert(edgeReferences[i] <= 2);

		if (edgeReferences[i] == 1)
		{
			if (numBorderEdges >= arraysize(borderEdgeIndices))
			{
				return false;
			}
			borderEdgeIndices[numBorderEdges++] = i;
		}
	}

	assert(numBorderEdges > 0);

	uint32 newEdgePerPoint[arraysize(points)];

	uint32 newPointIndex = PushPoint(newPoint);
	if (newPointIndex == UINT32_MAX)
	{
		return false;
	}

	uint32 triangleOffset = numTriangles;

	for (uint32 i = 0; i < numBorderEdges; ++i)
	{
		// Add a triangle for each border edge connecting it to the new point.

		uint32 edgeIndex = borderEdgeIndices[i];
		EPAEdge& edge = edges[edgeIndex];

		bool triAActive = IsTriangleActive(edge.triangleA);
		bool triBActive = IsTriangleActive(edge.triangleB);
		assert(triAActive != triBActive);

		uint32 pointToConnect = triBActive ? edge.a : edge.b;

		uint32 triangleIndex = numTriangles;

		// Push edge from border edge start to new point. The other edge will be added later, which is why we set its index to -1 here temporarily.
		uint32 newEdgeIndex = PushEdge(pointToConnect, newPointIndex, -1, numTriangles);
		if (newEdgeIndex == UINT32_MAX)
		{
			return false;
		}
		newEdgePerPoint[pointToConnect] = newEdgeIndex;


		uint32 bIndex = pointToConnect;
		uint32 cIndex = triBActive ? edge.b : edge.a;

		const GJKSupportPoint& b = points[bIndex];
		const GJKSupportPoint& c = points[cIndex];

		// Index of edge opposite B is again -1.
		uint32 triangleIndexTest = PushTriangle(newPointIndex, bIndex, cIndex, edgeIndex, -1, newEdgeIndex, GetTriangleInfo(newPoint, b, c));
		if (triangleIndexTest == UINT32_MAX)
		{
			return false;
		}
		assert(triangleIndex == triangleIndexTest);

		// Set edge's new neighbor triangle.
		uint32& edgeInactiveTriangle = triAActive ? edge.triangleB : edge.triangleA;
		edgeInactiveTriangle = triangleIndex;
	}

	// Fix up missing indices.
	for (uint32 i = 0; i < numBorderEdges; ++i)
	{
		uint32 edgeIndex = borderEdgeIndices[i];
		EPAEdge& edge = edges[edgeIndex];

		bool triangleANew = edge.triangleA >= triangleOffset;
		bool triangleBNew = edge.triangleB >= triangleOffset;
		assert(triangleANew != triangleBNew);

		uint32 pointToConnect = triangleBNew ? edge.a : edge.b; // Other way around than above. This is the point which was connected by another loop iteration above.

		uint32 otherEdgeIndex = newEdgePerPoint[pointToConnect];
		EPAEdge& otherEdge = edges[otherEdgeIndex];

		uint32 triangleIndex = i + triangleOffset;
		EPIATri& tri = triangles[triangleIndex];

		assert(tri.edgeOppositeB == UINT32_MAX);
		assert(otherEdge.triangleA == UINT32_MAX);

		tri.edgeOppositeB = otherEdgeIndex;
		otherEdge.triangleA = triangleIndex;
	}

	return true;
}

