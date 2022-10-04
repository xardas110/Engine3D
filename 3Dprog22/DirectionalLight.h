#pragma once
#include "Include/glm/vec3.hpp"
#include "GlobalTypeDefs.h"
#include "CascadedShadowMaps.h"

struct DirLight
{
	glm::vec3 lightDir = glm::normalize(glm::vec3(-0.58f, -0.58f, 0.58f));
	glm::vec3 ambient = glm::vec3(0.4f, 0.4f, 0.4f);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.f, 1.f);;
	glm::vec3 specular = glm::vec3(1.f, 1.f, 1.f);;
	float itensity{ 2.f };

	bool bCastShadow{ true };
	
	CascadedShadowMaps csm{};

	//should not know about the camera class, too lazy to refactor it out rn, might do it later if needed
	CascadedShadowMaps::Cascades GetCascades(FrustumCorners::FrustumArray& frustumCorners, float camNear, float camFar)
	{
		return csm.GetCascades(frustumCorners, lightDir, camNear, camFar);
	};
private:
	friend class SkySystem;
	UBO ubo;
};

struct DirLightBindings
{
	int lightDir;
	int ambient;
	int diffuse;
	int specular;
};