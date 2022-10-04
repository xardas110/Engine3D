#include "FogSystem.h"
#include "RenderEngine.h"
#include "World.h"

void FogSystem::Init(World* world)
{
	auto* re = RenderEngine::Get();

	re->GetShader(ShaderManager::Fog, fog.shader);
	re->BindShader(fog.shader);

	fog.bind.colorBuffer = re->GetUniformLocation("colorBuffer");
	fog.bind.posBuffer = re->GetUniformLocation("posBuffer");
	fog.bind.zFar = re->GetUniformLocation("zFar");
	fog.bind.fogColor = re->GetUniformLocation("fogColor");
	fog.bind.sightRange = re->GetUniformLocation("sightRange");
}

void FogSystem::Process(World* world, const Texture& posBuffer, const Texture& colorBuffer, const Mesh& quad, const DeferredRendererConfig& config)
{
	auto* re = RenderEngine::Get();
	auto* camera = world->GetRenderCamera();
	auto zFar = camera->GetFar();

	re->BindShader(fog.shader);
	re->SetTexture(posBuffer, GL_TEXTURE_2D, fog.bind.posBuffer, 0);
	re->SetTexture(colorBuffer, GL_TEXTURE_2D, fog.bind.colorBuffer, 1);
	re->SetFloat(fog.bind.zFar, zFar);
	re->SetFloat(fog.bind.sightRange, config.fog.sight);
	re->SetVec3(fog.bind.fogColor, fog.fogColor);
	re->DrawElements(quad, GL_TRIANGLES);
}

void FogSystem::Clean()
{
	//nothing to clean
}

void FogSystem::SetColor(const glm::vec3 newColor)
{
	fog.fogColor = newColor;
}

const glm::vec3& FogSystem::GetColor() const
{
	return fog.fogColor;
}
