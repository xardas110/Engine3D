#include "VolumetricClouds.h"
#include "RenderEngine.h"
#include "renderwindow.h"

void VolumetricClouds::Init(World* world)
{
	initializeOpenGLFunctions();

	perlinWorley.Init(world);
	worley.Init(world);
	weather.Init(world);

	InitShader();
	InitShaderBindings();
	InitFrameBuffer();
}

void VolumetricClouds::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::VolumetricClouds, shader);
}

void VolumetricClouds::InitShaderBindings()
{
	auto* re = RenderEngine::Get();
	re->BindShader(shader);

	vcb.fTime = re->GetUniformLocation("fTime");

	vcb.gBuffer.b0 = re->GetUniformLocation("gBuffer.b0");

	vcb.perlinWorley = re->GetUniformLocation("perlinWorley");
	vcb.worley = re->GetUniformLocation("worley");
	vcb.weather = re->GetUniformLocation("weather");

	vcb.vc.absorption = re->GetUniformLocation("vc.absorption");
	vcb.vc.atmosphereInner = re->GetUniformLocation("vc.atmosphereInner");
	vcb.vc.atmosphereOuter = re->GetUniformLocation("vc.atmosphereOuter");
	vcb.vc.bExpensive = re->GetUniformLocation("vc.bExpensive");
	vcb.vc.cloudColorBot = re->GetUniformLocation("vc.cloudColorBot");
	vcb.vc.cloudSpeed = re->GetUniformLocation("vc.cloudSpeed");
	vcb.vc.coverageMultiplier = re->GetUniformLocation("vc.coverageMultiplier");
	vcb.vc.crispiness = re->GetUniformLocation("vc.crispiness");
	vcb.vc.curliness = re->GetUniformLocation("vc.curliness");
	vcb.vc.densityFactor = re->GetUniformLocation("vc.densityFactor");
	vcb.vc.earthRadius = re->GetUniformLocation("vc.earthRadius");
	vcb.vc.numCloudSteps = re->GetUniformLocation("vc.numCloudSteps");
	vcb.vc.numShadowSteps = re->GetUniformLocation("vc.numShadowSteps");
	vcb.vc.numVolumetricLightSteps = re->GetUniformLocation("vc.numVolumetricLightSteps");
	vcb.vc.vlScattering = re->GetUniformLocation("vc.vlScattering");
	vcb.vc.windDirection = re->GetUniformLocation("vc.windDirection");
	vcb.vc.vlLightColor = re->GetUniformLocation("vc.vlLightColor");
}

void VolumetricClouds::InitFrameBuffer()
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();
	auto* fm = re->GetFBOManager();
	auto* tm = re->GetTextureManager();

	auto width = rw->GetWidth();
	auto height = rw->GetHeight();

	auto downscaledWidth = float(width) * downScale;
	auto downscaledHeight = float(height) * downScale;

	fm->GenerateFBO("VolumetricClouds", fbo);
	re->BindFrameBuffer(fbo);

	tm->GenerateTexture("vc.cloudColor", cloudColor);
	tm->GenerateTexture("vc.cloudColorBlur", cloudColorBlur);
	tm->GenerateTexture("vc.cloudColorUpscaled", cloudColorUpscaled);

	tm->GenerateTexture("vc.cloudShadowVL", cloudShadowVL);

	re->Bind2DTexture(cloudColorBlur);
	re->CreateTexture2D(
		cloudColorBlur,
		GL_RGBA, GL_RGBA,
		downscaledWidth,
		downscaledHeight,
		GL_UNSIGNED_BYTE);
	re->Bind2DBilinearFilter();

	re->Bind2DTexture(cloudColorUpscaled);
	re->CreateTexture2D(
		cloudColorUpscaled,
		GL_RGBA, GL_RGBA,
		width,
		height,
		GL_UNSIGNED_BYTE);
	re->Bind2DBilinearFilter();

	re->Bind2DTexture(cloudColor);
	re->CreateTexture2D(
		cloudColor, 
		GL_RGBA16F, GL_RGBA,
		downscaledWidth, 
		downscaledHeight, 
		GL_FLOAT);
	re->Bind2DBilinearFilter();
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cloudColor, 0);

	re->Bind2DTexture(cloudShadowVL);
	re->CreateTexture2D(
		cloudShadowVL, 
		GL_RGBA16F, GL_RGBA, 
		downscaledWidth, 
		downscaledHeight, 
		GL_FLOAT);
	re->Bind2DBilinearFilter();
	re->BindFrameBufferTexture2D(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, cloudShadowVL, 0);

	unsigned buffers[] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Volumetric FBO creation failed\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VolumetricClouds::Process(World* world, const GBuffer& gBuffer, const Mesh& screenQuad, const DeferredRendererConfig::VolumetricClouds& vcConfig)
{
	auto* re = RenderEngine::Get();
	auto* rw = RenderWindow::Get();

	auto width = float(rw->GetWidth()) * downScale;
	auto height = float(rw->GetHeight()) * downScale;

	if (downScale != vcConfig.downScale)
	{ 
		downScale = vcConfig.downScale;
		OnResize(rw->GetWidth(), rw->GetHeight());
	}
	
	re->SetViewport(0, 0, width, height);
	re->BindFrameBuffer(fbo);
	re->GLClear(GL_COLOR_BUFFER_BIT);
	re->DisableDepthTest();
	re->DisableBlending();
	re->BindShader(shader);

	re->SetFloat(vcb.fTime, World::GetElapsedTimeInSeconds());

	re->SetTexture(gBuffer.b0, GL_TEXTURE_2D, vcb.gBuffer.b0, 0);

	re->SetTexture(perlinWorley.texture, GL_TEXTURE_3D, vcb.perlinWorley, 2);
	re->SetTexture(worley.texture, GL_TEXTURE_3D, vcb.worley, 3);
	re->SetTexture(weather.texture, GL_TEXTURE_2D, vcb.weather, 4);
	
	re->SetFloat(vcb.vc.absorption, vcConfig.lightAbsorption);
	re->SetFloat(vcb.vc.atmosphereInner, vcConfig.atmosphereInnerRadius);
	re->SetFloat(vcb.vc.atmosphereOuter, vcConfig.atmosphereOuterRadius);
	re->SetInt(vcb.vc.bExpensive, vcConfig.bExpensive);
	re->SetVec3(vcb.vc.cloudColorBot, vcConfig.cloudBotColor);
	re->SetFloat(vcb.vc.cloudSpeed, vcConfig.cloudSpeed);
	re->SetFloat(vcb.vc.coverageMultiplier, vcConfig.cloudCoverage);
	re->SetFloat(vcb.vc.crispiness, vcConfig.cloudFluffyness);
	re->SetFloat(vcb.vc.curliness, vcConfig.cloudCurliness);
	re->SetFloat(vcb.vc.densityFactor, vcConfig.cloudDensity);
	re->SetFloat(vcb.vc.earthRadius, vcConfig.earthRadius);
	re->SetInt(vcb.vc.numCloudSteps, vcConfig.numCloudRaySteps);
	re->SetInt(vcb.vc.numShadowSteps, vcConfig.numShadowRaySteps);
	re->SetInt(vcb.vc.numVolumetricLightSteps, vcConfig.numVolumetricLightRaySteps);
	re->SetFloat(vcb.vc.vlScattering, vcConfig.volumetricLightScattering);
	re->SetVec3(vcb.vc.windDirection, vcConfig.windDirection);
	re->SetVec3(vcb.vc.vlLightColor, vcConfig.vlLightColor);

	re->DrawElements(screenQuad, GL_TRIANGLES);

	re->EnableDepthTest();
	re->SetViewport(0, 0, rw->GetWidth(), rw->GetHeight());
	re->BindFrameBuffer(0);
}

void VolumetricClouds::OnResize(int width, int height)
{
	auto* re = RenderEngine::Get();

	float downscaledWidth = float(width) * downScale;
	float downscaledHeight = float(height) * downScale;

	re->Bind2DTexture(cloudColor);	
	re->CreateTexture2D(
		cloudColor,
		GL_RGBA16F, GL_RGBA,
		downscaledWidth,
		downscaledHeight,
		GL_FLOAT);

	re->Bind2DTexture(cloudShadowVL);
	re->CreateTexture2D(
		cloudShadowVL,
		GL_RGBA16F, GL_RGBA,
		downscaledWidth,
		downscaledHeight,
		GL_FLOAT);

	if (IsDownscaled())
	{ 
		re->Bind2DTexture(cloudColorBlur);
		re->CreateTexture2D(
			cloudColorBlur,
			GL_RGBA, GL_RGBA,
			downscaledWidth,
			downscaledHeight,
			GL_UNSIGNED_BYTE);

		re->Bind2DTexture(cloudColorUpscaled);
		re->CreateTexture2D(
			cloudColorUpscaled,
			GL_RGBA, GL_RGBA,
			width,
			height,
			GL_UNSIGNED_BYTE);
	}
}

void VolumetricClouds::Clean()
{
	auto* re = RenderEngine::Get();

	perlinWorley.Clean();
	worley.Clean();
	weather.Clean();

	re->DeleteTexture("vc.cloudColor");
	re->DeleteTexture("vc.cloudColorBlur");
	re->DeleteTexture("vc.cloudColorUpscaled");

	re->DeleteTexture("vc.cloudShadowVL");
	re->DeleteTexture("vc.cloudShadowVLBlur");
	re->DeleteTexture("vc.cloudShadowVLUpscaled");
	
	re->DeleteFBO("VolumetricClouds");
}

const PerlinWorley3D& VolumetricClouds::GetPWTexture() const
{
	return perlinWorley;
}

const Texture& VolumetricClouds::GetCloudColorTexture() const
{
	return cloudColor;
}

const Texture& VolumetricClouds::GetCloudShadowVLTexture() const
{
	return cloudShadowVL;
}

bool VolumetricClouds::IsDownscaled() const
{
	return !Cmp(downScale, 1.f);
}

const Worley3D& VolumetricClouds::GetWorleyTexture() const
{
	return worley;
}

const Texture& VolumetricClouds::GetWeatherTexture() const
{
	return weather.texture;
}
