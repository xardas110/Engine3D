#include "GBufferSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"

void GBufferSystem::Init()
{
	initializeOpenGLFunctions();

	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();
	auto* fm = re->GetFBOManager();
	auto* tm = re->GetTextureManager();

	auto width = rw->GetWidth();
	auto height = rw->GetHeight();

	fm->GenerateFBO("GBuffer", gBuffer.fbo);
	re->BindFrameBuffer(gBuffer.fbo);

	tm->GenerateTexture("GBuffer.b0", gBuffer.b0);
	tm->GenerateTexture("GBuffer.depth", gBuffer.fbo.texture);

	re->Bind2DTexture(gBuffer.b0);
	re->CreateTexture2D(gBuffer.b0, GL_RGBA32UI, GL_RGBA_INTEGER, width, height, GL_UNSIGNED_INT);
	re->Bind2DNoFilter();
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.b0, 0);

	re->Bind2DTexture(gBuffer.fbo.texture);
	re->CreateTexture2D(gBuffer.fbo.texture, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, width, height, GL_FLOAT);
	re->Bind2DNoFilter();
	re->BindFrameBufferTexture2D(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gBuffer.fbo.texture, 0);

	unsigned buffers[] = {GL_COLOR_ATTACHMENT0};

	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{ 
		printf("GBUFFER FBO creation failed\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBufferSystem::BindGBuffer()
{
	auto* re = RenderEngine::Get();

	re->BindFrameBuffer(gBuffer.fbo);
	re->EnableDepthTest();
	re->SetDepthMask(true);
	re->SetDepthFunc(GL_LESS);
	re->EnableCulling();
	re->SetCulling(GL_FRONT);
	re->DisableBlending();
}

void GBufferSystem::UnbindGBuffer()
{
	auto* re = RenderEngine::Get();

	re->DisableDepthTest();
	re->BindFrameBuffer(0);
}

void GBufferSystem::ClearGBuffer()
{
	auto* re = RenderEngine::Get();
	re->BindFrameBuffer(gBuffer.fbo);

	static glm::vec4 zeroFillerVec(0.f);
	static glm::vec4 oneFillerVec(1.f);

	re->GLClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
	// use a four component float array or a glm::vec4(0.0)
	glClearBufferfv(GL_COLOR, 1, &zeroFillerVec[0]);
	// use a four component float array or a glm::vec4(1.0)
	glClearBufferfv(GL_COLOR, 2, &oneFillerVec[0]);
}

void GBufferSystem::OnResize(int width, int height)
{
	auto* re = RenderEngine::Get();

	re->Bind2DTexture(gBuffer.b0);
	re->CreateTexture2D(gBuffer.b0, GL_RGBA32UI, GL_RGBA_INTEGER, width, height, GL_UNSIGNED_INT);

	re->Bind2DTexture(gBuffer.fbo.texture);
	re->CreateTexture2D(gBuffer.fbo.texture, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, width, height, GL_FLOAT);
}

void GBufferSystem::Clean()
{
	auto* re = RenderEngine::Get();

	re->DeleteFBO("GBuffer");
	re->DeleteTexture("GBuffer.b0");
	re->DeleteTexture("GBuffer.accum");
	re->DeleteTexture("GBuffer.reveal");
	re->DeleteTexture("GBuffer.depth");
}
