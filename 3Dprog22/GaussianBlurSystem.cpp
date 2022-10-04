#include "GaussianBlurSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include <string>

void GaussianBlurSystem::Init()
{
	InitShader();
	InitShaderBindings();
	InitFBOs();
}

void GaussianBlurSystem::InitShader()
{
	auto* re = RenderEngine::Get();

	re->GetShader(ShaderManager::GaussianBlur, gb.shader);
}

void GaussianBlurSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();

	re->BindShader(gb.shader);

	gbb.source = re->GetUniformLocation("source");
	gbb.bHorizontal = re->GetUniformLocation("bHorizontal");
	gbb.n = re->GetUniformLocation("n");

	for (size_t i = 0; i < GB_WEIGHTS_SIZE; i++)
	{
		std::string location = "weights[" + std::to_string(i) + "]";
		gbb.weights[i] = re->GetUniformLocation(location);
	}
}

void GaussianBlurSystem::InitFBOs()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	re->GenerateFBO("GaussianBlur1", gb.fbo[0]);
	re->GenerateFBO("GaussianBlur2", gb.fbo[1]);
	re->GenerateTexture("GaussianTexture1", gb.fbo[0].texture);
	re->GenerateTexture("GaussianTexture2", gb.fbo[1].texture);

	re->Bind2DTexture(gb.fbo[0].texture);
	re->CreateTexture2D(gb.fbo[0].texture, GL_RGBA16F, GL_RGBA, rw->GetWidth(), rw->GetHeight(), GL_FLOAT);
	re->Bind2DBilinearFilter();
	re->Bind2DClampToEdge();

	re->Bind2DTexture(gb.fbo[1].texture);
	re->CreateTexture2D(gb.fbo[1].texture, GL_RGBA16F, GL_RGBA, rw->GetWidth(), rw->GetHeight(), GL_FLOAT);
	re->Bind2DBilinearFilter();
	re->Bind2DClampToEdge();

	re->BindFrameBuffer(gb.fbo[0]);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gb.fbo[0].texture, 0);

	re->BindFrameBuffer(gb.fbo[1]);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gb.fbo[1].texture, 0);
}

void GaussianBlurSystem::Filter(const Texture& source, const Texture& target, const Mesh& quad, int n)
{	
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	re->BindShader(gb.shader);

	re->DisableDepthTest();

	auto& width = target.width;
	auto& height = target.height;

	int amount = n * 2;

	re->Bind2DTexture(gb.fbo[0].texture);
	re->CreateTexture2D(gb.fbo[0].texture, target.format, target.formatType, width, height, target.dataType);

	re->Bind2DTexture(gb.fbo[1].texture);
	re->CreateTexture2D(gb.fbo[1].texture, target.format, target.formatType, width, height, target.dataType);

	re->BindFrameBuffer(gb.fbo[0]);
	re->GLClear(GL_COLOR_BUFFER_BIT);

	re->BindFrameBuffer(gb.fbo[1]);
	re->GLClear(GL_COLOR_BUFFER_BIT);

	re->SetViewport(0, 0, width, height);

	re->SetInt(gbb.n, n);

	bool bHorizontal{ true }, bSource{ true };

	for (int i = 0; i < amount - 1; i++)
	{
		re->BindFrameBuffer(gb.fbo[bHorizontal]);
		re->SetInt(gbb.bHorizontal, bHorizontal);

		if (bSource)
		{
			re->SetTexture(source, GL_TEXTURE_2D, gbb.source, 0);
		}
		else
		{
			re->SetTexture(gb.fbo[!bHorizontal].texture, GL_TEXTURE_2D, gbb.source, 0);		
		}

		re->DrawElements(quad, GL_TRIANGLES);
	
		bHorizontal = !bHorizontal;

		if (bSource) bSource = false;
	}

	re->BindFrameBuffer(gb.fbo[bHorizontal]);
	re->SetInt(gbb.bHorizontal, bHorizontal);

	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target, 0);
	re->SetTexture(gb.fbo[!bHorizontal].texture, GL_TEXTURE_2D, gbb.source, 0);
	re->DrawElements(quad, GL_TRIANGLES);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gb.fbo[bHorizontal].texture, 0);

	re->EnableDepthTest();
	re->BindFrameBuffer(0);
	re->SetViewport(0,0, rw->GetWidth(), rw->GetHeight());
}

void GaussianBlurSystem::Clean()
{
	auto* re = RenderEngine::Get();

	re->DeleteTexture("GaussianTexture1");
	re->DeleteTexture("GaussianTexture2");
	re->DeleteFBO("GaussianBlur1");
	re->DeleteFBO("GaussianBlur2");	
}
