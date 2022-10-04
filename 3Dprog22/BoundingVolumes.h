#pragma once
#include "Transform.h"
#include <vector>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef wchar_t wchar;

struct IndexedTriangle32
{
	uint32 a, b, c;
};

struct IndexedLine32
{
	uint32 a, b;

	bool operator == (const IndexedLine32& other) const {
		return ((a == other.a && b == other.b) || (a == other.b && b == other.a));
	}
};

struct BoundingHullEdge
{
	uint16 from, to;
	uint16 faceA, faceB;
};

struct BoundingHullFace
{
	uint32 a, b, c;
	glm::vec3 normal;
};

struct BoundingBox
{
	glm::vec3 min{FLT_MAX};
	glm::vec3 max{-FLT_MAX};

	BoundingBox();
	BoundingBox(const glm::vec3& min, const glm::vec3& max);

	void Expand(glm::vec3 o);
	void Expand(const glm::vec3* pts, const int num);

	float WidthX() const { return max.x - min.x; }
	float WidthY() const { return max.y - min.y; }
	float WidthZ() const { return max.z - min.z; }

	void Pad(glm::vec3 p);
	glm::vec3 GetCenter() const;
	glm::vec3 GetRadius() const;
	bool Intersect (glm::vec3 p) const;
	bool Intersect(const BoundingBox& other) const;

	BoundingBox TransformToAABB(glm::quat rotation, glm::vec3 translation) const;

	static BoundingBox Clear();
	static BoundingBox FromMinMax(glm::vec3 minCorner, glm::vec3 maxCorner);
	static BoundingBox FromCenterRadius(glm::vec3 center, glm::vec3 radius);

	float Volume()
	{
		glm::vec3 diameter = max - min;
		return diameter.x * diameter.y * diameter.z;
	}
};

struct BoundingSphere
{
private:
	glm::vec3 c;
	float r;
public:
	void SetCenter(const glm::vec3& newCenter);
	const glm::vec3& GetCenter() const;

	void SetRadius(float newRadius);
	float GetRadius() const;

	glm::vec3 operator()(glm::vec3 dir) const;

	glm::mat3 GetInertiaTensor() const;
	glm::vec3 GetCenterOfMass() const;

	BoundingBox GetLocalBounds() const;
	BoundingBox GetWorldBounds(const glm::vec3& pos) const;
};

struct BoundingTetrahedron
{
	glm::vec3 pts[6];

	BoundingTetrahedron(glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& d, glm::vec3& e, glm::vec3& f)
	{
		pts[0] = a; pts[1] = b; pts[2] = c; pts[3] = d; pts[4] = e; pts[5] = f;
	}

	glm::vec3 operator()(glm::vec3 dir) const
	{
		glm::vec3 result;
		float maxDist = -FLT_MAX;

		for (const glm::vec3& v : pts)
		{
			float d = glm::dot(dir, v);
			if (d > maxDist)
			{
				maxDist = d;
				result = v;
			}
		}

		return result;
	};
};

struct BoundingHullGeometry
{
	std::vector<glm::vec3> vertices;
	std::vector<BoundingHullEdge> edges;
	std::vector<BoundingHullFace> faces;

	BoundingBox aabb;

	static BoundingHullGeometry BuildMesh(const glm::vec3* vertices, uint32 numVertices, const IndexedTriangle32* triangles, uint32 numTriangles);

	static BoundingHullGeometry BuildConvexHull(const glm::vec3* vertices, uint32 numVertices, const IndexedTriangle32* triangles, uint32 numTriangles);
};

struct Triangle
{
	glm::vec3 a, b, c;
};