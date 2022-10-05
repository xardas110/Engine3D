#include "CollisionHeightmap.h"
#include "RenderDebugger.h"
#include "Debug.h"
#include "ConvexHull.h"
#include "Include/ImGui/imgui.h"

void CollisionHeightmap::SetHeightmap(int width, int height, const glm::vec3& scale, std::vector<float>* heightmap)
{
	this->heightMap = heightmap;
	this->width = width; this->height = height; 
	this->scale = scale;
}

glm::vec2 TransformToHMSpace(float x, float z, float scaleXZ, int width)
{
	glm::vec2 posMin = glm::vec2(x, z);
	posMin += scaleXZ * 0.5f;
	posMin /= scaleXZ;

	glm::vec2 f = glm::vec2((int)posMin.x, (int)posMin.y);

	posMin = posMin - f;
	posMin = posMin * (float)width;

	return posMin;
}

glm::vec2 TransformToWSSpace(int x, int z, float scaleXZ, int width)
{
	glm::vec2 pos;
	pos.x = (float)x / (float)width * scaleXZ - (scaleXZ * 0.5f);
	pos.y = (float)z / (float)width * scaleXZ - (scaleXZ * 0.5f);
	return pos;
}

bool CollisionHeightmap::Intersect(const BoundingBox& bb, CollisionHeightmapTriangles& cht, bool bShow) const
{
	float scaleXZ = scale.x;

	glm::ivec2 posMin = TransformToHMSpace(bb.min.x, bb.min.z, scale.x, width);
	glm::ivec2 posMax = TransformToHMSpace(bb.max.x, bb.max.z, scale.x, width);

	posMax += glm::ivec2(2);

	glm::ivec2 delta = posMax - posMin;

	auto* rd = RenderDebugger::Get();

	BoundingBox tempBox; cht.faces.reserve(50);
	for (int z = 0; z < delta.y; z++)
	{
		for (int x = 0; x < delta.x; x++)
		{
			glm::ivec2 currentPos = posMin + glm::ivec2(x, z);

			if (currentPos.y * width + currentPos.x >= heightMap->size()-1) continue;

			float h0 = heightMap->at(width * currentPos.y + currentPos.x) * scale.y;
			glm::vec2 v0 = TransformToWSSpace(currentPos.x, currentPos.y, scaleXZ, width);
			glm::vec3 p0(v0.x, h0, v0.y);

			tempBox.Expand(p0);

			int index = width * currentPos.y + currentPos.x;

			IndexedTriangle32 tri0, tri1;
			tri0.a = index;
			tri0.b = index + 1;
			tri0.c = index + width;

			tri1.a = index + 1;
			tri1.b = index + width + 1;
			tri1.c = index + width;

			cht.faces.emplace_back(tri0);
			cht.faces.emplace_back(tri1);
		}
	}

	if (bShow) rd->AddDebugBoundingBox(0.f, tempBox.min, tempBox.max);
	
	if (tempBox.Intersect(bb))
	{	
		return true;
	}

	return false;
}

std::vector<Triangle> CollisionHeightmapTriangles::GetAsTriangles(std::vector<float>* heightmap, float scaleXZ, float scaleY, int width, bool bShow)
{
	auto* rd = RenderDebugger::Get();
	std::vector<Triangle> result;

	for (auto& face : faces)
	{
		if (face.a >= heightmap->size() || face.b >= heightmap->size() || face.c >= heightmap->size()) continue;

		Triangle triangle;

		glm::ivec2 aWSXZ = TransformToWSSpace(face.a % width, face.a / width, scaleXZ, width);
		glm::ivec2 bWSXZ = TransformToWSSpace(face.b % width, face.b / width, scaleXZ, width);
		glm::ivec2 cWSXZ = TransformToWSSpace(face.c % width, face.c / width, scaleXZ, width);

		triangle.a = glm::vec3(aWSXZ.x, heightmap->at(face.a) * scaleY, aWSXZ.y);
		triangle.b = glm::vec3(bWSXZ.x, heightmap->at(face.b) * scaleY, bWSXZ.y);
		triangle.c = glm::vec3(cWSXZ.x, heightmap->at(face.c) * scaleY, cWSXZ.y);

		if (bShow)
		{
			rd->AddDebugSegment(0.f, triangle.a, triangle.b);
			rd->AddDebugSegment(0.f, triangle.b, triangle.c);
			rd->AddDebugSegment(0.f, triangle.c, triangle.a);
		}

		result.emplace_back(triangle);
	}
	return result;
}

std::vector<BoundingTetrahedron> CollisionHeightmapTriangles::GetAsTetrahedrons(std::vector<float>* heightmap, float scaleXZ, float scaleY, int width, bool bShow)
{
	auto* rd = RenderDebugger::Get();
	std::vector<BoundingTetrahedron> result;

	for (auto& face : faces)
	{
		if (face.a >= heightmap->size() || face.b >= heightmap->size() || face.c >= heightmap->size()) continue;

		glm::ivec2 aWSXZ = TransformToWSSpace(face.a % width, face.a / width, scaleXZ, width);
		glm::ivec2 bWSXZ = TransformToWSSpace(face.b % width, face.b / width, scaleXZ, width);
		glm::ivec2 cWSXZ = TransformToWSSpace(face.c % width, face.c / width, scaleXZ, width);

		glm::vec3 a = glm::vec3(aWSXZ.x, heightmap->at(face.a) * scaleY, aWSXZ.y);
		glm::vec3 b = glm::vec3(bWSXZ.x, heightmap->at(face.b) * scaleY, bWSXZ.y);
		glm::vec3 c = glm::vec3(cWSXZ.x, heightmap->at(face.c) * scaleY, cWSXZ.y);
	
		glm::vec3 d = a - glm::vec3(0.f, 5.f, 0.f);
		glm::vec3 e = b - glm::vec3(0.f, 5.f, 0.f);
		glm::vec3 f = c - glm::vec3(0.f, 5.f, 0.f);

		BoundingTetrahedron t(a,b,c,d,e, f);

		if (bShow)
		{
			rd->AddDebugSegment(0.f, a, b);
			rd->AddDebugSegment(0.f, b, c);
			rd->AddDebugSegment(0.f, c, a);

			rd->AddDebugSegment(0.f, d, e);
			rd->AddDebugSegment(0.f, e, f);
			rd->AddDebugSegment(0.f, f, d);
		}

		result.emplace_back(t);
	}

	return result;
}
