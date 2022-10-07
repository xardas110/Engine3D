#pragma once
#include <vector>
#include <BoundingVolumes.h>
#include "ConvexHull.h"

struct CollisionHeightmapTriangles
{
	std::vector<IndexedTriangle32> faces;
	std::vector<Triangle> GetAsTriangles(std::vector<float>* heightmap, float scaleXZ, float scaleY, int width, bool bShow = false);
};

class CollisionHeightmap
{
	friend class PhysicsSystem;

	glm::vec3 scale{0.f};
	int width{ 0 }; int height{ 0 };
	std::vector<float>* heightMap = nullptr;

public:
	void SetHeightmap(int width, int height, const glm::vec3& scale, std::vector<float>* heightmap);
	bool Intersect(const BoundingBox& bb, CollisionHeightmapTriangles& cht, bool bShow = false) const;
};

