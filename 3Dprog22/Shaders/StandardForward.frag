#version 460 core

layout(early_fragment_tests) in;

layout(location = 0) out uvec4 b0Out;

#include "Common/MeshSettings.frag"
#include "Common/DirectionalLight.frag"
#include "DistanceFog/DistanceFog.frag"
#include "Common/ABuffer.frag"
#include "Common/Camera.frag"
#include "Common/Rendersettings.frag"
#include "Common/ShaderModel.frag"

 #define MAX_CASCADES 4

struct Material {
	sampler2D diffusemap;
	sampler2D specularmap;
	sampler2D normalmap;
	sampler2D emissivemap;
	sampler2D opacitymap; 
	sampler2D heightmap; 
	sampler2D AOmap; 
	sampler2D metallicmap;
	sampler2D roughnessmap;


	int numNormalmaps;
	int numDiffusemaps;
	int numSpecularmaps;
	int numEmissivemaps;
	int numOpacitymaps;
	int numHeightmaps;
	int numAOmaps;
	int numMetallicmaps;
	int numRoughnessmaps;

	vec3 color; 
	float shininess;
};

uniform Material material;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

float GetAO();
float GetAlphaValue();
vec3 GetDiffuseColor();
vec3 GetSpecularColor();
vec3 GetNormal();
float GetMetallic();
float GetRoughness();

float GetHeight();

void main()
{
	float alpha = GetAlphaValue();
	vec3 norm = GetNormal();

	if (meshSettings.bHasTransparency == 1)
	{
		vec3 albedo = GetDiffuseColor();//pow(GetDiffuseColor(), vec3(2.2f));

		float shadowFactor = CalcDirLightShadow(dirlight, FragPos, norm, view);
		vec4 shadedColor = SM_BRDF(dirlight, shadowFactor, viewPos, norm, FragPos, albedo, vec4(GetSpecularColor(), 0.f));
		vec4 color = vec4(shadedColor.rgb, alpha);

		vec3 toView = viewPos - FragPos;
		float dist = length(toView);

		if (renderSettings.fogEnabled == 1)
			color = CalculateDistanceFog(color, renderSettings.fogColor, dist, renderSettings.fogSight, cameraZFar);
	
		InsertABufferFragment(ivec2(gl_FragCoord.xy), color, gl_FragCoord.z);

		discard;
	}

	b0Out = PackGBuffer(
		GetDiffuseColor(), 
		material.shininess,
		GetSpecularColor().rgb,
		0,
		norm,
		GetAO(),
		GetRoughness(),
		GetMetallic()
	);
}

vec3 GetDiffuseColor() // returns either a texture diffuse color or vec3(1)
{
	if (material.numDiffusemaps != 0)
	{
		return vec3(texture(material.diffusemap, TexCoords)) * material.color;
	}
	return material.color;
}
vec3 GetSpecularColor()// returns either a texture specular color or vec3(1)
{
	if (material.numSpecularmaps != 0)
	{
		return texture(material.specularmap, TexCoords).rgb;
	}
	return vec3(0.);
}

vec3 GetNormal2(sampler2D normalMap)
{
	vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
};


vec3 GetNormal()
{
	if (material.numNormalmaps != 0)
	{
		return GetNormal2(material.normalmap);
	}
	if (material.numHeightmaps != 0)		
	{
		return GetNormal2(material.heightmap);
	}

	return normalize(Normal);
}

float GetAlphaValue()
{
	if (material.numOpacitymaps != 0)
		return texture(material.opacitymap, TexCoords).r;	
	return texture(material.diffusemap, TexCoords).a;
}

float GetAO()
{
	if (material.numAOmaps != 0)
	{
		return texture(material.AOmap, TexCoords).r;
	}

	return 1.f;
}

float GetMetallic()
{
	if (material.numMetallicmaps != 0)
	{
		return texture(material.metallicmap, TexCoords).r;
	}
	return 0.0f;
}

float GetRoughness()
{
	if (material.numRoughnessmaps != 0)
	{
		return texture(material.roughnessmap, TexCoords).r;
	}
	return 0.5f;
}

float GetHeight()
{
	if (material.numHeightmaps != 0)
	{
		return texture(material.heightmap, TexCoords).r;
	}
	return 1.0f;
}