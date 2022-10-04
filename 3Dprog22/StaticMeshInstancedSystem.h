#pragma once
#include "SSBO.h"
#include "Include/entt/entt.hpp"
#include "GlobalTypeDefs.h"
#include "shader.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "ThreadPoolx.h"
#include "ABuffer.h"
#include "Texture.h"
#include "GBuffer.h"
#include "Include/glm/glm.hpp"
#include "Mesh.h"

struct ShadowPassBindingsSMI
{
	int instanceOffset;
	int opacitymap;
	int numOpacitymaps;
};

struct MainPassBindings
{
	int color;
	int shininess;

	int gBufferDepth;

	int sms;
	int cloudShadowVL;
	int depthmap;

	int instanceOffset;

	int meshSettingsbHasTransparency;

	MaterialInstancedBindings mib;
};

struct SMIAlphaMesh
{
	Mesh mesh;
	std::vector<glm::mat4> transforms;
};

/// <summary>
/// Less fancy shaders for we are many.
/// Max 1-2 cascades
/// Less texture support
/// </summary>
class StaticMeshInstancedSystem
{
	friend class World;
	friend class DeferredRenderer;

	void Init(class World* world, entt::registry& registry);
	
	void InitSSBO();
	void InitShaders();
	void InitShaderBindings();

	void Update(class World* world, entt::registry& registry);

	void UpdateBuffer(class World* world, entt::registry& registry);

	void MainPass(class World* world, entt::registry& registry, const ABuffer* aBuffer, const Texture& shadowMaps, const Texture& cloudShadowVL);

	void AlphaPass(class World* world, const Texture& shadowMaps, const ABuffer* aBuffer);

	void ShadowPass(class World* world, entt::registry& registry);

	void BindMaterial(const MaterialTextured& material);
private:
	SSBO ssbo;

	Shader mainPass;
	Shader shadowPass;

	MainPassBindings mpb;
	ShadowPassBindingsSMI spb;

	std::vector<SMIAlphaMesh> smiAlphaMeshes;
};

