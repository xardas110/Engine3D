#include "AlphaPassSystem.h"
#include "RenderEngine.h"

AlphaPassSystem* gs_singleton;

AlphaPassSystem::AlphaPassSystem()
{
    gs_singleton = this;
}

AlphaPassSystem::~AlphaPassSystem()
{
    gs_singleton = nullptr;
}

AlphaPassSystem* AlphaPassSystem::Get()
{
    return gs_singleton;
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
