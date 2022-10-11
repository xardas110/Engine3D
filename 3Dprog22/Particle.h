#pragma once
#include "Include/glm/glm.hpp"
#include "Texture.h"
#include <vector>
#include <functional>

struct Particle
{
	/*Localpos -> ws = pos + emitter.pos*/
	glm::vec3 pos;
	float lifetime;
	glm::vec3 vel;
	float scale;
	glm::vec4 color;
};

struct ParticleEmitter
{
	int numParticles{ 1000 };
	float lifeTime{ 20.f };
	Texture particleTexture;
	std::function<void(Particle&)> forceFunc{ nullptr };
	std::vector<Particle> particles;

	void UpdateParticle(Particle& particle, float deltatime)
	{
		particle.lifetime -= deltatime;
		glm::vec3 gravityImpulse = glm::vec3(0.f, -9.81f, 0.f) * deltatime;
		particle.vel += gravityImpulse;
		particle.pos += particle.vel * deltatime;
	}

	void SpawnDeadParticles()
	{
		int delta = numParticles - particles.size();

		if (!forceFunc) return;

		for (size_t i = 0; i < delta; i++)
		{
			Particle particle;
			forceFunc(particle);
			particles.emplace_back(particle);
		}
	}

	void Update(float deltatime)
	{
		if (!bActivated) return;

		if (lifeTime <= 0.f)
		{
			particles.clear();
			return;
		}
		lifeTime -= deltatime;

		for (auto it = particles.begin(); it != particles.end();)
		{
			auto& particle = *it;

			if (particle.lifetime <= 0)
			{
				particles.erase(it);
				continue;
			}
			else { ++it; }

			UpdateParticle(particle, deltatime);
		}
		SpawnDeadParticles();
	}

	void SetActivated(bool bActivated)
	{
		this->bActivated = bActivated;
	}
	bool GetBActiavated() const
	{
		return bActivated;
	}

private:
	bool bActivated{ false };
};

