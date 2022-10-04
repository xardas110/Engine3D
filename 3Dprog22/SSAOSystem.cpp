#include "SSAOSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include <string>

float Lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void SSAOSystem::Init()
{
	InitShader();
	InitShaderBinings();
	InitKernel();
	InitKernelBindings();
	InitNoise();
	InitFBO();
	InitBlur();
	InitUpscaled();
}

void SSAOSystem::InitShader()
{
	auto* re = RenderEngine::Get();

	re->GetShader(ShaderManager::SSAO, ssao.shader);
}

void SSAOSystem::InitShaderBinings()
{
	auto* re = RenderEngine::Get();
	
	re->BindShader(ssao.shader);
	ssaob.gBufferNormal = re->GetUniformLocation("gBuffer.normal");
	ssaob.gBufferPos = re->GetUniformLocation("gBuffer.pos");
	ssaob.gBufferDepth = re->GetUniformLocation("gBuffer.depth");

	ssaob.noisemap = re->GetUniformLocation("noisemap");
	ssaob.dim = re->GetUniformLocation("dim");
	ssaob.bias = re->GetUniformLocation("bias");
	ssaob.radius = re->GetUniformLocation("radius");

	for (size_t i = 0; i < SSAO_KERNEL_SIZE; i++)
	{
		const std::string location = "kernel[" + std::to_string(i) + "]";
		ssaob.kernel[i] = re->GetUniformLocation(location);
	}
}

void SSAOSystem::InitKernel()
{
	//Source: https://learnopengl.com/Advanced-Lighting/SSAO
	//with modifications

	std::uniform_real_distribution<float> rf(0.f, 1.f);
	std::default_random_engine generator;

	for (auto i = 0; i < ssao.kernel.size(); i++)
	{
		float x = rf(generator) * 2.f - 1.f;
		float y = rf(generator) * 2.f - 1.f;
		float z = rf(generator);

		glm::vec3 sample = { x, y, z };
		sample = glm::normalize(sample);
		sample *= rf(generator);

		float scale = (float)i / (float)ssao.kernel.size();
		scale = Lerp(0.1f, 1.f, scale * scale);
		sample *= scale;

		ssao.kernel[i] = sample;
	}
}

void SSAOSystem::InitKernelBindings()
{
	auto* re = RenderEngine::Get();
	for (size_t i = 0; i < SSAO_KERNEL_SIZE; i++)
	{
		re->SetVec3(ssaob.kernel[i], ssao.kernel[i]);
	}
}

void SSAOSystem::InitNoise()
{
	auto* re = RenderEngine::Get();

	std::uniform_real_distribution<float> rf(0.f, 1.f);
	std::default_random_engine generator;

	for (size_t i = 0; i < SSAO_NOISE_SIZE; i++)
	{
		float x = rf(generator) * 2.f - 1.f;
		float y = rf(generator) * 2.f - 1.f;
		float z = 0.f;
		ssao.noise[i] = { x,y,z };
	}

	re->GenerateTexture("SSAONoise", ssao.noiseTex);
	re->Bind2DTexture(ssao.noiseTex);
	re->CreateTexture2D(ssao.noiseTex, GL_RGBA32F, GL_RGB, sqrt(SSAO_NOISE_SIZE), sqrt(SSAO_NOISE_SIZE), GL_FLOAT, 0U, 0U, &ssao.noise[0]);
	re->Bind2DNoFilter();
	re->Bind2DWrapRepeat();
}

void SSAOSystem::InitBlur()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto scaledWidth = rw->GetWidth() / downSample;
	auto scaledHeight = rw->GetHeight() / downSample;

	re->GenerateTexture("SSAOblur", ssao.blur);
	re->Bind2DTexture(ssao.blur);
	re->CreateTexture2D(ssao.blur, GL_RED, GL_RED, scaledWidth, scaledHeight, GL_FLOAT);
	re->Bind2DNoFilter();
}

void SSAOSystem::InitFBO()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto scaledWidth = rw->GetWidth() / downSample;
	auto scaledHeight = rw->GetHeight() / downSample;

	re->GenerateFBO("SSAO", ssao.fbo);
	re->GenerateTexture("SSAOColor", ssao.fbo.texture);

	re->Bind2DTexture(ssao.fbo.texture);
	re->CreateTexture2D(ssao.fbo.texture, GL_RED, GL_RED, scaledWidth, scaledHeight, GL_FLOAT);
	re->Bind2DNoFilter();
	
	re->BindFrameBuffer(ssao.fbo);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao.fbo.texture, 0);

	re->BindFrameBuffer(0);
}

void SSAOSystem::InitUpscaled()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	re->GenerateTexture("SSAOUpscaled", ssao.upscaledColor);

	re->Bind2DTexture(ssao.upscaledColor);
	re->CreateTexture2D(ssao.upscaledColor, GL_RED, GL_RED, rw->GetWidth(), rw->GetHeight(), GL_FLOAT);
	re->Bind2DNoFilter();
}

void SSAOSystem::UpdateNoise()
{
	auto* re = RenderEngine::Get();

	std::uniform_real_distribution<float> rf(0.f, 1.f);
	std::default_random_engine generator;

	for (size_t i = 0; i < SSAO_NOISE_SIZE; i++)
	{
		float x = rf(generator) * 2.f - 1.f;
		float y = rf(generator) * 2.f - 1.f;
		float z = 0.f;
		ssao.noise[i] = { x,y,z };
	}

	re->Bind2DTexture(ssao.noiseTex);
	re->CreateTexture2D(ssao.noiseTex, GL_RGBA32F, GL_RGB, sqrt(SSAO_NOISE_SIZE), sqrt(SSAO_NOISE_SIZE), GL_FLOAT, 0U, 0U, &ssao.noise[0]);
	re->Bind2DNoFilter();
	re->Bind2DWrapRepeat();
}

void SSAOSystem::Process(const GBuffer& gBuffer, const Mesh& quad, const DeferredRendererConfig& config)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto dims = glm::vec2(ssao.fbo.texture.width, ssao.fbo.texture.height);

	re->SetViewport(0,0, dims.x, dims.y);//in case of downscaling
	re->BindFrameBuffer(ssao.fbo);
	re->GLClear(GL_COLOR_BUFFER_BIT);

	re->BindShader(ssao.shader);
	
	re->SetFloat(ssaob.bias, config.ssao.bias);
	re->SetFloat(ssaob.radius, config.ssao.radius);

	re->SetTexture(gBuffer.b0, GL_TEXTURE_2D, ssaob.gBufferNormal, 0);
	re->SetTexture(ssao.noiseTex, GL_TEXTURE_2D, ssaob.noisemap, 2);
	re->SetTexture(gBuffer.fbo.texture, GL_TEXTURE_2D, ssaob.gBufferDepth, 3);

	re->SetVec2(ssaob.dim, dims);

	re->DrawElements(quad, GL_TRIANGLES);

	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
}

void SSAOSystem::OnResize(int width, int height)
{
	auto* re = RenderEngine::Get();

	auto scaledWidth = width / downSample;
	auto scaledHeight = height / downSample;

	re->Bind2DTexture(ssao.fbo.texture);
	re->CreateTexture2D(ssao.fbo.texture, GL_RED, GL_RED, scaledWidth, scaledHeight, GL_FLOAT);

	re->Bind2DTexture(ssao.blur);
	re->CreateTexture2D(ssao.blur, GL_RED, GL_RED, scaledWidth, scaledHeight, GL_FLOAT);

	re->Bind2DTexture(ssao.upscaledColor);
	re->CreateTexture2D(ssao.upscaledColor, GL_RED, GL_RED, width, height, GL_FLOAT);
}

void SSAOSystem::Clean()
{
	auto* re = RenderEngine::Get();

	re->DeleteFBO("SSAO");
	re->DeleteTexture("SSAOColor");
	re->DeleteTexture("SSAONoise");
	re->DeleteTexture("SSAOUpscaled");
}
