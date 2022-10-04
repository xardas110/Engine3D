#pragma once
#include "Include/entt/entt.hpp"
#include "skybox.h"
#include "shader.h"
#include "Sun.h"
#include <vector>
#include "LensFlare.h"
#include "DirectionalLight.h"
#include "Skydome.h"
#include "VolumetricLighting.h"
#include "HDR.h"
#include "DeferredRendererConfig.h"
#include "PerlinNoise.h"
#include "ABuffer.h"

struct SkyboxBindings
{
	int MVP;
	int skybox;
};

struct SkyDomeBindings
{
	int model;
	int skydomeTexture;
	int botColor;
	int topColor;
	int cloudColor;
	int perlinTex;
};

struct SunBindings
{
	int MVP;
	int texture;
	int sunInnerTexture;
};

struct LensFlareBindings
{
	int pos2D;
	int scale2D;
	int lensTexture;
	int brightness;
};

class SkySystem
{
	friend class World;
	friend class DeferredRenderer;

	SkySystem();
	~SkySystem();

	void Init(class World* world, entt::registry& registry);

	void Update(class World* world, entt::registry& registry, float deltaTime);

	void Render(class World* world, entt::registry& registry, const DeferredRendererConfig& config, const PerlinNoise& perlinNoise, const ABuffer* aBuffer);

	void InitQuadMesh(class World* world);

	void InitSkybox(class World* world);

	void InitSkyDome(class World* world);

	void InitDirlight(class World* world);

	void InitSun(class World* world);

	void InitLensFlares(class World* world);

	void UpdateDirlight(class World* world);

	void UpdateLensFlares(class World* world, entt::registry& registry);

	void UpdateSun(class World* world, float deltatime);

	void RenderSkybox(class World* world, entt::registry& registry);

	void RenderSkyDome(class World* world, entt::registry& registry, const DeferredRendererConfig& config, const PerlinNoise& perlinNoise);

	void RenderSun(class World* world, entt::registry& registry, const ABuffer* aBuffer);

	void RenderLensFlares(class World* world, entt::registry& registry);

	void InitShaders(class World* world);

	void InitShaderBindings();

	void InitSkyboxShaderBindings();
	void InitSkyDomeShaderBindings();
	void InitSunShaderBindings();
	void InitLensShaderBindings();

	//Skybox* skybox;
	Skydome	skyDome;
	Sun     sun;
	DirLight dirlight;
	LensFlares lensFlares;
	
	Shader skyboxShader;
	Shader sunShader;
	Shader lensShader;

	SkyboxBindings skyboxBindings;
	SkyDomeBindings skyDomeBindings;
	SunBindings sunBindings;
	LensFlareBindings lensBindings;
	
	Mesh quadMesh;

	float deltaTime{ 0.f };
public:
	const DirLight& GetDirectionalLight();
	void SetLightDirection(const glm::vec3& newDir);
	void SetLightAmbient(const glm::vec3& newAmbient);
	void SetLightDiffuse(const glm::vec3& newDiffuse);
	void SetLightSpecular(const glm::vec3& newSpecular);
	void SetLightItensity(const float newVal);

	const glm::vec3&	GetLightDirection() const;
	const glm::vec3&	GetAmbientLight() const;
	const glm::vec3&	GetDiffuseLight() const;
	const glm::vec3&	GetSpecularLight() const;
	const float			GetItensity() const;
};

