#include "BoundingVolumes.h"
#include "GJK.h"
#include <iostream>
#include <unordered_map>

static bool isZero(float x) 
{
	return abs(x) < 1e-6f;
}

static void addBoundingHullEdge(uint32 a, uint32 b, uint32 face, std::unordered_map<uint32, BoundingHullEdge*>& edgeMap, std::vector<BoundingHullEdge>& edges, uint32& edgeIndex)
{
	uint32 from = glm::min(a, b);
	uint32 to = glm::max(a, b);
	uint32 edge = (from << 16) | to;

	auto it = edgeMap.find(edge);
	if (it == edgeMap.end())
	{
		auto& newEdge = edges[edgeIndex++];
		newEdge.from = from;
		newEdge.to = to;
		newEdge.faceA = face;
		edgeMap.insert({ edge, &newEdge });
	}
	else
	{
		assert(it->second->from == from);
		assert(it->second->to == to);
		assert(it->second->faceA != UINT16_MAX);
		assert(it->second->faceB == UINT16_MAX);
		it->second->faceB = face;
	}
}

template <typename triangle_t>
static BoundingHullGeometry hullFromMesh(const glm::vec3* vertices, uint32 numVertices, const triangle_t* triangles, uint32 numTriangles)
{
	std::cout << "Building hull from num verts: " << numVertices << std::endl;

	BoundingHullGeometry hull;

	uint32 numEdges = numVertices + numTriangles - 2; // Euler characteristic for convex polyhedra.

	hull.vertices.resize(numVertices);
	hull.faces.resize(numTriangles);
	hull.edges.resize(numEdges, { UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX });
	hull.aabb = BoundingBox::Clear();

	for (uint32 i = 0; i < numVertices; ++i)
	{
		hull.vertices[i] = vertices[i];
		hull.aabb.Expand(vertices[i]);
	}

	std::unordered_map<uint32, BoundingHullEdge*> edgeMap;

	uint32 edgeIndex = 0;

	for (uint32 i = 0; i < numTriangles; ++i)
	{
		triangle_t tri = triangles[i];

		uint32 a = tri.a;
		uint32 b = tri.b;
		uint32 c = tri.c;

		glm::vec3 va = vertices[a];
		glm::vec3 vb = vertices[b];
		glm::vec3 vc = vertices[c];
		glm::vec3 normal = cross(vb - va, vc - va);

		hull.faces[i].a = a;
		hull.faces[i].b = b;
		hull.faces[i].c = c;
		hull.faces[i].normal = normal;

		addBoundingHullEdge(a, b, i, edgeMap, hull.edges, edgeIndex);
		addBoundingHullEdge(b, c, i, edgeMap, hull.edges, edgeIndex);
		addBoundingHullEdge(a, c, i, edgeMap, hull.edges, edgeIndex);
	}

	//assert(edgeIndex == numEdges);

	std::cout << "Hull result finished with num verts: " << hull.vertices.size() << std::endl;

	return hull;
}

BoundingHullGeometry BoundingHullGeometry::BuildMesh(const glm::vec3* vertices, uint32 numVertices, const IndexedTriangle32* triangles, uint32 numTriangles)
{
	return hullFromMesh(vertices, numVertices, triangles, numTriangles);
}

BoundingHullGeometry BoundingHullGeometry::BuildConvexHull(const glm::vec3* vertices, uint32 numVertices, const IndexedTriangle32* triangles, uint32 numTriangles)
{
	return BoundingHullGeometry();
}

BoundingBox::BoundingBox()
{
}

BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max)
	:min(min), max(max) {}

void BoundingBox::Expand(glm::vec3 o)
{
	min.x = glm::min(min.x, o.x);
	min.y = glm::min(min.y, o.y);
	min.z = glm::min(min.z, o.z);
	max.x = glm::max(max.x, o.x);
	max.y = glm::max(max.y, o.y);
	max.z = glm::max(max.z, o.z);
}

void BoundingBox::Expand(const glm::vec3* pts, const int num)
{
	for (int i = 0; i < num; i++) {
		Expand(pts[i]);
	}
}

void BoundingBox::Pad(glm::vec3 p)
{
	min -= p;
	max += p;
}

glm::vec3 BoundingBox::GetCenter() const
{
	return (min + max) * 0.5f;
}

glm::vec3 BoundingBox::GetRadius() const
{
	return (max - min) * 0.5f;
}

bool BoundingBox::Intersect(glm::vec3 p) const
{
	return p.x >= min.x && p.x <= max.x
		&& p.y >= min.y && p.y <= max.y
		&& p.z >= min.z && p.z <= max.z;
}

bool BoundingBox::Intersect(const BoundingBox& b) const
{
	return (b.min.x <= max.x && b.max.x >= min.x) &&
		(b.min.y <= max.y && b.max.y >= min.y) &&
		(b.min.z <= max.z && b.max.z >= min.z);
}

BoundingBox BoundingBox::TransformToAABB(glm::quat rotation, glm::vec3 translation) const
{
	BoundingBox result = BoundingBox::Clear();
	result.Expand(rotation * min + translation);
	result.Expand(rotation * glm::vec3(max.x, min.y, min.z) + translation);
	result.Expand(rotation * glm::vec3(min.x, max.y, min.z) + translation);
	result.Expand(rotation * glm::vec3(max.x, max.y, min.z) + translation);
	result.Expand(rotation * glm::vec3(min.x, min.y, max.z) + translation);
	result.Expand(rotation * glm::vec3(max.x, min.y, max.z) + translation);
	result.Expand(rotation * glm::vec3(min.x, max.y, max.z) + translation);
	result.Expand(rotation * max + translation);
	return result;
}

BoundingBox BoundingBox::Clear()
{
	return BoundingBox{ glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX), glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX) };
}

BoundingBox BoundingBox::FromMinMax(glm::vec3 minCorner, glm::vec3 maxCorner)
{
	return BoundingBox{ minCorner, maxCorner };
}

BoundingBox BoundingBox::FromCenterRadius(glm::vec3 center, glm::vec3 radius)
{
	return BoundingBox{ center - radius, center + radius };
}

void BoundingSphere::SetCenter(const glm::vec3& newCenter)
{
	c = newCenter;
}

const glm::vec3& BoundingSphere::GetCenter() const
{
	return c;
}

void BoundingSphere::SetRadius(float newRadius)
{
	r = newRadius;
}

float BoundingSphere::GetRadius() const
{
	return r;
}

glm::vec3 BoundingSphere::operator()(glm::vec3 dir) const
{
	return r * glm::normalize(dir) + c;
}

glm::mat3 BoundingSphere::GetInertiaTensor() const
{
	glm::mat3 inertia(1.f);

	inertia[0][0] = 2.f * r * r / 5.f;
	inertia[1][1] = 2.f * r * r / 5.f;
	inertia[2][2] = 2.f * r * r / 5.f;

	return inertia;
}

glm::vec3 BoundingSphere::GetCenterOfMass() const
{
	return glm::vec3(0.f);
}

BoundingBox BoundingSphere::GetLocalBounds() const
{
	return BoundingBox(glm::vec3(c - r), glm::vec3(c + r));
}

BoundingBox BoundingSphere::GetWorldBounds(const glm::vec3& pos) const
{
	return BoundingBox(glm::vec3(pos + c - r), glm::vec3(pos + c + r));
}
