#pragma once
#include "Texture.h"
#include "Mesh.h"
#include "Include/glm/glm.hpp"
#include <vector>

struct LensFlare
{
	Texture lensTexture;

	glm::vec2 pos{0.f,0.f};
	glm::vec2 scale{0.1f};	

	LensFlare(const Texture& texture, float scale)
		:lensTexture(texture), scale({ scale }) {};
};

struct LensFlares
{
	std::vector<LensFlare> flares;

	float spacing{ 0.4f };
	float brightness{ 1.f };

	bool bClipPass{ false };

	void AddFlare(const LensFlare& flare)
	{
		flares.emplace_back(flare);
	}
};