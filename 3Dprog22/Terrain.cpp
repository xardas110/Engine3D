#include "Terrain.h"
#include "World.h"
#include "Debug.h"

int Terrain::gridSize = 8;

Terrain::Terrain()
{
}

Terrain::Terrain(const std::string& jsonConfigPath)
{
	LoadConfig(jsonConfigPath);
}

Terrain::~Terrain() {}

void Terrain::Init()
{		
	for (auto i = 0; i < gridSize; i++)
	{
		for (auto j = 0; j < gridSize; j++)
		{
			glm::vec2 location = glm::vec2((float)i / (float)gridSize, (float)j / (float)gridSize);
			glm::vec2 index = glm::vec2((float)i, (float)j);
			auto chunk = TerrainNode(&config, location, 0, index);
			AddChunk(chunk);
		}
	}
}

void Terrain::Update()
{
	for (auto& node : chunks)
		node.UpdateQuadtree();
}

float Terrain::GetHeightAt(float x, float z)
{
	float result = 0.f;

	glm::vec2 pos = glm::vec2(x, z);
	pos += config.scaleXZ * 0.5f;
	pos /= config.scaleXZ;

	glm::vec2 f = glm::vec2((int)pos.x, (int)pos.y);

	pos = pos - f;
	pos = pos * (float)heightMap.width;

	int x0 = pos.x;
	int x1 = x0 + 1;
	int z0 = pos.y;
	int z1 = z0 + 1;

	int hw = heightMap.width;

	if (hw * z0 + x1 >= heightmapBuffer.size()) return 0.f;
	if (hw * z1 + x0 >= heightmapBuffer.size()) return 0.f;

	float h0 = heightmapBuffer[hw * z0 + x0];
	float h1 = heightmapBuffer[hw * z0 + x1];
	float h2 = heightmapBuffer[hw * z1 + x0];
	float h3 = heightmapBuffer[hw * z1 + x1];

	float pU = pos.x - x0;
	float pV = pos.y - z0;

	float dU, dV;

	if (pU > pV)
	{
		dU = h1 - h0;
		dV = h3 - h1;
	}
	else
	{
		dU = h3 - h2;
		dV = h2 - h0;
	}

	result = h0 + (dU * pU) + (dV * pV);
	result *= config.scaleY;

	return result;
}

float Terrain::GetNormalSlopeAt(float x, float z)
{
	float result = 0.f;

	glm::vec2 pos = glm::vec2(x, z);
	pos += config.scaleXZ * 0.5f;
	pos /= config.scaleXZ;

	glm::vec2 f = glm::vec2((int)pos.x, (int)pos.y);

	pos = pos - f;
	pos = pos * (float)heightMap.width;

	int x0 = pos.x;
	int x1 = x0 + 1;
	int z0 = pos.y;
	int z1 = z0 + 1;

	int hw = heightMap.width;

	if (hw * z0 + x1 >= slopeBuffer.size()) return 0.f;
	if (hw * z1 + x0 >= slopeBuffer.size()) return 0.f;

	float h0 = slopeBuffer[hw * z0 + x0];
	float h1 = slopeBuffer[hw * z0 + x1];
	float h2 = slopeBuffer[hw * z1 + x0];
	float h3 = slopeBuffer[hw * z1 + x1];

	float pU = pos.x - x0;
	float pV = pos.y - z0;

	float dU, dV;

	if (pU > pV)
	{
		dU = h1 - h0;
		dV = h3 - h1;
	}
	else
	{
		dU = h3 - h2;
		dV = h2 - h0;
	}

	result = h0 + (dU * pU) + (dV * pV);

	return result;
}

glm::vec3 Terrain::GetNormalAt(float x, float z)
{
	glm::vec3 result{ 0.f };

	glm::vec2 pos = glm::vec2(x, z);
	pos += config.scaleXZ * 0.5f;
	pos /= config.scaleXZ;

	glm::vec2 f = glm::vec2((int)pos.x, (int)pos.y);

	pos = pos - f;
	pos = pos * (float)heightMap.width;

	int x0 = pos.x;
	int x1 = x0 + 1;
	int z0 = pos.y;
	int z1 = z0 + 1;

	int hw = heightMap.width;

	if (hw * z0 + x1 >= normalBuffer.size()) return glm::vec3();
	if (hw * z1 + x0 >= normalBuffer.size()) return glm::vec3();

	glm::vec3 h0 = normalBuffer[hw * z0 + x0];
	glm::vec3 h1 = normalBuffer[hw * z0 + x1];
	glm::vec3 h2 = normalBuffer[hw * z1 + x0];
	glm::vec3 h3 = normalBuffer[hw * z1 + x1];

	float pU = pos.x - x0;
	float pV = pos.y - z0;

	glm::vec3 dU, dV;

	if (pU > pV)
	{
		dU = h1 - h0;
		dV = h3 - h1;
	}
	else
	{
		dU = h3 - h2;
		dV = h2 - h0;
	}

	result = h0 + (dU * pU) + (dV * pV);
	std::swap(result.y, result.z);

	return result;
}

void Terrain::AddChunk(const TerrainNode& node)
{
	chunks.emplace_back(node);
}

void Terrain::GetNodes(std::vector<TerrainNode*>& outNodes)
{	
	for (auto& node : chunks)
		node.GetNodes(outNodes);
}

void Terrain::SetHeightmap(Texture& heightmap)
{
	this->heightMap = heightMap;
}

void Terrain::SetSplatmap(Texture& splatmap)
{
	this->splatMap = splatmap;
}

void Terrain::SetNormalmap(Texture& normalmap)
{
	this->normalMap = normalmap;
}

Texture& Terrain::GetHeightmapTexture()
{
	return heightMap;
}

Texture& Terrain::GetNormalmapTexture()
{
	return normalMap;
}

Texture& Terrain::GetSplatmapTexture()
{
	return splatMap;
}

void Terrain::SetHeightmapTexture(const Texture& texture)
{
	heightMap = texture;
}

void Terrain::SetNormalmapTexture(const Texture& texture)
{
	normalMap = texture;
}

void Terrain::SetSplatmapTexture(const Texture& texture)
{
	splatMap = texture;
}

MaterialTextured& Terrain::GetMaterial(int index)
{
	return materials[index];
}

void Terrain::SetMaterial(int index, const MaterialTextured& material)
{
	materials[index] = material;
}

void Terrain::LoadConfig(const std::string& jsonPath)
{
	config = TerrainConfig(jsonPath);
}
