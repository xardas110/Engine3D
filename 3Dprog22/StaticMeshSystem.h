#pragma once
#include "Include/entt/entt.hpp"
#include "shader.h"
#include "DirectionalLight.h"
#include "Mesh.h"
#include "Material.h"
#include "AlphaMesh.h"
#include "VoxelConeTracing.h"

struct ShadowPassBindings
{
	int model;
};

struct MainPassShaderBindings
{
	int model;

	int shadowMaps;
	int settingsbTransparency;

	MaterialTexturedBindings material;
};

class StaticMeshSystem
{
	friend class World;
	friend class DeferredRenderer;
	friend class AlphaPassSystem;

	void Init(class World* world, entt::registry& registry);
	void ShadowPass(class World* world, entt::registry& registry);

	void Voxelize(class World* world, entt::registry& registry, const VoxelStructure* voxelStructure);

	void MainPass(class World* world, entt::registry& registry);
	void AlphaPass(class World* world, entt::registry& registry, const AlphaMesh& alphaMesh);

	void DrawMesh(class RenderEngine* renderEngine, const Mesh& mesh);

	void InitShaders(class World* world);
	void InitShaderBindings();

	/*To multithread file loading at program startup*/
	void LoadPaths(class World* world, const std::string& jsonPath);

	void BindDirlight(class World* world);

	Shader mainPassShader;
	Shader shadowPassShader;
	Shader voxelPassShader;

	MainPassShaderBindings msb;
	ShadowPassBindings spb;

public:
	void AddAlphaMesh(const Mesh& mesh, const glm::mat4& meshModel, const glm::vec3& camPos);
};

