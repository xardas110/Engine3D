#include "TerrainConfig.h"
#include "Terrain.h"

TerrainConfig::TerrainConfig()
{
	scaleXZ = 3000.f;
	scaleY = 600.f;

	SetLodRange(0, 1750);
	SetLodRange(1, 1152);
	SetLodRange(2, 768);
	SetLodRange(3, 512);
	SetLodRange(4, 256);
	SetLodRange(5, 128);
	SetLodRange(6, 0);
	SetLodRange(7, 0);

	tessellationFactor = 1200;
	tessellationSlope = 1.8f;
	tessellationShift = 0.1f;

	strength = 60.f;
	N = 1024;
}

TerrainConfig::TerrainConfig(const Json& config)
{
	CHECKJSON(config, "scaleY", Json::Type::Float);
	CHECKJSON(config, "scaleXZ", Json::Type::Float);
	CHECKJSON(config, "tessellationFactor", Json::Type::Int);
	CHECKJSON(config, "tessellationSlope", Json::Type::Float);
	CHECKJSON(config, "tessellationShift", Json::Type::Float);
	CHECKJSON(config, "normalStrength", Json::Type::Float);
	CHECKJSON(config, "tessellationRange", Json::Type::Int);
	CHECKJSON(config, "LOD_Range", Json::Type::Array);
	CHECKJSON(config, "heightmapPath", Json::Type::String);
	CHECKJSON(config, "materials", Json::Type::Array);
	CHECKJSON(config, "bEnableTess", Json::Type::Bool);
	
	printf("Loading config files for terrain\n");

	scaleY = config["scaleY"];
	scaleXZ = config["scaleXZ"];
	bEnableTess = config["bEnableTess"];
	tessellationFactor = config["tessellationFactor"];
	tessellationSlope = config["tessellationSlope"];
	tessellationShift = config["tessellationShift"];
	strength = config["normalStrength"];
	
	tessellationRange = config["tessellationRange"];

	printf("scaleY %f \n", scaleY);
	printf("scaleXZ %f \n", scaleXZ);

	printf("tessellationFactor %i \n", tessellationFactor);
	printf("tessellationFactor %f \n", tessellationSlope);
	printf("tessellationShift %f \n", tessellationShift);
	printf("strength %f \n", strength);

	printf("tbn range %i \n", tessellationRange);
	
	printf("\nloading lod ranges\n");
	for (auto i = 0; i < config["LOD_Range"].Size(); i++)
	{
		SetLodRange(i, config["LOD_Range"][i]);
		printf("Lod level: %i, Lod range: %i \n", i, lod_range[i]);
	}
	
	heightmapPath = config["heightmapPath"];

	const auto materialsSize = config["materials"].Size();

	terrainMaterialConfig = new TerrainMaterialConfig[materialsSize];

	printf("\nLoading materials...\n");
	for (size_t i = 0; i < materialsSize; i++)
	{
		printf("\nMaterial nr. %i\n", i);

		const Json& materialConfig = config["materials"][i];
		auto& material = terrainMaterialConfig[i];
		material.diffusePath = materialConfig["diffusePath"];
		material.normalPath = materialConfig["normalPath"];
		material.displacementPath = materialConfig["displacementPath"];
		material.aoPath = materialConfig["aoPath"];
		material.specularPath = materialConfig["specularPath"];
		material.roughPath = materialConfig["roughnessPath"];

		material.heightScaling = materialConfig["heightScaling"];
		material.horizontalScaling = materialConfig["horizontalScaling"];

		printf("Material height scaling: %f\n", material.heightScaling);
		printf("Material horizontal scaling: %f\n", material.horizontalScaling);

		printf("%s \n", material.diffusePath.c_str());
		printf("%s \n", material.normalPath.c_str());
		printf("%s \n", material.displacementPath.c_str());
		printf("%s \n", material.aoPath.c_str());
		printf("%s \n", material.specularPath.c_str());
		printf("%s \n", material.roughPath.c_str());
	}	
}

TerrainConfig::TerrainConfig(const std::string& jsonPath)
{
	Json json = Json::Load(jsonPath);
	json.Print();
	*this = TerrainConfig(json);
}

TerrainConfig::~TerrainConfig()
{
	//delete terrainMaterialConfig;
}

void TerrainConfig::SetLodRange(int index, int lod_range)
{
	this->lod_range[index] = lod_range;
	this->lod_morphin_area[index] = lod_range - UpdateMorphingArea(index + 1);
}

int TerrainConfig::UpdateMorphingArea(int lod)
{
	return (int)((scaleXZ / Terrain::gridSize) / (pow(2, lod)));
}
