#pragma once
#include <string>
#include "Include/glm/glm.hpp"

struct DeferredRendererConfig
{
	enum LightModel
	{
		PBR,
		Phong
	}lightModel{ LightModel::PBR };

	struct VoxelConeTracing
	{
		bool bDisplay{ false };
	}vct;
	struct GlobalIllumination
	{
		bool enabled{ true };
	}gi;
	struct DOF{
		bool enabled{true};
		float focusScale{ 0.f };
		float uFar{ 0.f };
		float maxBlurSize{ 0.f };
		float radScale{ 0.f };
	}dof;

	struct Bloom{
		bool enabled{ false };
		glm::vec3 threshold{ 0.f };
	}bloom;

	struct SSAO{
		bool enabled{ false };
		float radius{ 0.f };
		float bias{ 0.f };
	}ssao;

	struct VolumetricLight{
		bool enabled{ false };
		int numRays{ 0 };
		float scattering{ 0.f };
		glm::vec3 sunColor{ 0.f, 0.f, 0.f };
	}volumetricLight;

	struct VolumetricClouds {
		bool enabled{ false };

		int numCloudRaySteps{ 128 };
		int numShadowRaySteps{ 16 };
		int numVolumetricLightRaySteps{ 64 };

		float volumetricLightScattering{ 0.35f };
		float lightAbsorption{ 0.0035f };
		glm::vec3 vlLightColor{ 0.6f, 0.6f, 0.4f };

		float cloudCoverage{ 0.f };
		float cloudSpeed{ 900.f };
		float cloudFluffyness{ 40.f };
		float cloudCurliness{ 0.1 };
		float cloudDensity{ 0.002f };
		
		float earthRadius{ 600000.f };
		float atmosphereInnerRadius{ 1000.f };
		float atmosphereOuterRadius{ 20000.f };

		bool bExpensive{ true };

		float downScale = 0.5f;

		glm::vec3 windDirection{ glm::normalize(glm::vec3(0.5f, 0.f, 1.f)) };
		glm::vec3 cloudBotColor{ (glm::vec3(65.f, 70.f, 80.f) * (1.5f / 255.f)) };
	}volumetricClouds;

	struct Sky {
		glm::vec3 topColor{ 0.f };
		glm::vec3 botColor{ 0.f };
		glm::vec3 cloudColor{ 1.f };
	}sky;

	struct Fog{
		bool enabled{ false };
		float sight{ 0.f };
	}fog;

	struct Shadows{
		bool enabled{ false };
		int resolution{ 4096 };
	}shadows;

	struct ContactShadows{
		bool enabled{ false };
	}contactShadows;

	struct SMAA{
		bool enabled{ false };
	}smaa;

	struct SSAA{
		bool enabled{ false };
	}ssaa;

	struct Dirlight {
		glm::vec3 ambient{ 0.f };
		glm::vec3 diffuse{ 0.f };
		glm::vec3 specular{ 0.f };
		glm::vec3 direction{ 0.f };
		float itensity{ 0.f };
	}dirlight;

	struct Vegetation
	{
		bool enabled{ false };
		int LODRanges[5]{ 0 };
	}vegetation;

	struct InstancedMesh {
		int LODRanges[4]{ 0 };
	}instancedMesh;

	void Load(const std::string& jsonPath);
};

//GPU data
struct RenderSettings
{
	int fogEnabled;
	float fogSight;
	int volumetricLightEnabled;
	int giEnabled;

	glm::vec3 fogColor;
	int pad3;

	glm::vec3 bloomThreshold;
	int bloomEnabled;

	int lightModel;
	int ssaoEnabled; 
	int pad11;
	int pad12;
};