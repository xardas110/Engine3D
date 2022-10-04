#include "HDRSystem.h"
#include "RenderEngine.h"

void HDRSystem::Init()
{
	InitShader();
	InitShaderBindings();
}

void HDRSystem::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::HDR, hdrShader);
}

void HDRSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();
	re->BindShader(hdrShader);
	hdrb.colorTexture = re->GetUniformLocation("colorTexture");
	hdrb.bloomTexture = re->GetUniformLocation("bloomTexture");
	hdrb.bloomEnabled = re->GetUniformLocation("config.bloomEnabled");
}

void HDRSystem::Render(const Texture& lightPassTexture, const Texture& bloomTexture, const Mesh& quad, const DeferredRendererConfig& config)
{
	auto* re = RenderEngine::Get();

	re->DisableBlending();
	re->DisableDepthTest();

	re->BindShader(hdrShader);
	re->SetInt(hdrb.bloomEnabled, config.bloom.enabled);
	re->SetTexture(lightPassTexture, GL_TEXTURE_2D, hdrb.colorTexture, 0);
	re->SetTexture(bloomTexture, GL_TEXTURE_2D, hdrb.bloomTexture, 1);
	re->DrawElements(quad, GL_TRIANGLES);

	re->EnableDepthTest();
}

void HDRSystem::Clean()
{
	// nothing to clean
}
