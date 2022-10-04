#include "AlphaPassSystem.h"
#include "RenderEngine.h"

AlphaPassSystem* singleton;

AlphaPassSystem::AlphaPassSystem()
{
	singleton = this;
}

AlphaPassSystem::~AlphaPassSystem()
{
	singleton = nullptr;
}

AlphaPassSystem* AlphaPassSystem::Get()
{
	return singleton;
}

void AlphaPassSystem::AddAlphaMesh(const AlphaPassInformation& alphaPassInfo)
{
	alphaQue.push(alphaPassInfo);
}

void AlphaPassSystem::AlphaPass(World* world, entt::registry& registry)
{
	auto* re = RenderEngine::Get();

	re->EnableBlending();
	re->EnableDepthTest();
	re->SetDepthMask(GL_FALSE);

	while (!alphaQue.empty())
	{
		auto& top = alphaQue.top();
		top.callBackFunc(world, registry, top.alphaMesh);
		alphaQue.pop();
	}
	re->DisableBlending();
	re->DisableDepthTest();
	re->SetDepthMask(GL_TRUE);
}
