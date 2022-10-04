#include "LightPassSystem.h"
#include "RenderEngine.h"
#include "renderwindow.h"
#include "World.h"
#include <chrono>

using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

void LightPassSystem::Init()
{
	initializeOpenGLFunctions();
	InitShader();
	InitShaderBindings();
	InitLightPass();
}

void LightPassSystem::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::DeferredLightPass, lp.shader);
}

void LightPassSystem::InitShaderBindings()
{
	auto* re = RenderEngine::Get();
	re->BindShader(lp.shader);

	lpb.gBuf.b0 = re->GetUniformLocation("gBuffer.b0");
	lpb.gBuf.depth = re->GetUniformLocation("gBuffer.depth");

	lpb.SSAOmap = re->GetUniformLocation("SSAOmap");

	lpb.indirectDiffuseAO = re->GetUniformLocation("indirectLight.diffuseAO");
	lpb.indirectSpecularShadow = re->GetUniformLocation("indirectLight.specularShadow");

	lpb.shadowMaps = re->GetUniformLocation("shadowMaps");

	lpb.volumetricLightmap = re->GetUniformLocation("volumetricLightmap");

	lpb.cloudColorTex = re->GetUniformLocation("cloudColorTex");
	lpb.cloudShadowVLTex = re->GetUniformLocation("cloudShadowVLTex");
	lpb.fTime = re->GetUniformLocation("fTime");

	lpb.SSAOmap = re->GetUniformLocation("SSAOmap");
}

void LightPassSystem::InitLightPass()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();
	auto* fm = re->GetFBOManager();
	auto* tm = re->GetTextureManager();

	auto width = rw->GetWidth();
	auto height = rw->GetHeight();

	fm->GenerateFBO("LightPass", lp.fbo);
	tm->GenerateTexture("LightPassColor", lp.color);
	tm->GenerateTexture("LightPassBrightColor", lp.brightColor);

	re->Bind2DTexture(lp.color);
	re->CreateTexture2D(lp.color, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
	re->Bind2DNoFilter();

	re->Bind2DTexture(lp.brightColor);
	re->CreateTexture2D(lp.brightColor, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);
	re->Bind2DNoFilter();

	re->BindFrameBuffer(lp.fbo);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lp.color, 0);
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lp.brightColor, 0);
	
	unsigned buffers[] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("LP FBO creation failed\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void LightPassSystem::Render(
	class World* world, 
	const GBuffer& gBuffer, 
	const ABuffer* aBuffer, 
	const Texture& volumetricLightmap, 
	const Texture& SSAOmap, 
	const Mesh& quad, 
	const IndirectLight& indirectLight,
	const VolumetricClouds& vc)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto camFar = world->GetRenderCamera()->GetFar();

	DirLight dirlight;
	world->GetDirectionalLight(dirlight);

	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
	re->BindFrameBuffer(lp.fbo);
	re->GLClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	re->DisableBlending();

	if (aBuffer)
		re->BindImageTexture(0, 
			aBuffer->indexBuffer, 
			0, 
			false, 0, 
			GL_READ_ONLY, 
			aBuffer->indexBuffer.format);

	re->BindShader(lp.shader);

	re->SetTexture(gBuffer.b0, GL_TEXTURE_2D, lpb.gBuf.b0, 0);
	re->SetTexture(gBuffer.fbo.texture, GL_TEXTURE_2D, lpb.gBuf.depth, 1);
	re->SetTexture(dirlight.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, lpb.shadowMaps, 2);
	re->SetTexture(volumetricLightmap, GL_TEXTURE_2D, lpb.volumetricLightmap, 3);

	re->SetTexture(SSAOmap, GL_TEXTURE_2D, lpb.SSAOmap, 4);

	re->SetTexture(indirectLight.specularShadow, GL_TEXTURE_2D, lpb.indirectDiffuseAO, 17);
	re->SetTexture(indirectLight.specularShadow, GL_TEXTURE_2D, lpb.indirectSpecularShadow, 18);


	if (vc.IsDownscaled())
	{ 
		re->SetTexture(vc.cloudColorUpscaled, GL_TEXTURE_2D, lpb.cloudColorTex, 10);
		re->SetTexture(vc.GetCloudShadowVLTexture(), GL_TEXTURE_2D, lpb.cloudShadowVLTex, 11);
	}
	else
	{
		re->SetTexture(vc.GetCloudColorTexture(), GL_TEXTURE_2D, lpb.cloudColorTex, 10);
		re->SetTexture(vc.GetCloudShadowVLTexture(), GL_TEXTURE_2D, lpb.cloudShadowVLTex, 11);
	}

	static std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	static std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto val = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();

	re->SetFloat(lpb.fTime, val);
	end = std::chrono::steady_clock::now();

	re->DrawElements(quad, GL_TRIANGLES);

	re->EnableDepthTest();
	re->SetDepthFunc(GL_LESS);
	re->BindFrameBuffer(0);
}

void LightPassSystem::OnResize(int width, int height)
{
	auto* re = RenderEngine::Get();

	re->Bind2DTexture(lp.color);
	re->CreateTexture2D(lp.color, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);

	re->Bind2DTexture(lp.brightColor);
	re->CreateTexture2D(lp.brightColor, GL_RGBA16F, GL_RGBA, width, height, GL_FLOAT);

	glBindRenderbuffer(GL_RENDERBUFFER, lp.fbo.texture.textureID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	lp.fbo.texture.width = width;
	lp.fbo.texture.height = height;
}

void LightPassSystem::OnClear()
{
	auto* re = RenderEngine::Get();
	re->BindFrameBuffer(lp.fbo);
	re->GLClear(GL_COLOR_BUFFER_BIT);
	
	static glm::vec4 zeroFiller(0.f);
	re->ClearBuffer(GL_COLOR, 1, &zeroFiller[0]);
}

void LightPassSystem::Clean()
{
	auto* re = RenderEngine::Get();

	re->DeleteFBO("LightPass");
	re->DeleteTexture("LightPassColor");
	re->DeleteTexture("LightPassBrightColor");
}
