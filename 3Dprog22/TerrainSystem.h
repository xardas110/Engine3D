#pragma once
#include "Include/entt/entt.hpp"
#include "Mesh.h"
#include <vector>
#include "DirectionalLight.h"
#include "FrameBuffer.h"

/*Might be temp, for now saving all the bindings for cache efficiency*/
/*glGetuniform is very slow*/

struct TerrainShaderMaterialBindings
{
	int diffusemap;
	int normalmap;
	int heightmap;
	int aomap;
	int specularmap;
	int roughnessmap;
	int heightscale;
	int horizontalScale;
};

struct TerrainShaderBindings
{
	int scaleY;
	bool bEnableTess;
	bool bShowHeightLines;
	int tessellationFactor;
	int tessellationSlope;
	int tessellationShift;
	int normalStrength;
	int TBNRange;
	int heightmap;
	int splatmap;
	int normalMap;
	int localMatrix;
	int location;
	int index;
	int lod; // lod for node
	int gap;

	int worldMatrix;
	int smCascades;

	int lodMorphArea[8];

	TerrainShaderMaterialBindings materialBindings[3];
};

struct NormalShaderBindings
{
	int heightmap;
	int N;
	int strength;
};

struct SplatShaderBindings
{
	int normalmap;
	int N;
};

struct ShadowGridBindings
{
	int localMatrix;
	int worldMatrix;
	int heightmap;
	int scaleY;
};

class TerrainSystem
{
	friend class World;
	friend class DeferredRenderer;

	void Init(class World* world, entt::registry& registry);

	void InitMesh(class World* world);
	void InitShaders();
	
	void InitShaderBindings();

	void InitComputeShaders(entt::registry& registry);

	void ShadowPass(class World* world, entt::registry& registry);
	void Update(class World* world, entt::registry& registry);
	void Render(class World* world, entt::registry& registry);

	void InitNormalBuffers(class World* world, entt::registry& registry);

	/*Getuniforms are very expensive, so they need to be cached ->*/
	TerrainShaderBindings tBindings;
	TerrainShaderBindings spBindings;
	NormalShaderBindings nBindings;
	SplatShaderBindings sBindings;
	ShadowGridBindings sgb;

	Shader normalComputeShader;
	Shader splatComputeShader;
	Shader terrainShader;
	Shader sgShader;
	Shader spShader;
	Mesh meshPatch;

public:
	entt::entity CreateTerrain(class World* world, entt::registry& registry, const std::string& configJsonPath);
	entt::entity CreateTerrain(class World* world, entt::registry& registry, Texture& heightmap, const std::string& configJsonPath);
};

