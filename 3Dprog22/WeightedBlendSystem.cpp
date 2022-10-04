#include "WeightedBlendSystem.h"
#include "RenderEngine.h"

void WeightedBlendSystem::Init()
{
	InitShader();
	InitSSBO();
}

void WeightedBlendSystem::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::ShaderType::WBClearCompute, clearShader);
}

void WeightedBlendSystem::InitSSBO()
{
	auto* re = RenderEngine::Get();
	auto* ssbom = re->GetSSBOManager();

	ssbo = ssbom->CreateSSBO(
		"AlphaBuffer",
		sizeof(AlphaFragment) * WB_RES,
		5);

	re->BindSSBO(ssbo);
	ssbom->BindShaderToBlock(clearShader.shaderId, ssbo.id, 5, "OITSSBO");
}

void WeightedBlendSystem::Clear()
{
	auto* re = RenderEngine::Get();
	re->BindShader(clearShader);
	re->DispatchCompute(ceil(3840 / 32), ceil(2160 / 32), 1);
}

void WeightedBlendSystem::Clean()
{
	auto* re = RenderEngine::Get();
	auto* ssbom = re->GetSSBOManager();
	ssbom->DeleteSSBO("AlphaBuffer");
}
