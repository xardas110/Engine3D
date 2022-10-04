#pragma once
#include "Worley3D.h"
#include "PerlinWorley3D.h"
#include "Weather.h"
#include "GBuffer.h"
#include "shader.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "DeferredRendererConfig.h"
#include "Texture.h"
#include <qopenglfunctions_4_5_core.h>

/// <summary>
/// ============================================
/// GPU GEM PRO 7, see p. 97
/// ============================================
/// Brief: 2 3D textures and 1 weather texture
/// ------------------------------------------------------------------------------
/// First 3D texture 128^3 consists of PerlinWorley in the red channel and 3 Worley noises at increasing frequences in GBA channels. 
/// Used to create the base shape of the clouds.
/// Second 3D texture 32^3 WorleyFBMs used as the edges for the base clouds
/// Weather texture is used to lookup cloud coverage and cloud type, precipitation will not be added.
/// ------------------------------------------------------------------------------
 
struct VCBindings
{
	int crispiness;
	int cloudSpeed;
	int curliness;
	int coverageMultiplier;
	int densityFactor;
	int absorption;
	int cloudColorBot;
	int windDirection;
	int vlLightColor;

	int earthRadius;
	int atmosphereInner;
	int atmosphereOuter;

	int vlScattering;

	int bExpensive;

	int numCloudSteps;
	int numShadowSteps;
	int numVolumetricLightSteps;
};

struct VolumetricCloudsBindings
{
	int perlinWorley;
	int worley;
	int weather;
	int fTime;

	VCBindings vc;
	GBufferBindings gBuffer;
};

class VolumetricClouds : protected QOpenGLFunctions_4_5_Core
{
	friend class DeferredRenderer;
	friend class Editor;

	void Init(class World* world);
	void InitShader();
	void InitShaderBindings();
	void InitFrameBuffer();

	void Process(
		class World* world,
		const GBuffer& gBuffer,
		const Mesh& screenQuad,
		const DeferredRendererConfig::VolumetricClouds& vcConfig);
	
	void OnResize(int width, int height);
	void Clean();
public:
	const Texture& GetWeatherTexture() const;
	const Worley3D& GetWorleyTexture() const;
	const PerlinWorley3D& GetPWTexture() const;

	const Texture& GetCloudColorTexture() const;
	const Texture& GetCloudShadowVLTexture() const;

	bool IsDownscaled() const;

	Texture					cloudColorBlur;
	Texture					cloudColorUpscaled;
private:
	float downScale			{ 0.5f };		/*resolution*downscale*/

	Shader					shader;

	FrameBuffer				fbo;
	Texture					cloudColor;
	Texture					cloudShadowVL;/*x:Shadow, yzw:volumetric light*/

	Weather					weather;
	Worley3D				worley;
	PerlinWorley3D			perlinWorley;

	VolumetricCloudsBindings vcb;
};

