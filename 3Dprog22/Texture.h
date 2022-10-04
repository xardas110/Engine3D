#pragma once
#include <array>
#include <cassert>
#include <string>
struct Texture
{
	enum Types // order has to be like because I follow the assimp order:
	{
		None,
		Diffuse,
		Specular,
		Ambient,
		Emissive,
		Height,
		Normals,
		Shininess,
		Opacity,
		Displacement,
		LightMap,
		Reflection,
		ShadowMap,
		Splat,
		Roughness,
		Metallic,
		Size
	} type = Diffuse;

	static std::string GetTypeString(const Types type)
	{
		switch (type)
		{
			case None:			return std::string("None");
			case Diffuse:		return std::string("Diffuse");
			case Specular:		return std::string("Specular");
			case Ambient:		return std::string("Ambient");
			case Emissive:		return std::string("Emissive");
			case Height:		return std::string("Height");
			case Normals:		return std::string("Normals");
			case Shininess:		return std::string("Shininess");
			case Opacity:		return std::string("Opacity");
			case Displacement:	return std::string("Displacement");
			case LightMap:		return std::string("LightMap");
			case Reflection:	return std::string("Reflection");
			case ShadowMap:		return std::string("ShadowMap");
			default: assert(false && "Texture type does not exist"); return std::string("FAILED");
		}
	}
	
	unsigned textureID{0};
	unsigned width{0}, height{0};
	unsigned dimensions{0};
	/*Internal format*/
	unsigned format; 
	unsigned formatType;
	unsigned dataType;

	unsigned filter;
	unsigned wrap{ 0x2901 };//repeat

	std::string path;

	[[nodiscard]]
	bool IsValid() const
	{
		//TODO: enter right statement
		return textureID != 0;
	}
};

struct CubemapDescriptor
{
	std::string tag;
	std::array<std::string, 6> texturePaths;
private:
	friend class TextureManager;
	Texture texture;
};