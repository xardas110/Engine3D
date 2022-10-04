#include "BilateralSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include "Include/glm/glm.hpp"

void BilateralSystem::Init(World* world)
{
	auto* re = RenderEngine::Get();

	re->GetShader(ShaderManager::Bilateral, bilateral.shader);

	re->BindShader(bilateral.shader);
	bilateral.bind.source = re->GetUniformLocation("source");
    bilateral.bind.targetDim = re->GetUniformLocation("targetDim");

    re->GenerateFBO("BilateralFBO", bilateral.fbo);
}

void BilateralSystem::Filter(const Texture& source, const Texture& target, const Mesh& quad)
{
    auto* re = RenderEngine::Get();
    auto* rw = RenderWindow::Get();

    re->SetViewport(0, 0, target.width, target.height);
    re->BindFrameBuffer(bilateral.fbo);
    re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target, 0);
    
    re->BindShader(bilateral.shader);
    re->DisableDepthTest();
    re->DisableBlending();
    re->SetTexture(source, GL_TEXTURE_2D, bilateral.bind.source, 0);
    re->SetVec2(bilateral.bind.targetDim, glm::vec2(target.width, target.height));
    re->DrawElements(quad, GL_TRIANGLES);

    re->BindFrameBuffer(0);
    re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
}

void BilateralSystem::Clean()
{
    auto* re = RenderEngine::Get();
    re->DeleteFBO("BilateralFBO");
}
