#pragma once
#include "Include/glm/glm.hpp"
#include "Texture.h"
#include <vector>

struct Particle
{
	glm::vec3 pos;
	float lifetime;
	glm::vec3 vel;
	float scale;
	glm::vec4 color;
};

struct ParticleEmitter
{
	int numParticles{ 1000 };
	float lifeTime{ 2.f };
	glm::vec3 pos{ 0.f };
	Texture particleTexture;
	std::vector<Particle> particles;
};

