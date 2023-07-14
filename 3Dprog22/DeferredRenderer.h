#pragma once
#include <qopenglfunctions_4_5_core.h>
#include "GBuffer.h"
#include "shader.h"
#include "Mesh.h"
#include "VolumetricLighting.h"
#include "Dithering.h"
#include "BilateralSystem.h"
#include "texturemanager.h"
#include "SMAASystem.h"
#include "VolumetricLightSystem.h"
#include "Fog.h"
#include "GBufferSystem.h"
#include "HDRSystem.h"
#include "LightPassSystem.h"
#include "SSAOSystem.h"
#include "BlurSystem.h"
#include "GaussianBlurSystem.h"
#include "BloomSystem.h"
#include "DoFSystem.h"
#include "OITSystem.h"
#include "DeferredRendererConfig.h"
#include "PerlinNoise.h"
#include "VolumetricClouds.h"
#include "VoxelConeTracing.h"
#include "WeightedBlendSystem.h"
#include "WaterSystem.h"

class DeferredRenderer : protected QOpenGLFunctions_4_5_Core
{
	friend class World;
	friend class Editor;

	void Init(class World* world);
	void InitRenderSettingsUBO(class World* world);
	void InitQuadMesh(class World* world);

	void UpdateRenderSettingsUBO();
	void Update(class World* world, float deltatime);
	void Render(class World* world, float deltatime);
	void ShadowPass(class World* world);
	void VoxelPass(class World* world);
	void MainPass(class World* world);
	void AlphaPass(class World* world);
	void PostProcessPass(class World* world);
	void FilteringPass(class World* world);
	void IndirectLightPass(class World* world);
	void LightPass(class World* world);
	void BloomPass();
	void DoFPass(class World* world);
	void HDRPass(class World* world);
	void BlitFrameBuffer();
	void ForwardPass(class World* world, float deltatime);
	void OnResize();

	void CleanRenderSettingsUBO();

	void Clear();
	void Clean();
private:

	const DeferredRendererConfig& LoadConfig(const std::string& path);

	int GetWindowWidth();
	int GetWindowHeight();

	SMAASystem smaaSystem;
	BilateralSystem bilateralSystem;
	VolumetricLightSystem volumetricLightSystem;
	VolumetricClouds volumetricClouds;

	GBufferSystem gBufferSystem;
	HDRSystem hdrSystem;
	LightPassSystem lightPassSystem;
	SSAOSystem ssaoSystem;
	BlurSystem blurSystem;
	GaussianBlurSystem gaussianBlurSystem;
	BloomSystem bloomSystem;
	DoFSystem dofSystem;
	PerlinNoise perlinNoise;
	OITSystem oitSystem;
	WaterSystem waterSystem;
	//WeightedBlendSystem wbSystem;

	VoxelConeTracing voxelConeTracing;

	Mesh quadMesh;
	Mesh unitQuadMesh;

	DeferredRendererConfig config;
	RenderSettings renderSettings;
	UBO renderSettingsUBO;
public:
    void ReloadVoxels(class World* world);

	void LoadRenderConfig(const std::string& jsonPath);
};

