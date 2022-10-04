#version 460 core
layout (location = 0) out vec4 outPos;

struct Material {
	sampler2D diffusemap;
	sampler2D specularmap;
	sampler2D normalmap;  
	sampler2D AOmap; 
	sampler2D heigthMap;

	int numNormalmaps;
	int numSpecularmaps;
	int numDiffusemaps;
	int numAOmaps;
	int numHeightMaps;

	vec3 color; 
	float shininess;
};

uniform Material material;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

float GetAlphaValue();
float GetAO();
float GetSpecularColor();
float GetHeight();

vec3 GetDiffuseColor();
vec3 GetNormal();

void main()
{
	outPos = vec4(1.f, 0.f, 0.f, 1.f);
}

vec3 GetDiffuseColor() // returns either a texture diffuse color or vec3(1)
{
	if (material.numDiffusemaps != 0)
	{
		return vec3(texture(material.diffusemap, TexCoords));
	}
	return vec3(1.f, 1.f, 1.f);
}
float GetSpecularColor()// returns either a texture specular color or vec3(1)
{
	if (material.numSpecularmaps != 0)
	{
		return texture(material.specularmap, TexCoords).r;
	}
	return 1.f;
}
vec3 GetNormal()
{
	if (material.numNormalmaps != 0)
	{
		vec3 normalMap = texture(material.normalmap, TexCoords).rgb;
		vec3 norm = normalMap * 2.f - 1.f;
		return normalize(TBN * norm);
	}
	else if (material.numHeightMaps != 0)		
	{
		vec3 normalMap = texture(material.heigthMap, TexCoords).rgb;
		vec3 norm = normalMap * 2.f - 1.f;
		return normalize(TBN * norm);
	}

	return normalize(Normal);
}

float GetAO()
{
	if (material.numAOmaps != 0)
	{
		return texture(material.AOmap, TexCoords).r;
	}

	return 1.f;
}

float GetHeight()
{
	if (material.numHeightMaps != 0)
	{
		return texture(material.heigthMap, TexCoords).r;
	}

	return 1.f;
}