#include "CollisionUtility.h"
#include "RenderDebugger.h"

glm::vec3 ColUtil::Barycentric(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p)
{
	glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = glm::dot(v0, v0);
	float d01 = glm::dot(v0, v1);
	float d11 = glm::dot(v1, v1);
	float d20 = glm::dot(v2, v0);
	float d21 = glm::dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;

	denom = (abs(denom) < FLT_EPSILON) ? 1.f : denom;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	return glm::vec3(u, v, w);
}

glm::vec3 ColUtil::ClosestPoint(const Triangle& t, const glm::vec3& p)
{
	/*Optimized test from Christer Ericson p. 139. I have my own version using GJK+EPA, using this for better performance*/

	glm::vec3 ab = t.b - t.a;
	glm::vec3  ac = t.c - t.a;
	glm::vec3  bc = t.c - t.b;
	// Compute parametric position s for projection P’ of P on AB,
	// P’ = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - t.a, ab), sdenom = glm::dot(p - t.b, t.a - t.b);
	// Compute parametric position t for projection P’ of P on AC,
	// P’ = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - t.a, ac), tdenom = glm::dot(p - t.c, t.a - t.c);
	if (snom <= 0.0f && tnom <= 0.0f) return t.a; // Vertex region early out
	// Compute parametric position u for projection P’ of P on BC,
	// P’ = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - t.b, bc), udenom = glm::dot(p - t.c, t.b - t.c);
	if (sdenom <= 0.0f && unom <= 0.0f) return t.b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return t.c; // Vertex region early out
	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(t.b - t.a, t.c - t.a);
	float vc = glm::dot(n, glm::cross(t.a - p, t.b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return t.a + snom / (snom + sdenom) * ab;
	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(t.b - p, t.c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return t.b + unom / (unom + udenom) * bc;
	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(t.c - p, t.a - p));
		// If P outside CA and within feature region of CA,
		// return projection of P onto CA
		if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
			return t.a + tnom / (tnom + tdenom) * ac;
	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * t.a + v * t.b + w * t.c;
}

glm::vec3 ColUtil::ClosestPtPointSegment(const glm::vec3& c, const glm::vec3& a, const glm::vec3& b)
{
	glm::vec3 ab = b - a;
	float t = glm::dot(c - a, ab) / glm::dot(ab, ab);

	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	return a + t * ab;
}

bool ColUtil::IntersectPtCapsule(const glm::vec3& pt, const BoundingCapsule& caps)
{
	glm::vec3 closestPt = ClosestPtPointSegment(pt, caps.GetCenter() + caps.GetA(), caps.GetCenter() + caps.GetB());

	glm::vec3 dir = pt - closestPt;
	float dist2 = glm::dot(dir, dir);
	
	if (dist2 > caps.GetRadius() * caps.GetRadius()) return false;

	return true;
}

glm::mat3 ColUtil::CalculateCapsInertia(const BoundingCapsule& caps, int numSamples)
{
	BoundingBox bounds;
	bounds.min = caps.GetA() - caps.GetRadius();
	bounds.max = caps.GetB() + caps.GetRadius();

	glm::mat3 tensor(0.f);

	const float dx = bounds.WidthX() / (float)numSamples;
	const float dy = bounds.WidthY() / (float)numSamples;
	const float dz = bounds.WidthZ() / (float)numSamples;

	int sampleCount = 0;
	for (float x = bounds.min.x; x < bounds.max.x; x += dx) {
		for (float y = bounds.min.y; y < bounds.max.y; y += dy) {
			for (float z = bounds.min.z; z < bounds.max.z; z += dz) {
				glm::vec3 pt(x, y, z);

				if (!IntersectPtCapsule(pt, caps)) {
					continue;
				}

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
