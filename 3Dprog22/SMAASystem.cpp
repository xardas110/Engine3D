#include "SMAASystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include "Include/glm/glm.hpp"

void SMAASystem::Init(World* world)
{
	auto* re = RenderEngine::Get();

	re->GetShader(ShaderManager::SMAA, smaa.shader);

	re->BindShader(smaa.shader);
	smaa.bind.source = re->GetUniformLocation("source");
	smaa.bind.targetDim = re->GetUniformLocation("targetDim");

	re->GenerateFBO("SMAAFBO", smaa.fbo);
}

void SMAASystem::Filter(const Texture& source, const Texture& target, const Mesh& quad)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	re->SetViewport(0, 0, target.width, target.height);

	re->BindFrameBuffer(smaa.fbo);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target, 0);
	re->DisableDepthTest();
	re->DisableBlending();

	re->BindShader(smaa.shader);
	re->SetTexture(source, GL_TEXTURE_2D, smaa.bind.source, 0);
	re->SetVec2(smaa.bind.targetDim, glm::vec2(target.width, target.height));
	re->DrawElements(quad, GL_TRIANGLES);

	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
	re->BindFrameBuffer(0);
	re->EnableDepthTest();
}

void SMAASystem::Clean()
{
	auto* re = RenderEngine::Get();
	re->DeleteFBO("SMAAFBO");
}
