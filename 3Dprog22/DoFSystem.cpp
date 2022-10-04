#include "DoFSystem.h"
#include "World.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include <random>

void DoFSystem::Init()
{
	InitShader();
	InitShaderBindings();
	InitFBO();
	InitKernel();
	UploadKernel();
}

void DoFSystem::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::DoF, dof.shader);
}

void DoFSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();
	
	re->BindShader(dof.shader);
	db.colorTexture = re->GetUniformLocation("colorTexture");
	db.depthTexture = re->GetUniformLocation("depthTexture");
	db.targetDim = re->GetUniformLocation("targetDim");
	db.zNear = re->GetUniformLocation("zNear");
	db.zFar = re->GetUniformLocation("zFar");
	db.fov = re->GetUniformLocation("fov");
	db.focusScale = re->GetUniformLocation("focusScale");
	db.uFar = re->GetUniformLocation("uFar");
	db.maxBlurSize = re->GetUniformLocation("maxBlurSize");
	db.radScale = re->GetUniformLocation("radScale");

	for (int i = 0; i < DOF_UC_SIZE; i++)
	{
		std::string location = "uc[" + std::to_string(i) + "]";
		db.uc[i] = re->GetUniformLocation(location);
	}
}

void DoFSystem::InitFBO()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	re->GenerateFBO("DoF", dof.fbo);
	re->GenerateTexture("DoFBlur", dof.blur);

	re->Bind2DTexture(dof.blur);
	re->CreateTexture2D(dof.blur, GL_RGBA16F, GL_RGBA, rw->GetWidth(), rw->GetHeight(), GL_FLOAT);
	re->Bind2DBilinearFilter();
	re->Bind2DClampToEdge();

	re->BindFrameBuffer(dof.fbo);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dof.blur, 0);
}

void DoFSystem::InitKernel()
{
	std::uniform_real_distribution<float> rf(0.f, 1.f);
	std::uniform_real_distribution<float> rf1(0.1f, 1.f);
	std::default_random_engine generator;

	for (int i = 0; i < dof.uc.size(); i++)
	{
		float x = rf(generator) * 2.f - 1.f;
		float y = rf(generator) * 2.f - 1.f;

		dof.uc[i] = glm::normalize(glm::vec2(x, y));
		dof.uc[i] *= rf1(generator);
	}
}

void DoFSystem::UploadKernel()
{
	auto* re = RenderEngine::Get();
	re->BindShader(dof.shader);

	for (int i = 0; i < dof.uc.size(); i++)
	{
		re->SetVec2(db.uc[i], dof.uc[i]);
	}
}

void DoFSystem::Process(World* world, const Texture& colorTexture, const Texture& depthTexture, const Mesh& quad, const DeferredRendererConfig& config)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();
	auto* camera = world->GetRenderCamera();

	re->SetViewport(0, 0, dof.blur.width, dof.blur.height);
	re->BindFrameBuffer(dof.fbo);
	re->GLClear(GL_COLOR_BUFFER_BIT);
	re->DisableDepthTest();

	re->BindShader(dof.shader);

	re->SetTexture(colorTexture, GL_TEXTURE_2D, db.colorTexture, 0);
	re->SetTexture(depthTexture, GL_TEXTURE_2D, db.depthTexture, 1);
	re->SetVec2(db.targetDim, glm::vec2(dof.blur.width, dof.blur.height));
	
	re->SetFloat(db.zNear, camera->GetNear());
	re->SetFloat(db.zFar, camera->GetFar());
	re->SetFloat(db.fov, camera->GetFov());
	re->SetFloat(db.focusScale, config.dof.focusScale);
	re->SetFloat(db.uFar, config.dof.uFar);
	re->SetFloat(db.maxBlurSize, config.dof.maxBlurSize);
	re->SetFloat(db.radScale, config.dof.radScale);

	re->DrawElements(quad, GL_TRIANGLES);

	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
	re->EnableDepthTest();
	re->BindFrameBuffer(0);
}

void DoFSystem::OnResize(int width, int height)
{
	auto* re = RenderEngine::Get();

	re->Bind2DTexture(dof.blur);
	re->CreateTexture2D(dof.blur, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
}

void DoFSystem::Clean()
{
	auto* re = RenderEngine::Get();
	re->DeleteTexture("DoFBlur");
	re->DeleteFBO("DoF");
}
