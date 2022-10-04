#pragma once
#include "TerrainConfig.h"
#include "Camera.h"
#include <vector>
#include "TerrainNode.h"
#include "Json.h"

//Based on https://github.com/fynnfluegge/oreon-engine tutorial series.
//Java tutorial series, almost all of the code is rewritten and has different logic though.

#define MAX_MATERIALS 4

class Terrain
{
	friend class TerrainSystem;
	friend class World;

	void AddChunk(const TerrainNode& node);	
public:
	Terrain();
	Terrain(const std::string& jsonConfigPath);
	~Terrain();

	TerrainConfig config;

	std::vector<TerrainNode> chunks;

	void Init();

	void Update();

	float GetHeightAt(float x, float z);

	float GetNormalSlopeAt(float x, float z);

	glm::vec3 GetNormalAt(float x, float z);

	void GetNodes(std::vector<TerrainNode*>& outNodes);

	void SetHeightmap(Texture& heightmap);
	/*Splatmap will automatically get generated if not specified*/
	void SetSplatmap(Texture& splatmap);
	/*Normalmap will be automatically genereated if not specified*/
	void SetNormalmap(Texture& normalmap);

	Texture& GetHeightmapTexture();
	Texture& GetNormalmapTexture();
	Texture& GetSplatmapTexture();

	void SetHeightmapTexture(const Texture& texture);
	void SetNormalmapTexture(const Texture& texture);
	void SetSplatmapTexture(const Texture& texture);

	MaterialTextured& GetMaterial(int index);
	void SetMaterial(int index, const MaterialTextured& material);

	void LoadConfig(const std::string& jsonPath);

	static int gridSize;

	std::vector<float> heightmapBuffer;
	std::vector<float> slopeBuffer;
	std::vector<glm::vec3> normalBuffer;
private:


	Texture heightMap;
	Texture normalMap;
	Texture splatMap;

	MaterialTextured materials[MAX_MATERIALS];
	int numMaterials{ 0 };
};

