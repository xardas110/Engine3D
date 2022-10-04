#pragma once
#include "SSBO.h"
#include "Include/entt/entt.hpp"
#include "GlobalTypeDefs.h"
#include "shader.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "ThreadPoolx.h"
#include "ABuffer.h"

struct ShadowPassBindingsVeg
{
	int instanceOffset;
	int opacitymap;
	int numOpacitymaps;
};

struct MainPassBindingsVeg
{
	int color;
	int shininess;
	int sms;
	int cloudShadowVL;
	int instanceOffset;
	int depthmap;

	MaterialInstancedBindings mib;
};
/// <summary>
/// Less fancy shaders for we are many.
/// Max 1-2 cascades
/// Less texture support
/// </summary>
class VegetationSystem
{
	friend class World;
	friend class DeferredRenderer;

	void Init(class World* world, entt::registry& registry);

	void InitSSBO();
	void InitShaders();
	void InitShaderBindings();

	void Update(class World* world, entt::registry& registry);

	void UpdateBuffer(class World* world, entt::registry& registry);

	void MainPass(class World* world, entt::registry& registry, const ABuffer* aBuffer, const Texture& shadowMaps, const Texture& cloudShadowVL, const Texture& depthmap);

	void ShadowPass(class World* world, entt::registry& registry);

	void BindMaterials(MaterialInstanced& materialInstanced);
private:
	SSBO ssbo;

	Shader mainPass;
	Shader shadowPass;
	Shader sgShader;

	MainPassBindingsVeg mpb;
	ShadowPassBindingsVeg spb;
	ShadowPassBindingsVeg sgb;
};

