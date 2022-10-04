#include "ConvexHull.h"
#include <vector>
#include "Include/glm/gtx/norm.hpp"
#include "RenderDebugger.h"
#include "Debug.h"

constexpr float PI = 3.1459f;

void ConvexHull::BuildMesh(const std::vector<glm::vec3>& pts, const std::vector<unsigned>& incides)
{
	std::vector<IndexedTriangle32> faces;

	for (size_t i = 0; i < incides.size(); i+=3 )
	{
		IndexedTriangle32 temp;
		temp.a = incides[i];
		temp.b = incides[i + 1];
		temp.c = incides[i + 2];
		faces.emplace_back(temp);
	}

	geo = BoundingHullGeometry::BuildMesh(pts.data(), pts.size(), faces.data(), faces.size());
}

int FindPointFurthestInDir(const glm::vec3* pts, const int num, const glm::vec3& dir) {
	int maxIdx = 0;
	float maxDist = glm::dot(dir, pts[0]);
	for (int i = 1; i < num; i++) {
		float dist = glm::dot(dir, pts[i]);
		if (dist > maxDist) {
			maxDist = dist;
			maxIdx = i;
		}
	}
	return maxIdx;
}

float DistanceFromLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& pt) {
	glm::vec3 ab = glm::normalize(b - a);
	glm::vec3 ray = pt - a;
	glm::vec3 projection = ab * glm::dot(ray, ab);	// project the ray onto ab
	glm::vec3 perpindicular = ray - projection;
	return glm::length(perpindicular);
}

glm::vec3 FindPointFurthestFromLine(const glm::vec3* pts, const int num, const glm::vec3& ptA, const glm::vec3& ptB) {
	int maxIdx = 0;
	float maxDist = DistanceFromLine(ptA, ptB, pts[0]);
	for (int i = 1; i < num; i++) {
		float dist = DistanceFromLine(ptA, ptB, pts[i]);
		if (dist > maxDist) {
			maxDist = dist;
			maxIdx = i;
		}
	}
	return pts[maxIdx];
}

float DistanceFromTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& pt) {
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 normal = glm::normalize(glm::cross(ab, ac));

	glm::vec3 ray = pt - a;
	float dist = glm::dot(ray, normal);
	return dist;
}

glm::vec3 FindPointFurthestFromTriangle(const glm::vec3* pts, const int num, const glm::vec3& ptA, const glm::vec3& ptB, const glm::vec3& ptC) {
	int maxIdx = 0;
	float maxDist = DistanceFromTriangle(ptA, ptB, ptC, pts[0]);
	for (int i = 1; i < num; i++) {
		float dist = DistanceFromTriangle(ptA, ptB, ptC, pts[i]);
		if (dist * dist > maxDist * maxDist) {
			maxDist = dist;
			maxIdx = i;
		}
	}
	return pts[maxIdx];
}


void BuildTetrahedron(const glm::vec3* verts, const int num, std::vector< glm::vec3 >& hullPts, std::vector< IndexedTriangle32 >& hullTris)
{
	hullPts.clear();
	hullTris.clear();

	glm::vec3 points[4];

	int idx = FindPointFurthestInDir(verts, num, glm::vec3(1, 0, 0));
	points[0] = verts[idx];
	idx = FindPointFurthestInDir(verts, num, points[0] * -1.0f);
	points[1] = verts[idx];
	points[2] = FindPointFurthestFromLine(verts, num, points[0], points[1]);
	points[3] = FindPointFurthestFromTriangle(verts, num, points[0], points[1], points[2]);

	// This is important for making sure the ordering is CCW for all faces.
	float dist = DistanceFromTriangle(points[0], points[1], points[2], points[3]);
	if (dist > 0.0f) {
		std::swap(points[0], points[1]);
	}

	// Build the tetrahedron
	hullPts.push_back(points[0]);
	hullPts.push_back(points[1]);
	hullPts.push_back(points[2]);
	hullPts.push_back(points[3]);

	IndexedTriangle32 tri;
	tri.a = 0;
	tri.b = 1;
	tri.c = 2;
	hullTris.push_back(tri);

	tri.a = 0;
	tri.b = 2;
	tri.c = 3;
	hullTris.push_back(tri);

	tri.a = 2;
	tri.b = 1;
	tri.c = 3;
	hullTris.push_back(tri);

	tri.a = 1;
	tri.b = 0;
	tri.c = 3;
	hullTris.push_back(tri);
}

void RemoveInternalPoints(const std::vector< glm::vec3 >& hullPoints, const std::vector< IndexedTriangle32 >& hullTris, std::vector< glm::vec3 >& checkPts) {
	for (int i = 0; i < checkPts.size(); i++) {
		const glm::vec3& pt = checkPts[i];

		bool isExternal = false;
		for (int t = 0; t < hullTris.size(); t++) {
			const IndexedTriangle32& tri = hullTris[t];
			const glm::vec3& a = hullPoints[tri.a];
			const glm::vec3& b = hullPoints[tri.b];
			const glm::vec3& c = hullPoints[tri.c];

			// If the point is in front of any triangle then it's external
			float dist = DistanceFromTriangle(a, b, c, pt);
			if (dist > 0.0f) {
				isExternal = true;
				break;
			}
		}

		// if it's not external, then it's inside the polyhedron and should be removed
		if (!isExternal) {
			checkPts.erase(checkPts.begin() + i);
			i--;
		}
	}

	// Also remove any points that are just a little too close to the hull points
	for (int i = 0; i < checkPts.size(); i++) {
		const glm::vec3& pt = checkPts[i];

		bool isTooClose = false;
		for (int j = 0; j < hullPoints.size(); j++) {
			glm::vec3 hullPt = hullPoints[j];
			glm::vec3 ray = hullPt - pt;
			if ( glm::length2(ray) < 0.01f * 0.01f) {	// 1cm is too close
				isTooClose = true;
				break;
			}
		}

		if (isTooClose) {
			checkPts.erase(checkPts.begin() + i);
			i--;
		}
	}
}

bool IsEdgeUnique(const std::vector< IndexedTriangle32 >& tris, const std::vector< int >& facingTris, const int ignoreTri, const IndexedLine32& edge) {
	for (int i = 0; i < facingTris.size(); i++) {
		const int triIdx = facingTris[i];
		if (ignoreTri == triIdx) {
			continue;
		}

		const IndexedTriangle32& tri = tris[triIdx];

		IndexedLine32 edges[3];
		edges[0].a = tri.a;
		edges[0].b = tri.b;

		edges[1].a = tri.b;
		edges[1].b = tri.c;

		edges[2].a = tri.c;
		edges[2].b = tri.a;

		for (int e = 0; e < 3; e++) {
			if (edge == edges[e]) {
				return false;
			}
		}
	}
	return true;
}


void AddPoint(std::vector< glm::vec3 >& hullPoints, std::vector< IndexedTriangle32 >& hullTris, const glm::vec3& pt) {
	// This point is outside
	// Now we need to remove old triangles and build new ones

	// Find all the triangles that face this point
	std::vector< int > facingTris;
	for (int i = (int)hullTris.size() - 1; i >= 0; i--) {
		const IndexedTriangle32& tri = hullTris[i];

		const glm::vec3& a = hullPoints[tri.a];
		const glm::vec3& b = hullPoints[tri.b];
		const glm::vec3& c = hullPoints[tri.c];

		const float dist = DistanceFromTriangle(a, b, c, pt);
		if (dist > 0.0f) {
			facingTris.push_back(i);
		}
	}

	// Now find all edges that are unique to the tris, these will be the edges that form the new triangles
	std::vector< IndexedLine32 > uniqueEdges;
	for (int i = 0; i < facingTris.size(); i++) {
		const int triIdx = facingTris[i];
		const IndexedTriangle32& tri = hullTris[triIdx];

		IndexedLine32 edges[3];
		edges[0].a = tri.a;
		edges[0].b = tri.b;

		edges[1].a = tri.b;
		edges[1].b = tri.c;

		edges[2].a = tri.c;
		edges[2].b = tri.a;

		for (int e = 0; e < 3; e++) {
			if (IsEdgeUnique(hullTris, facingTris, triIdx, edges[e])) {
				uniqueEdges.push_back(edges[e]);
			}
		}
	}

	// now remove the old facing tris
	for (int i = 0; i < facingTris.size(); i++) {
		hullTris.erase(hullTris.begin() + facingTris[i]);
	}

	// Now add the new point
	hullPoints.push_back(pt);
	const int newPtIdx = (int)hullPoints.size() - 1;

	// Now add triangles for each unique edge
	for (int i = 0; i < uniqueEdges.size(); i++) {
		const IndexedLine32& edge = uniqueEdges[i];

		IndexedTriangle32 tri;
		tri.a = edge.a;
		tri.b = edge.b;
		tri.c = newPtIdx;
		hullTris.push_back(tri);
	}
}

void RemoveUnreferencedVerts(std::vector< glm::vec3 >& hullPoints, std::vector< IndexedTriangle32 >& hullTris) {
	for (int i = 0; i < hullPoints.size(); i++) {

		bool isUsed = false;
		for (int j = 0; j < hullTris.size(); j++) {
			const IndexedTriangle32& tri = hullTris[j];

			if (tri.a == i || tri.b == i || tri.c == i) {
				isUsed = true;
				break;
			}
		}

		if (isUsed) {
			continue;
		}

		for (int j = 0; j < hullTris.size(); j++) {
			IndexedTriangle32& tri = hullTris[j];
			if (tri.a > i) {
				tri.a--;
			}
			if (tri.b > i) {
				tri.b--;
			}
			if (tri.c > i) {
				tri.c--;
			}
		}

		hullPoints.erase(hullPoints.begin() + i);
		i--;
	}
}

void ExpandConvexHull(std::vector< glm::vec3 >& hullPoints, std::vector< IndexedTriangle32 >& hullTris, const std::vector< glm::vec3 >& verts) {
	std::vector< glm::vec3 > externalVerts = verts;
	RemoveInternalPoints(hullPoints, hullTris, externalVerts);

	while (externalVerts.size() > 0) {
		int ptIdx = FindPointFurthestInDir(externalVerts.data(), (int)externalVerts.size(), externalVerts[0]);

		glm::vec3 pt = externalVerts[ptIdx];

		// remove this element
		externalVerts.erase(externalVerts.begin() + ptIdx);

		AddPoint(hullPoints, hullTris, pt);

		RemoveInternalPoints(hullPoints, hullTris, externalVerts);
	}

	RemoveUnreferencedVerts(hullPoints, hullTris);
}

bool IsExternal(const std::vector< glm::vec3 >& pts, const std::vector< IndexedTriangle32 >& tris, const glm::vec3& pt) {
	bool isExternal = false;
	for (int t = 0; t < tris.size(); t++) {
		const IndexedTriangle32& tri = tris[t];
		const glm::vec3& a = pts[tri.a];
		const glm::vec3& b = pts[tri.b];
		const glm::vec3& c = pts[tri.c];

		// If the point is in front of any triangle then it's external
		float dist = DistanceFromTriangle(a, b, c, pt);
		if (dist > 0.0f) {
			isExternal = true;
			break;
		}
	}

	return isExternal;
}

glm::vec3 CalculateCenterOfMass(const std::vector< glm::vec3 >& pts, const std::vector< IndexedTriangle32 >& tris) {
	const int numSamples = 100;

	BoundingBox bounds;
	bounds.Expand(pts.data(), pts.size());

	glm::vec3 cm(0.0f);
	const float dx = bounds.WidthX() / (float)numSamples;
	const float dy = bounds.WidthY() / (float)numSamples;
	const float dz = bounds.WidthZ() / (float)numSamples;

	int sampleCount = 0;
	for (float x = bounds.min.x; x < bounds.max.x; x += dx) {
		for (float y = bounds.min.y; y < bounds.max.y; y += dy) {
			for (float z = bounds.min.z; z < bounds.max.z; z += dz) {
				glm::vec3 pt(x, y, z);

				if (IsExternal(pts, tris, pt)) {
					continue;
				}

				cm += pt;
				sampleCount++;
			}
		}
	}

	cm /= (float)sampleCount;
	return cm;
}

glm::mat3 CalculateInertiaTensor(const std::vector< glm::vec3 >& pts, const std::vector< IndexedTriangle32 >& tris, const glm::vec3& cm) {
	const int numSamples = 100;

	BoundingBox bounds;
	bounds.Expand(pts.data(), (int)pts.size());

	glm::mat3 tensor(0.f);

	const float dx = bounds.WidthX() / (float)numSamples;
	const float dy = bounds.WidthY() / (float)numSamples;
	const float dz = bounds.WidthZ() / (float)numSamples;

	int sampleCount = 0;
	for (float x = bounds.min.x; x < bounds.max.x; x += dx) {
		for (float y = bounds.min.y; y < bounds.max.y; y += dy) {
			for (float z = bounds.min.z; z < bounds.max.z; z += dz) {
				glm::vec3 pt(x, y, z);

				if (IsExternal(pts, tris, pt)) {
					continue;
				}

				// Get the point relative to the center of mass
				pt -= cm;

				tensor[0][0] += pt.y * pt.y + pt.z * pt.z;
				tensor[1][1] += pt.z * pt.z + pt.x * pt.x;
				tensor[2][2] += pt.x * pt.x + pt.y * pt.y;

				tensor[0][1] += -1.0f * pt.x * pt.y;
				tensor[0][2] += -1.0f * pt.x * pt.z;
				tensor[1][2] += -1.0f * pt.y * pt.z;

				tensor[1][0] += -1.0f * pt.x * pt.y;
				tensor[2][0] += -1.0f * pt.x * pt.z;
				tensor[2][1] += -1.0f * pt.y * pt.z;

				sampleCount++;
			}
		}
	}

	tensor *= 1.0f / (float)sampleCount;
	return tensor;
}

void ConvexHull::BuildConvex(const glm::vec3* pts, const int num)
{
//	printf("building convex");
	Clear(); geo.vertices.reserve(num);
	
	for (size_t i = 0; i < num; i++)
	{
		geo.vertices.push_back(pts[i]);
		geo.aabb.Expand(pts[i]);
	}

	std::vector<glm::vec3> hullPts; std::vector<IndexedTriangle32> hullTris;
	BuildTetrahedron(geo.vertices.data(), geo.vertices.size(), hullPts, hullTris);
	ExpandConvexHull(hullPts, hullTris, geo.vertices);

	centerOfMass = CalculateCenterOfMass(hullPts, hullTris);
	//printf("Calculating inertia tensor\n");
	inertiaTensor = CalculateInertiaTensor(hullPts, hullTris, centerOfMass);

	geo.vertices = hullPts;

	for (size_t i = 0; i < hullTris.size(); i++)
	{
		BoundingHullFace face;
		auto& tri = hullTris[i];
		glm::vec3& a = hullPts[tri.a];
		glm::vec3& b = hullPts[tri.b];
		glm::vec3& c = hullPts[tri.c];
		glm::vec3 normal = glm::cross(b-a, c-a);

		face.a = tri.a;
		face.b = tri.b;
		face.c = tri.c;
		face.normal = normal;

		geo.faces.emplace_back(face);
	}

	//printf("Convex hull finished with num verts: %i\n", (int)geo.vertices.size());
}

void ConvexHull::BuildKDOP(const glm::vec3* pts, const int num, KDOP kDop, const KDOP_AXIS axis)
{
	std::vector<glm::vec3> verts;
	
	glm::vec3* min = new glm::vec3[kDop]; glm::vec3 minA;
	glm::vec3* max = new glm::vec3[kDop]; glm::vec3 maxA;

	float deltaRad = 2.f * PI / (float)kDop;

	glm::vec3 startDir = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 searchAxis = glm::vec3(0.f, 0.f, 1.f);

	if (axis == Y)
	{
		startDir = glm::vec3(1.f, 0.f, 0.f);
		searchAxis = glm::vec3(0.f, 1.f, 0.f);
	}
	if (axis == X)
	{
		startDir = glm::ivec3(0.f, 0.f, 1.f);
		searchAxis = glm::vec3(1.f, 0.f, 0.f);
	}

	for (size_t i = 0; i < kDop; i++)
	{
		glm::mat4 rot(1.f); rot = glm::rotate(rot, deltaRad * i, glm::vec3(searchAxis));
		glm::vec3 nextDir = rot * glm::vec4(startDir, 1.f);

		float maxDist = -FLT_MAX; float minAxis = FLT_MAX;
		float minDist = FLT_MAX; float maxAxis = -FLT_MAX;

		for (size_t j = 0; j < num; j++)
		{
			auto& pt = pts[j];
			float dist2 = glm::dot(pt, nextDir);
			float dist2Axis = glm::dot(pt, glm::vec3(searchAxis));

			if (dist2Axis < minAxis)
			{
				minAxis = dist2Axis;
				minA = pt;
			}
			if (dist2Axis > maxAxis)
			{
				maxAxis = dist2Axis;
				maxA = pt;
			}
			if (dist2 < minDist)
			{
				minDist = dist2;
				min[i] = pt;
			}
			if (dist2 > maxDist)
			{
				maxDist = dist2;
				max[i] = pt;
			}
		}	
	}

	for (size_t i = 0; i < kDop; i++)
	{
		glm::vec3 v0; glm::vec3 v1;
		if (axis == Z)
		{
			v0 = glm::vec3(min[i].x, min[i].y, minA.z);
			v1 = glm::vec3(min[i].x, min[i].y, maxA.z);
		}
		if (axis == Y)
		{
			v0 = glm::vec3(min[i].x, minA.y, min[i].z);
			v1 = glm::vec3(min[i].x, maxA.y, min[i].z);
		}
		if (axis == X)
		{
			v0 = glm::vec3(minA.x, min[i].y, min[i].z);
			v1 = glm::vec3(maxA.x, min[i].y, min[i].z);
		}

		verts.emplace_back(v0);
		verts.emplace_back(v1);
	}
	for (size_t i = 0; i < kDop; i++)
	{
		glm::vec3 v0; glm::vec3 v1;
		if (axis == Z)
		{ 
			v0 = glm::vec3(max[i].x, max[i].y, minA.z);
			v1 = glm::vec3(max[i].x, max[i].y, maxA.z);
		}
		if (axis == Y)
		{
			v0 = glm::vec3(max[i].x, minA.y, max[i].z);
			v1 = glm::vec3(max[i].x, maxA.y, max[i].z);
		}
		if (axis == X)
		{
			v0 = glm::vec3(minA.x, max[i].y, max[i].z);
			v1 = glm::vec3(maxA.x, max[i].y, max[i].z);
		}

		verts.emplace_back(v0);
		verts.emplace_back(v1);
	}

	BuildConvex(verts.data(), (int)verts.size());

	delete[] min;
	delete[] max;
}

void ConvexHull::SetPosition(const glm::vec3& newPos)
{
	transform.SetPosition(newPos);
}

const glm::vec3& ConvexHull::GetPosition() const
{
	return transform.GetPosition();
}

void ConvexHull::AddPosition(const glm::vec3& addPos)
{
	transform.AddPosition(addPos);
}

void ConvexHull::SetScale(const glm::vec3& newScale)
{
	transform.SetScale(newScale);
}

const glm::vec3& ConvexHull::GetScale() const
{
	return transform.GetScale();
}

void ConvexHull::SetRotation(const glm::quat& newRotation)
{
	transform.SetRotation(newRotation);
	ExpandToFitAABB();
}

const glm::quat& ConvexHull::GetRotation() const
{
	return transform.GetRotation();
}

void ConvexHull::SetTransform(const Transform& newTransform)
{
	transform = newTransform;
}

const Transform& ConvexHull::GetTransform() const
{
	return transform;
}

void ConvexHull::ExpandToFitAABB()
{
	geo.aabb.Clear();
	for (size_t i = 0; i < geo.vertices.size(); i++)
	{
		geo.aabb.Expand(transform.GetRotation() * geo.vertices[i]);
	}
}

glm::mat3 ConvexHull::GetInertiaTensor() const
{
	glm::mat3 extent(1.f);
	extent[0][0] = GetScale().x;
	extent[1][1] = GetScale().y;
	extent[2][2] = GetScale().z;
	return inertiaTensor * extent;
}

glm::vec3 ConvexHull::GetCenterOfMass() const
{
	return centerOfMass;
}

void ConvexHull::Clear()
{
	geo.vertices.clear();
	geo.faces.clear();
	geo.edges.clear();
	geo.aabb.Clear();
}

BoundingBox ConvexHull::GetLocalBounds() const
{
	BoundingBox b;
	b.min = transform.GetScale() * geo.aabb.min + transform.GetPosition();
	b.max = transform.GetScale() * geo.aabb.max + transform.GetPosition();
	return b;
}

BoundingBox ConvexHull::GetWorldBounds(const glm::vec3& pos) const
{
	BoundingBox b;
	b.min = transform.GetScale() * geo.aabb.min + pos;
	b.max = transform.GetScale() * geo.aabb.max + pos;
	return b;
}

glm::vec3 ConvexHull::operator()(glm::vec3 dir) const
{
	glm::vec3 result;
	float maxDist = -FLT_MAX;

	for (const glm::vec3& v : geo.vertices)
	{
		glm::vec3 vert = transform.GetTransform() * glm::vec4(v, 1.f);

		float d = glm::dot(dir, vert);
		if (d > maxDist)
		{
			maxDist = d;
			result = vert;
		}
	}

	return result;
}
