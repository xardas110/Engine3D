#include "VolumetricLightSystem.h"
#include "RenderEngine.h"
#include "World.h"
#include "renderwindow.h"

void VolumetricLightSystem::Init()
{
	InitShader();
	InitShaderBindings();
	InitFBOAndTextures();
}

void VolumetricLightSystem::InitShader()
{
	auto* re = RenderEngine::Get();

	re->GetShader(ShaderManager::VolumetricLight, volumetricLight.shader);
}

void VolumetricLightSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();

	re->BindShader(volumetricLight.shader);
	volumetricLight.bind.depthBuffer = re->GetUniformLocation("gBuffer.depth");
	volumetricLight.bind.smCascades = re->GetUniformLocation("smCascades");
	volumetricLight.bind.numSteps = re->GetUniformLocation("numSteps");
	volumetricLight.bind.scattering = re->GetUniformLocation("scattering");
	volumetricLight.bind.sunColor = re->GetUniformLocation("sunColor");
}

void VolumetricLightSystem::InitFBOAndTextures()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto downSampleWidth = rw->GetWidth() / downSample;
	auto downSampleHeight = rw->GetHeight() / downSample;

	if (!re->GenerateFBO("VolumetricLightFBO", volumetricLight.fbo))
	{
		printf("VolumetricLightFBO creation failed\n");
	}
	if (!re->GenerateTexture("VolumetricLightTexture", volumetricLight.fbo.texture))
	{
		printf("VolumetricLightTexture creation failed\n");
	}
	if (!re->GenerateTexture("VolumetricLightFilter", volumetricLight.filteredTexture))
	{
		printf("VolumetricLightFilter creation failed\n");
	}
	if (!re->GenerateTexture("VolumetricLightUpScale", volumetricLight.upScaledTexture))
	{
		printf("VolumetricLightUpScale creation failed\n");
	}

	re->BindFrameBuffer(volumetricLight.fbo);
	re->Bind2DTexture(volumetricLight.fbo.texture);
	re->CreateTexture2D(volumetricLight.fbo.texture, GL_RGBA16F, GL_RGBA, downSampleWidth, downSampleHeight, GL_FLOAT);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, volumetricLight.fbo.texture, 0);
	re->Bind2DBilinearFilter();
	re->BindFrameBuffer(0);

	re->Bind2DTexture(volumetricLight.filteredTexture);
	re->CreateTexture2D(volumetricLight.filteredTexture, GL_RGBA16F, GL_RGBA, downSampleWidth, downSampleHeight, GL_FLOAT);
	re->Bind2DBilinearFilter();

	re->Bind2DTexture(volumetricLight.upScaledTexture);
	re->CreateTexture2D(volumetricLight.upScaledTexture, GL_RGBA16F, GL_RGBA, rw->GetWidth(), rw->GetHeight(), GL_FLOAT);
	re->Bind2DBilinearFilter();
}

/**/
void VolumetricLightSystem::Process(World* world, const Texture& posBuffer, const Mesh& quad, const DeferredRendererConfig& config, const ABuffer* aBuffer)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	re->SetViewport(0, 0, volumetricLight.fbo.texture.width, volumetricLight.fbo.texture.height);

	re->BindFrameBuffer(volumetricLight.fbo);
	re->GLClear(GL_DEPTH_BUFFER_BIT);
	re->DisableDepthTest();
	re->DisableBlending();

	if (aBuffer)
	{
		re->BindImageTexture(0, aBuffer->indexBuffer, 0, false, 0, GL_READ_WRITE, GL_R32UI);
		re->BindACBO(aBuffer->acbo, 0);
	}

	DirLight dirlight;
	world->GetDirectionalLight(dirlight);

	re->BindShader(volumetricLight.shader);

	re->SetInt(volumetricLight.bind.numSteps, config.volumetricLight.numRays);
	re->SetFloat(volumetricLight.bind.scattering, config.volumetricLight.scattering);
	re->SetVec3(volumetricLight.bind.sunColor, config.volumetricLight.sunColor);

	re->SetTexture(posBuffer, GL_TEXTURE_2D, volumetricLight.bind.depthBuffer, 0);
	re->SetTexture(dirlight.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, volumetricLight.bind.smCascades, 1);

	re->DrawElements(quad, GL_TRIANGLES);

	re->EnableDepthTest();
	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
	re->BindFrameBuffer(0);
}

void VolumetricLightSystem::OnResize(unsigned width, unsigned height)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto downSampleWidth = width / downSample;
	auto downSampleHeight = height / downSample;

	re->Bind2DTexture(volumetricLight.fbo.texture);
	re->CreateTexture2D(volumetricLight.fbo.texture, GL_RGBA16F, GL_RGBA, downSampleWidth, downSampleHeight, GL_FLOAT);

	re->Bind2DTexture(volumetricLight.filteredTexture);
	re->CreateTexture2D(volumetricLight.filteredTexture, GL_RGBA16F, GL_RGBA, downSampleWidth, downSampleHeight, GL_FLOAT);

	re->Bind2DTexture(volumetricLight.upScaledTexture);
	re->CreateTexture2D(volumetricLight.upScaledTexture, GL_RGBA16F, GL_RGBA, rw->GetWidth(), rw->GetHeight(), GL_FLOAT);
}

void VolumetricLightSystem::Clean()
{
	auto* re = RenderEngine::Get();
	re->DeleteTexture("VolumetricLightTexture");
	re->DeleteTexture("VolumetricLightFilter");
	re->DeleteTexture("VolumetricLightUpScale");
	re->DeleteFBO("VolumetricLightFBO");
}