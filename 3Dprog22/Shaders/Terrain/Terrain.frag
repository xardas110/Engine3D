#version 460

#define MAX_CASCADES 4
#define SHADOW_DEPTH_BIAS 0.00003

layout(location = 0) out uvec4 b0Out;

#include "../Common/gBuffer.frag"
#include "../Common/Camera.frag"
#include "../Common/ShaderModelDefines.frag"

struct Material
{
	sampler2D diffusemap;
	sampler2D specularmap;
	sampler2D normalmap;
	sampler2D heightmap;
	sampler2D aomap;
	sampler2D roughnessmap;
	
	float heightScaling;
	float horizontalScaling;
};

uniform sampler2D normalmap;
uniform sampler2D splatmap;
uniform Material materials[3];
uniform int tessellationRange;
uniform bool bShowHeightLines;

in vec2 mapCoord_FS;
in vec3 position_FS;
in vec3 tangent_FS;

float GetAOValue(int index);

void main()
{
	float dist = length(viewPos - position_FS);
	float height = position_FS.y;

	vec3 normal = normalize(texture(normalmap, mapCoord_FS).rbg);

	vec4 blendValues = texture(splatmap, mapCoord_FS).rgba;
	
	float[4] blendValueArray = float[](blendValues.r,blendValues.g,blendValues.b,blendValues.a);
	
	if (dist < tessellationRange-50)
	{
		float attenuation = clamp(-dist/(tessellationRange-50) + 1,0.0,1.0);
		
		vec3 bitangent = normalize(cross(tangent_FS, normal));
		mat3 TBN = mat3(bitangent,normal,tangent_FS);
		
		vec3 bumpNormal;
		for (int i=0; i<3; i++){		
			bumpNormal += (2*(texture(materials[i].normalmap, mapCoord_FS * materials[i].horizontalScaling).rbg) - 1) * blendValueArray[i];
		}
		
		bumpNormal = normalize(bumpNormal);
		
		bumpNormal.xz *= attenuation;
		
		normal = normalize(TBN * bumpNormal);
	}
	
	vec3 diffuseColor = vec3(0.f, 0.f, 0.f);
	float roughness = 0.f;
	float specularColor = 0.f;

	float ambientOcclusion = 0.f;

	for (int i=0; i<3; i++)
	{
		ambientOcclusion +=  GetAOValue(i)* blendValueArray[i];			
	}

	for (int i=0; i<3; i++){
		diffuseColor +=  texture(materials[i].diffusemap, mapCoord_FS * materials[i].horizontalScaling).rgb
					* blendValueArray[i];
	}

	for (int i=0; i<3; i++){
		specularColor +=  texture(materials[i].specularmap, mapCoord_FS * materials[i].horizontalScaling).r
					* blendValueArray[i];
	}

	for (int i=0; i<3; i++){
		roughness +=  texture(materials[i].roughnessmap, mapCoord_FS * materials[i].horizontalScaling).r
					* blendValueArray[i];
	}
		
	if (bShowHeightLines)
	{
		if (int(position_FS.y) % 5 == 0 && fract(position_FS.y) < 0.1f)
		{
			diffuseColor = vec3(1.f, 0.f, 0.f);
		}
		if (int(position_FS.y) % 100 == 0 && fract(position_FS.y) < 0.4f)
		{
			diffuseColor = vec3(1.f);
		}
	}

	b0Out = PackGBuffer(
		diffuseColor, 
		0.f,
		vec3(0.f),
		SHADERMODEL_TERRAIN,
		normal,
		ambientOcclusion,
		roughness,
		0
	);
}

float GetAOValue(int index)
{
	return texture(materials[index].aomap, mapCoord_FS * materials[index].horizontalScaling).r;
}