#include "DeferredRendererConfig.h"
#include "Json.h"
#include "Debug.h"
#include <iostream>
#include <Windows.h>

void DeferredRendererConfig::Load(const std::string& jsonPath)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 14);

	std::cout << "Loading render settings..." << std::endl;
	std::cout << "----------------------" << std::endl;

	Json json = Json::Load(jsonPath);

	std::cout << "Dof:" << std::endl;

	auto& jDof = json["dof"];
	dof.enabled = jDof["enabled"];
	dof.focusScale = jDof["focusScale"];
	dof.uFar = jDof["uFar"];
	dof.maxBlurSize = jDof["maxBlurSize"];
	dof.radScale = jDof["radScale"];

	std::cout << "enabled: " << dof.enabled << std::endl;
	std::cout << "focusScale: " << dof.focusScale << std::endl;
	std::cout << "uFar: " << dof.uFar << std::endl;
	std::cout << "maxBlurSize: " << dof.maxBlurSize << std::endl;
	std::cout << "radScale: " << dof.radScale << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "Bloom:" << std::endl;
	auto& jBloom = json["bloom"];
	bloom.enabled = jBloom["enabled"];
	for (size_t i = 0; i < jBloom["threshold"].Size(); i++)
	{
		bloom.threshold[i] = jBloom["threshold"][i];
	}
	std::cout << "enabled: " << bloom.enabled << std::endl;
	VEC3(bloom.threshold);
	std::cout << "----------------------" << std::endl;

	std::cout << "SSAO: " << std::endl;

	auto& jSSAO = json["SSAO"];
	ssao.enabled = jSSAO["enabled"];
	ssao.radius = jSSAO["radius"];
	ssao.bias = jSSAO["bias"];

	std::cout << "enabled: " << ssao.enabled << std::endl;
	std::cout << "radius: " << ssao.radius << std::endl;
	std::cout << "bias: " << ssao.bias << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "Global Illumination: " << std::endl;
	auto& jGI = json["globalIllumination"];
	gi.enabled = jGI["enabled"];

	std::cout << "enabled: " << gi.enabled << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "VolumetricLight: " << std::endl;
	auto& jvl = json["volumetricLight"];
	volumetricLight.enabled = jvl["enabled"];
	volumetricLight.numRays = jvl["numRays"];
	volumetricLight.scattering = jvl["scattering"];
	volumetricLight.sunColor[0] = jvl["sunColor"][0];
	volumetricLight.sunColor[1] = jvl["sunColor"][1];
	volumetricLight.sunColor[2] = jvl["sunColor"][2];

	std::cout << "enabled: " << volumetricLight.enabled << std::endl;
	std::cout << "numRays: " << volumetricLight.numRays << std::endl;
	std::cout << "scattering: " << volumetricLight.scattering << std::endl;
	VEC3(volumetricLight.sunColor);
	std::cout << "----------------------" << std::endl;

	std::cout << "VolumetricClouds: " << std::endl;
	auto& jvc = json["volumetricClouds"];
	volumetricClouds.enabled = jvc["enabled"];
	volumetricClouds.cloudCoverage = jvc["coverage"];
	volumetricClouds.cloudSpeed = jvc["speed"];
	volumetricClouds.cloudFluffyness = jvc["crispiness"];
	volumetricClouds.cloudCurliness = jvc["curliness"];
	volumetricClouds.cloudDensity = jvc["density"];

	std::cout << "enabled: " << volumetricClouds.enabled << std::endl;
	std::cout << "coverage: " << volumetricClouds.cloudCoverage << std::endl;
	std::cout << "speed: " << volumetricClouds.cloudSpeed << std::endl;
	std::cout << "crispiness: " << volumetricClouds.cloudFluffyness << std::endl;
	std::cout << "density: " << volumetricClouds.cloudDensity << std::endl;
	VEC3(volumetricClouds.cloudBotColor);
	std::cout << "----------------------" << std::endl;

	std::cout << "Sky: " << std::endl;
	auto& jSky = json["sky"];
	sky.botColor[0] = jSky["botColor"][0];
	sky.botColor[1] = jSky["botColor"][1];
	sky.botColor[2] = jSky["botColor"][2];
	sky.topColor[0] = jSky["topColor"][0];
	sky.topColor[1] = jSky["topColor"][1];
	sky.topColor[2] = jSky["topColor"][2];
	VEC3(sky.botColor);
	VEC3(sky.topColor);
	std::cout << "----------------------" << std::endl;

	std::cout << "Fog: " << std::endl;
	auto& jFog = json["fog"];
	fog.enabled = jFog["enabled"];
	fog.sight = jFog["sight"];
	std::cout << "enabled: " << fog.enabled << std::endl;
	std::cout << "sight: " << fog.sight << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "Shadows: " << std::endl;
	auto& jShadows = json["shadows"];
	shadows.enabled = jShadows["enabled"];
	shadows.resolution = jShadows["resolution"];
	std::cout << "enabled: " << shadows.enabled << std::endl;
	std::cout << "resolution: " << shadows.resolution << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "ContactShadows: " << std::endl;
	auto& jContactShadows = json["contactShadows"];
	contactShadows.enabled = jContactShadows["enabled"];
	std::cout << "enabled: " << contactShadows.enabled << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "SMAA: " << std::endl;
	auto& jSMAA = json["SMAA"];
	smaa.enabled = jSMAA["enabled"];
	std::cout << "enabled: " << smaa.enabled << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "SSAA: " << std::endl;
	auto& jSSAA = json["SSAA"];
	ssaa.enabled = jSSAA["enabled"];
	std::cout << "enabled: " << ssaa.enabled << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "Dirlight: " << std::endl;

	auto& jDirlight = json["dirlight"];
	dirlight.ambient[0] = jDirlight["ambient"][0];
	dirlight.ambient[1] = jDirlight["ambient"][1];
	dirlight.ambient[2] = jDirlight["ambient"][2];
	dirlight.diffuse[0] = jDirlight["diffuse"][0];
	dirlight.diffuse[1] = jDirlight["diffuse"][1];
	dirlight.diffuse[2] = jDirlight["diffuse"][2];
	dirlight.specular[0] = jDirlight["specular"][0];
	dirlight.specular[1] = jDirlight["specular"][1];
	dirlight.specular[2] = jDirlight["specular"][2];
	dirlight.direction[0] = jDirlight["direction"][0];
	dirlight.direction[1] = jDirlight["direction"][1];
	dirlight.direction[2] = jDirlight["direction"][2];
	dirlight.itensity = jDirlight["itensity"];
	dirlight.direction = glm::normalize(dirlight.direction);

	VEC3(dirlight.ambient);
	VEC3(dirlight.diffuse);
	VEC3(dirlight.specular);
	VEC3(dirlight.direction);
	std::cout << "----------------------" << std::endl;

	std::cout << "Vegetation: " << std::endl;
	auto& jVeg = json["vegetation"];
	vegetation.enabled = jVeg["enabled"];
	vegetation.LODRanges[0] = jVeg["LODRanges"][0];
	vegetation.LODRanges[1] = jVeg["LODRanges"][1];
	vegetation.LODRanges[2] = jVeg["LODRanges"][2];
	vegetation.LODRanges[3] = jVeg["LODRanges"][3];
	vegetation.LODRanges[4] = jVeg["LODRanges"][4];
	std::cout << "enabled: " << vegetation.enabled << std::endl;
	std::cout << "LODRanges[0]: " << vegetation.LODRanges[0] << std::endl;
	std::cout << "LODRanges[1]: " << vegetation.LODRanges[1] << std::endl;
	std::cout << "LODRanges[2]: " << vegetation.LODRanges[2] << std::endl;
	std::cout << "LODRanges[3]: " << vegetation.LODRanges[3] << std::endl;
	std::cout << "LODRanges[4]: " << vegetation.LODRanges[4] << std::endl;
	std::cout << "----------------------" << std::endl;

	std::cout << "InstancedMesh: " << std::endl;
	auto& jIM = json["instancedMesh"];
	instancedMesh.LODRanges[0] = jIM["LODRanges"][0];
	instancedMesh.LODRanges[1] = jIM["LODRanges"][1];
	instancedMesh.LODRanges[2] = jIM["LODRanges"][2];
	instancedMesh.LODRanges[3] = jIM["LODRanges"][3];
	std::cout << "LODRanges[0]: " << instancedMesh.LODRanges[0] << std::endl;
	std::cout << "LODRanges[1]: " << instancedMesh.LODRanges[1] << std::endl;
	std::cout << "LODRanges[2]: " << instancedMesh.LODRanges[2] << std::endl;
	std::cout << "LODRanges[3]: " << instancedMesh.LODRanges[3] << std::endl;
	std::cout << "----------------------" << std::endl << std::endl;

	SetConsoleTextAttribute(hConsole, 15);
}
