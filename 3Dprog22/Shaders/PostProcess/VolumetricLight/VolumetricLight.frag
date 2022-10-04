#version 460 core

layout(location = 0) out vec4 ScatterColor;

#include "../../Common/GBuffer.frag"
#include "../../Common/Camera.frag"

#define MAX_CASCADES 4
#define SHADOW_DEPTH_BIAS 0.0005

uniform int numSteps = 100;
uniform float scattering = 0.3f;
const float scattering2 = scattering*scattering;
const float PI = 3.14159;
const float sa = 4.0f * PI;

uniform vec3 sunColor = vec3(1.9f, 1.7f, 1.40f);

struct Cascade
{
	mat4 cascadeProjection;
	float cascadeOffsetX;
	float cascadeOffsetY;
	float cascadeScaleX;
	float cascadeScaleY;
};

struct DirLight {
	vec3 ambient;
	float pad1;
	vec3 diffuse;
	float pad2;
	vec3 specular;	
	float pad3;
	vec3 direction;
	float itensity;
};

layout (std140, binding=0) uniform CSMMatrices
{
	mat4 toShadowSpaceMat;
	Cascade cascades[MAX_CASCADES];
};

layout (std140, binding=1) uniform DirlightUBO
{
	DirLight dirlight;
};

uniform sampler2DArray smCascades;

in vec2 TexCoords;

bool CalcDirLightShadow(DirLight light, vec3 FragPos)
{
	vec4 frag_lightSpace = view * vec4(FragPos, 1.f);
	int level = MAX_CASCADES - 1; float depth = abs(frag_lightSpace.z);
	for (int i = 0; i< MAX_CASCADES; i++) //best cascade will always be the first
	{
		if (depth < cascades[i].cascadeOffsetX)
		{
			level = i;
			break;
		}
	}
	frag_lightSpace = cascades[level].cascadeProjection *  vec4(FragPos, 1.f);

	vec3 UVD = vec3(
		frag_lightSpace.x,
		frag_lightSpace.y,
		frag_lightSpace.z);
	UVD.xyz = UVD.xyz * 0.5f + 0.5f;

	float fragDepth = UVD.z - SHADOW_DEPTH_BIAS;
	return texture(smCascades, vec3(UVD.xy, level)).r	> fragDepth ? true : false;	 
}

float ComputeScattering(float LdV)
{
	float result = 1.0f - scattering2;
	result /= (sa * pow(1.0f + scattering2 - (2.0f * scattering) * LdV, 1.5f));
	return result;
}

vec3 ComputeScatteringColor()
{
	vec3 fragPos = WorldPosFromDepth(TexCoords, inverse(proj), inverse(view));

	vec3 cameraPos = viewPos;
	vec3 pixelPos = fragPos;

	vec3 lightDir = -dirlight.direction;

	vec3 rayVector = pixelPos - cameraPos;
	
	float rayLength = length(rayVector);

	vec3 rayDir = rayVector / rayLength;

	float stepLength = rayLength / numSteps;

	vec3 rayStep = rayDir * stepLength;

	vec3 currentPosition = cameraPos;

	vec3 result = 0.f.xxx;
	
	float LdV = dot(rayDir, lightDir);

	for (int i = 0; i < numSteps; i++)
	{
		bool shadowFactor = CalcDirLightShadow(dirlight, currentPosition);

		if (shadowFactor)
		{
			result += ComputeScattering(LdV) * sunColor;
		}
		currentPosition += rayStep;
	}

	result /= numSteps;

	return result;
}

void main()
{
	mat4 invView = inverse(view);
	vec3 camDir = vec3(invView[0][3], invView[1][3], invView[2][3]);
	vec3 lightDir = -dirlight.direction;

	if (dot(camDir, lightDir) < 0.0f) return;

	ScatterColor = vec4(ComputeScatteringColor(), 1.f);
}
