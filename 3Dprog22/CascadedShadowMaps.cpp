#include "CascadedShadowMaps.h"
#include "Debug.h"
#include "GlobalTypeDefs.h"
#include "World.h"
#include "Include/glm/gtx/norm.hpp"

void CascadedShadowMaps::CalculateCascades(const FrustumCorners::FrustumArray& frustumCorners, const glm::vec3& dirLight, float camNear, float camFar)
{
	//Now calculate a projection and offset for each cascade. For best quality.
	glm::vec3 e[4] =
	{
		frustumCorners[FrustumCorners::LeftBotFar] - frustumCorners[FrustumCorners::LeftBotNear],
		frustumCorners[FrustumCorners::RightBotFar] - frustumCorners[FrustumCorners::RightBotNear],
		frustumCorners[FrustumCorners::RightTopFar] - frustumCorners[FrustumCorners::RightTopNear],
		frustumCorners[FrustumCorners::LeftTopFar] - frustumCorners[FrustumCorners::LeftTopNear],	
	};
	
#ifdef DEBUG_CASCADE
	auto* renderSystem = World::Get()->renderSystem;
	glm::vec3 cascadeColors[4] =
	{
	{1.f, 1.f, 1.f},
	{0.f, 1.f, 1.f},
	{0.f, 0.f, 1.f},
	{0.5f, 0.5f, 0.5f}
	};
#endif

	auto* world = World::Get();
	auto* cam = world->GetRenderCamera();

	glm::vec3 offset = cam->GetDirection() * zOffset;

	constexpr float delta = 1.f / MAX_CASCADES;
	for (auto i = 0; i < MAX_CASCADES; i++)
	{
		FrustumCorners::FrustumArray cascadeCorners;
		for (int j = 0; j < 4; j++)
		{
			const glm::vec3 lc = (frustumCorners[j]- offset) + (e[j] * delta * (float)(i));
			const glm::vec3 cc = (frustumCorners[j]- offset) + (e[j] * delta * (float)(i+1));
			
#ifdef DEBUG_CASCADE
			renderSystem->AddDebugSegment(0.f, lc, cc, cascadeColors[i], 10.f);
#endif
			cascadeCorners[j]	=	lc;
			cascadeCorners[j+4] =	cc;
		}
		cascades[i].cascadeOffsetX = camFar * delta * float(i + 1);
		CalculateCascade(cascadeCorners, dirLight, i, camNear, camFar);
	}
}

void CascadedShadowMaps::CalculateCascade(const FrustumCorners::FrustumArray& corners, const glm::vec3& dirLight, int index, float camNear, float camFar)
{
	glm::vec3 center = { 0.f, 0.f, 0.f };
	for (const auto& corner : corners)
		center += corner;
	center /= corners.size();

	const auto worldUp = World::GetWorldUp();

	const glm::mat4 CSMView = glm::lookAt(center - dirLight, center, worldUp);

	glm::vec3 minBounds{ FLT_MAX };
	glm::vec3 maxBounds{ -FLT_MAX };
	
	for (const auto& corner : corners)
	{
		glm::vec3 cornerTSV = CSMView * glm::vec4(corner, 1.f);
		minBounds = glm::min(minBounds, cornerTSV);
		maxBounds = glm::max(maxBounds, cornerTSV);
	}

	// Will use box trace against the world later to determine minZ and maxZ, for now hardcoded, works very well.
	const glm::mat4 CSMProj = glm::ortho(minBounds.x, maxBounds.x, minBounds.y, maxBounds.y, minBounds.z * AMPLIFIER, maxBounds.z * AMPLIFIER);
	cascades[index].cascadeProjection = CSMProj * CSMView;
}

const std::array<Cascade, MAX_CASCADES>& CascadedShadowMaps::GetCascades(const FrustumCorners::FrustumArray& frustumCorners, const glm::vec3& dirLight, float camNear, float camFar)
{
	CalculateCascades(frustumCorners, dirLight, camNear, camFar);
	return cascades;
}
