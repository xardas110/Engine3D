#version 460 core

layout(location = 0) out uvec4 b0Out;

layout(early_fragment_tests) in;
 
#include "../Common/DirectionalLight.frag"
#include "../DistanceFog/DistanceFog.frag"
#include "../Common/ABuffer.frag"
#include "../Common/Camera.frag"
#include "../Common/Rendersettings.frag"
#include "../Common/ShaderModel.frag"

struct Material {
	sampler2D diffusemap;
	sampler2D specularmap;
	sampler2D normalmap;
	sampler2D opacitymap; 
	sampler2D AOmap; 
	sampler2D metallicmap;
	sampler2D roughnessmap;

	int numNormalmaps;
	int numDiffusemaps;
	int numSpecularmaps;
	int numOpacitymaps;
	int numAOmaps;
	int numMetallicmaps;
	int numRoughnessmaps;

	vec3 color; 
	float shininess;
};

uniform Material material;
uniform sampler2D depthmap;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

float GetAlphaValue();

float GetAO();
vec3 GetDiffuseColor();
vec3 GetSpecularColor();
vec3 GetNormal();
float GetMetallic();
float GetRoughness();

void main()
{
	float alpha = GetAlphaValue();
	vec3 norm = GetNormal();

	float shadowFactor = CalcDirLightShadow(dirlight, FragPos, norm, view);

	vec3 viewDir = viewPos - FragPos;
	viewDir = normalize(viewDir);

	vec4 shadedColor = vec4(PHONG(dirlight, viewDir, norm, GetDiffuseColor(), GetSpecularColor(), material.shininess, 1.f, GetAO(), shadowFactor), 1) * 0.5;

	vec4 color = vec4(shadedColor.rgb, alpha);
	vec3 toView = viewPos - FragPos;
	float dist = length(toView);

	if (renderSettings.fogEnabled == 1)
		color = CalculateDistanceFog(color, renderSettings.fogColor, dist, renderSettings.fogSight, cameraZFar);
	
	InsertABufferFragment(ivec2(gl_FragCoord.xy), color, gl_FragCoord.z);
	discard;
	
}

vec3 GetDiffuseColor() // returns either a texture diffuse color or vec3(1)
{
	if (material.numDiffusemaps != 0)
	{
		return vec3(texture(material.diffusemap, TexCoords));
	}
	return vec3(1.f, 1.f, 1.f);
}
vec3 GetSpecularColor()// returns either a texture specular color or vec3(1)
{
	if (material.numSpecularmaps != 0)
	{
		return texture(material.specularmap, TexCoords).rgb;
	}
	return vec3(0.f);
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

	return normalize(Normal);
}

float GetAlphaValue()
{
	if (material.numOpacitymaps != 0)
		return texture(material.opacitymap, TexCoords).r;	
	return 1.f;
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
	return 0.95f;
}