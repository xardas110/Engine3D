#pragma once

#include "Include/glm/glm.hpp"
#include "BoundingVolumes.h"

namespace ColUtil
{
	glm::vec3 Barycentric(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p);

	glm::vec3 ClosestPoint(const Triangle& t, const glm::vec3& pt);

	glm::vec3 ClosestPtPointSegment(const glm::vec3& c, const glm::vec3& a, const glm::vec3& b);

	bool IntersectPtCapsule(const glm::vec3& pt, const BoundingCapsule& caps);

	glm::mat3 CalculateCapsInertia(const BoundingCapsule& caps, int numSamples = 100);
}
