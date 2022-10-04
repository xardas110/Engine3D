#pragma once
#include "shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Include/entt/entt.hpp"
#include "SkeletalMesh.h"
#include "AlphaMesh.h"
#include "DirectionalLight.h"

struct SMShadowPassBindings
{
	int model;
	int gBones[100];
};

struct SMMainPassShaderBindings
{
	int model;
	int MVP;
	int smCascades;
	int viewPos;
	int camViewMat;
	int scale;

	int renderSettingsbHasTransparency;

	int gBones[100];

	DirLightBindings dirlight;
	MaterialTexturedBindings material;
};

class SkeletalMeshSystem
{
	friend class World;
	friend class DeferredRenderer;
	friend class AlphaPassSystem;

	void Init(class World* world, entt::registry& registry);

	void Update(class World* world, entt::registry& registry, float deltaTime);

	void UpdateAnimiations(SkeletalMesh& skeletalMesh, float deltaTime);

	void ShadowPass(class World* world, entt::registry& registry);
	void MainPass(class World* world, entt::registry& registry);
	void AlphaPass(class World* world, entt::registry& registry, const AlphaMesh& alphaMesh);

	void DrawMesh(class RenderEngine* renderEngine, const Mesh& mesh);

	void InitShaders(class World* world);
	void InitShaderBindings();

	void BindSkeletalBones(class World* world, class RenderEngine* renderEngine, SkeletalMesh& skeletalMesh);
	void BindSkeletalBonesShadowPass(class World* world, class RenderEngine* renderEngine, SkeletalMesh& skeletalMesh);

	void AddAlphaMesh(const Mesh& mesh, const glm::mat4& meshModel, const glm::vec3& camPos, entt::entity entity);

	Shader mainPassShader;
	Shader shadowPassShader;

	SMMainPassShaderBindings msb;
	SMShadowPassBindings spb;
};
