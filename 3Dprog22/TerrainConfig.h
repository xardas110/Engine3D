#pragma once
#include "Texture.h"
#include "Material.h"
#include "Json.h"

struct TerrainMaterialConfig
{
	std::string diffusePath;
	std::string normalPath;
	std::string displacementPath;
	std::string aoPath;
	std::string specularPath;
	std::string roughPath;
	float heightScaling;
	float horizontalScaling;
};

struct TerrainConfig
{
	float scaleY;
	float scaleXZ;

	int tessellationFactor;
	float tessellationSlope;
	float tessellationShift;

	float strength;
	int N;

	bool bEnableTess;
	bool bShowHeightLines = false;

	int lod_range[8];
	int lod_morphin_area[8];

	int tessellationRange = 200;

	std::string heightmapPath = "../3Dprog22/Assets/Maps/HeightFields/hm0.bmp";

	TerrainMaterialConfig* terrainMaterialConfig;

	TerrainConfig();

	TerrainConfig(const Json& config);
	TerrainConfig(const std::string& jsonPath);

	~TerrainConfig();

	void SetLodRange(int index, int lod_range);
	int UpdateMorphingArea(int lod);
};

