#include "ParticleSystem.h"
#include "World.h"
#include "RenderEngine.h"
#include "GlobalTypeDefs.h"
#include "Particle.h"
#include "Components.h"
#include "Components.h"
#include "debug.h"


void ParticleSystem::Init(World* world, entt::registry& registry)
{
	srand(time(nullptr));
	InitShader();
	InitShaderBindings();
	InitBuffer();	
	InitQuadMesh(world);
}

void ParticleSystem::InitShader()
{
	auto* renderEngine = RenderEngine::Get();

	if (!renderEngine->GetShader(ShaderManager::ShaderType::Particle, shader))
	{
		printf("Failed to get particle shader\n");
	}
}

void ParticleSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();
	re->BindShader(shader);

	pb.projection = re->GetUniformLocation("projection");
	pb.sprite = re->GetUniformLocation("sprite");
}

void ParticleSystem::InitBuffer()
{
	auto* renderEngine = RenderEngine::Get();
	auto* ssboManager = renderEngine->GetSSBOManager();

	renderEngine->BindShader(shader);
	ssbo = ssboManager->CreateSSBO("ParticleBuffer", MAX_PARTICLES * sizeof(Particle), 2);
	ssboManager->BindShaderToBlock(shader.shaderId, ssbo.id, 2, "particleData");
}

void ParticleSystem::InitQuadMesh(class World* world)
{
	auto* meshManager = world->GetMeshManager();
	quadMesh = meshManager->GetQuadMesh();
}

void ParticleSystem::Update(World* world, entt::registry& registry, float deltaTime)
{
	auto* re = RenderEngine::Get();
	auto* cam = world->GetRenderCamera();
	auto view = registry.view<ParticleEmitterComponent>();
	auto dataSize = sizeof(Particle);

	for (auto entity : view)
	{
		auto& emitter = view.get<ParticleEmitterComponent>(entity).emitter;
		auto& particles = emitter.particles;

		for (auto it = particles.begin(); it != particles.end();)
		{
			auto& particle = *it;

			if (particle.lifetime <= 0)
			{
				particles.erase(it);
				continue;
			}
			else { ++it; }

			UpdateParticle(particle, deltaTime);
		}
		SpawnDeadParticles(emitter);
	}
}

void ParticleSystem::UpdateParticle(Particle& particle, float deltatime)
{
	particle.lifetime -= deltatime;
	
	glm::vec3 gravityImpulse = gravity * deltatime;
	particle.vel += gravityImpulse;
	particle.pos += particle.vel * deltatime;
}

void ParticleSystem::SpawnDeadParticles(ParticleEmitter& emitter)
{
	int delta = emitter.numParticles - emitter.particles.size();

	for (size_t i = 0; i < delta; i++)
	{
		Particle particle;
		float randX = ((rand() % 100) - 50) / 10.0f;
		float randY = ((rand() % 100) - 50) / 10.0f;
		float randZ = ((rand() % 100) - 50) / 10.0f;
		float velocity = rand() % 10 - 5;
		glm::vec3 random = glm::vec3(randX, randY, randZ);
		float rColor = 0.5f + ((rand() % 100) / 100.0f);
		particle.pos = emitter.pos + random;
		particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
		particle.lifetime = rand() % 3;
		particle.vel = { 0.0f, 5.1f + velocity, 0.f};
		emitter.particles.emplace_back(particle);
	}
}

void ParticleSystem::Render(class World* world, entt::registry& registry, const ABuffer* aBuffer)
{
	auto* re = RenderEngine::Get();
	auto projection = world->GetRenderCamera()->GetProjectionMatrix();
	auto viewMat = world->GetRenderCamera()->GetViewMatrix();
	auto view = registry.view<TransformComponent, ParticleEmitterComponent>();
	
	re->BindShader(shader);
	re->DisableBlending();
	re->EnableDepthTest();
	re->SetDepthMask(GL_FALSE);

	if (aBuffer)
	{
		re->BindImageTexture(0, aBuffer->indexBuffer, 0, false, 0, GL_READ_WRITE, GL_R32UI);
		re->BindACBO(aBuffer->acbo, 0);
	}

	for (auto entity : view)
	{
		auto [trans, emitterComp] = view.get<TransformComponent, ParticleEmitterComponent>(entity);

		auto& emitter = emitterComp.emitter;
		auto& particles = emitter.particles;

		glm::mat4 model = trans.GetTransform();

		/*
		model[0][0] = viewMat[0][0];
		model[0][1] = viewMat[1][0];
		model[0][2] = viewMat[2][0];

		model[1][0] = viewMat[0][1];
		model[1][1] = viewMat[1][1];
		model[1][2] = viewMat[2][1];

		model[2][0] = viewMat[0][2];
		model[2][1] = viewMat[1][2];
		model[2][2] = viewMat[2][2];
		*/

		re->BindSSBO(ssbo);
		re->BindSSBOData(0, sizeof(Particle) * emitter.particles.size(), emitter.particles.data());

		re->SetMat4(pb.projection, projection * viewMat * model);

		re->SetTexture(emitter.particleTexture, GL_TEXTURE_2D, pb.sprite, 0);
		re->DrawElementsInstanced(quadMesh, GL_TRIANGLES, particles.size());
	}

	re->DisableBlending();
	re->SetDepthFunc(GL_TRUE);
}

