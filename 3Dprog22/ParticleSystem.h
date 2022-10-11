#pragma once
#include "Include/entt/entt.hpp"
#include "SSBO.h"
#include "shader.h"
#include "Particle.h"
#include "Mesh.h"
#include "ABuffer.h"

struct ParticleBindings
{
	int sprite;
	int projection;
	int instanceOffset;
};
//source: http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/
/*Still needs fixing*/
class ParticleSystem
{
	friend class World;
	friend class DeferredRenderer;

	void Init(class World* world, entt::registry& registry);
	void InitShader();
	void InitShaderBindings();
	void InitBuffer();
	void InitQuadMesh(class World* world);
	void Update(class World* world, entt::registry& registry, float deltaTime);
	void Render(class World* world, entt::registry& registry, const ABuffer* aBuffer);
	
private:
	glm::vec3 gravity = glm::vec3(0.f, -9.81f, 0.f);
	SSBO ssbo;
	Shader shader;
	ParticleBindings pb;
	Mesh quadMesh;
};

