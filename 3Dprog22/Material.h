#pragma once
#include "Include/glm/glm.hpp"
#include <string>
#include "Texture.h"

struct Material
{
	glm::vec3 ambient{1.f};
	glm::vec3 diffuse{1.f};
	glm::vec3 specular{1.f};

	float shine{ 64.f };
};

//Getting uniform locations from shaders are expensive, this data struct will cahce the binding locations
//A map can be usead instead of this, but thats unecessary performance loss. This approach is abit more work, but not by much

struct TexNum
{
	int texLoc;
	int numTex;
};

struct MaterialTexturedBindings
{
	TexNum diffusemap;
	TexNum specularmap;
	TexNum ambientmap;
	TexNum emissivemap;
	TexNum heightmap;
	TexNum normalmap;
	TexNum shininessmap;
	TexNum displacementmap;
	TexNum lightmap;
	TexNum reflectionmap;
	TexNum opacitymap;
	TexNum metallicmap;
	TexNum roughnessmap;

	int color;
	int shininess;
};

struct MaterialTextured
{
	std::string name;

	Texture diffusemap;
	Texture specularmap;
	Texture ambientmap;
	Texture emissivemap;
	Texture heightmap;
	Texture normalmap;
	Texture shininessmap;
	Texture displacementmap;
	Texture lightmap;
	Texture reflectionmap;
	Texture opacitymap;
	Texture metallicmap;
	Texture roughnessmap;
	
	glm::vec3 color{1.f, 1.f, 1.f};
	float alpha;
	float emission;
	float shininess{64.f};

	float displaceScale;
	float horizontalScale;

	bool bCastShadow{ true };
	bool bRecieveShadow{ true };

	[[nodiscard]] bool HasDiffusemap() const
	{
		return diffusemap.IsValid();
	}

	[[nodiscard]] bool HasSpecularmap() const
	{
		return specularmap.IsValid();
	}

	[[nodiscard]] bool HasAOmap() const
	{
		return ambientmap.IsValid();
	}

	[[nodiscard]] bool HasEmissivemap() const
	{
		return emissivemap.IsValid();
	}

	[[nodiscard]] bool HasHeightmap() const
	{
		return heightmap.IsValid();
	}

	[[nodiscard]] bool HasNormalmap() const
	{
		return normalmap.IsValid();
	}

	[[nodiscard]] bool HasShininessmap() const
	{
		return shininessmap.IsValid();
	}
	
	[[nodiscard]] bool HasDisplacementmap() const
	{
		return displacementmap.IsValid();
	}

	[[nodiscard]] bool HasLightmap() const
	{
		return lightmap.IsValid();
	}

	[[nodiscard]] bool HasReflectionmap() const
	{
		return reflectionmap.IsValid();
	}

	[[nodiscard]] bool HasOpacitymap() const
	{
		return opacitymap.IsValid();
	}

	[[nodiscard]] bool HasMetallicmap() const
	{
		return metallicmap.IsValid();
	}

	[[nodiscard]] bool HasRoughnessmap() const
	{
		return roughnessmap.IsValid();
	}
};

struct MaterialInstancedBindings
{
	TexNum diffusemap;
	TexNum specularmap;
	TexNum ambientmap;
	TexNum normalmap;
	TexNum opacitymap;
	TexNum metallicmap;
	TexNum roughnessmap;
	TexNum heightmap;

	int color;
	int shininess;
};

struct MaterialInstanced
{
	Texture diffusemap;
	Texture specularmap;
	Texture ambientmap;
	Texture normalmap;
	Texture opacitymap;
	Texture metallicmap;
	Texture roughnessmap;

	glm::vec3 color{ 1.f, 1.f, 1.f };
	float shininess{ 64.f };

	[[nodiscard]] bool HasDiffusemap() const
	{
		return diffusemap.IsValid();
	}

	[[nodiscard]] bool HasSpecularmap() const
	{
		return specularmap.IsValid();
	}

	[[nodiscard]] bool HasAOmap() const
	{
		return ambientmap.IsValid();
	}

	[[nodiscard]] bool HasNormalmap() const
	{
		return normalmap.IsValid();
	}

	[[nodiscard]] bool HasOpacitymap() const
	{
		return opacitymap.IsValid();
	}
	[[nodiscard]] bool HasMetallicmap() const
	{
		return metallicmap.IsValid();
	}
	[[nodiscard]] bool HasRoughnessmap() const
	{
		return roughnessmap.IsValid();
	}

	[[nodiscard]] bool HasAnyMaterial() const
	{
		return HasDiffusemap() || HasSpecularmap() || HasAOmap() || HasNormalmap() || HasOpacitymap() || HasMetallicmap() || HasRoughnessmap();;
	}
};