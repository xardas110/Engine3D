#include "BlurSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"

void BlurSystem::Init()
{
	InitShader();
	InitShaderBindings();
	InitFBO();
}

void BlurSystem::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::Blur, blur.shader);
}

void BlurSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();
	re->BindShader(blur.shader);
	bb.source = re->GetUniformLocation("source");
    bb.targetDim = re->GetUniformLocation("targetDim");
}

void BlurSystem::InitFBO()
{
	auto* re = RenderEngine::Get();
	re->GenerateFBO("Blur", blur.fbo);
}

void BlurSystem::Filter(const Texture& source, const Texture& target, const Mesh& quad)
{
    auto* re = RenderEngine::Get();
    auto* rw = RenderWindow::Get();

    re->SetViewport(0, 0, target.width, target.height);
    re->BindFrameBuffer(blur.fbo);
    re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target, 0);

    re->BindShader(blur.shader);
    re->DisableDepthTest();
    re->DisableBlending();
    re->SetTexture(source, GL_TEXTURE_2D, bb.source, 0);
    re->SetVec2(bb.targetDim, glm::vec2(target.width, target.height));
    re->DrawElements(quad, GL_TRIANGLES);

    re->BindFrameBuffer(0);
    re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
}

void BlurSystem::Clean()
{
    auto* re = RenderEngine::Get();
    re->DeleteFBO("Blur");
}
