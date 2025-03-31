#version 460 core
layout(location = 0) out vec4 colorOut;
layout(location = 1) out vec4 brightColorOut;


#include "../../Common/DirectionalLight.frag"
#include "../../DistanceFog/DistanceFog.frag"
#include "../../Common/ABuffer.frag"
#include "../../Common/PBR.frag"
#include "../../Common/Camera.frag"
#include "../../Common/RenderSettings.frag"
#include "../../Common/ShaderModel.frag"
#include "../../Common/ShaderModelDefines.frag"

//#include "../../Common/ErrorColorDefines.frag"

struct IndirectLight
{
	sampler2D diffuseAO;
	sampler2D specularShadow;
};

uniform sampler2D volumetricLightmap;
uniform sampler2D SSAOmap;

uniform sampler2D cloudColorTex;
uniform sampler2D cloudShadowVLTex;

uniform IndirectLight indirectLight; 

uniform float fTime = 0.0f;
uniform float bounceStrength = 5;

const mat4 iView = inverse(view);
const mat4 iProj = inverse(proj);

in vec2 TexCoords;

float CalcDirLightShadow(DirLight light, vec3 fragPos, vec3 norm);

const float EPSILON = 0.00001f;

bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
	FragInfo_t fi = UnpackGBuffer(TexCoords);
	
	vec3 albedo = fi.albedo;//pow(fi.albedo, vec3(2.2f));
	
	vec3 fragPos = WorldPosFromDepth(TexCoords, iProj, iView);

	vec3 volumetricLight = renderSettings.volumetricLightEnabled == 1?
		texture(volumetricLightmap, TexCoords).rgb : 0.f.xxx;

	float dlShadowFactor = CalcDirLightShadow(dirlight, fragPos, fi.normal, view);

	float fragDepth = texture(gBuffer.depth, TexCoords).r;

	float ssao = 1.f;

	vec3 viewDir = viewPos - fragPos;
	viewDir = normalize(viewDir);

	if(renderSettings.ssaoEnabled == 1)
	{
		ssao = texture(SSAOmap, TexCoords).r;
	}

	switch (fi.bitfield) // direct light
	{
		case SHADERMODEL_BRDF:
		{
			colorOut = vec4(PHONG(dirlight, viewDir, fi.normal, albedo, fi.specular, fi.specPower, ssao, fi.ao, dlShadowFactor), 1);

		}
		break;
		case SHADERMODEL_TERRAIN:
		{
			colorOut = vec4(PHONG(dirlight, viewDir, fi.normal, albedo, vec3(0), fi.specPower, ssao, fi.ao, dlShadowFactor), 1);

		}
		break;
		case SHADERMODEL_WATER:
		{
			colorOut = vec4(fi.albedo, 1.f);
		}
		break;
		case SHADERMODEL_SKY:
		{
			vec4 cloudColor = texture(cloudColorTex, TexCoords);
			colorOut = SM_Sky(cloudColor, volumetricLight);
			
			vec3 toView = viewPos - fragPos;
			float dist = length(toView) * 0.5;
			colorOut = CalculateDistanceFog(colorOut, renderSettings.fogColor, dist, renderSettings.fogSight, cameraZFar);
		}
		break;
		case SHADERMODEL_SUN:
		{
			colorOut = SM_Sun(fi.albedo);
		}
		break;
		case SHADERMODEL_UNLIT:
		{
			colorOut = vec4(fi.albedo, 1.f);
		}
		break;
		case SHADERMODEL_DEBUG:
		{
			colorOut = vec4(fi.albedo, 1.f);
			BlendABuffer(ivec2(gl_FragCoord.xy), colorOut.rgb, fragDepth);
			return;
		}
		break;
		default:
		{
			colorOut = vec4(0.f, 1.f, 0.f, 1.f);
		}
		break;
	}

	if (renderSettings.giEnabled == 1 && fi.bitfield != SHADERMODEL_SKY && fi.bitfield != SHADERMODEL_SUN) //indirect light
	{	
		vec4 indirectDiffuseAO = texture(indirectLight.diffuseAO, TexCoords);
		vec4 indirectSpecularShadow = texture(indirectLight.specularShadow, TexCoords);

		vec3 indirectDiffuse = indirectDiffuseAO.rgb * albedo;
		vec3 indirectSpecular = indirectSpecularShadow.rgb * fi.specular;
		vec3 indirectLighting = (indirectDiffuse + indirectSpecular) * dirlight.itensity * 5.f;

		colorOut.rgb = (colorOut.rgb + indirectLighting) * indirectDiffuseAO.a;
	}

	if (renderSettings.fogEnabled == 1 && fi.bitfield != SHADERMODEL_SKY && fi.bitfield != SHADERMODEL_SUN)
	{
		vec3 toView = viewPos - fragPos;
		float dist = length(toView);
		colorOut = CalculateDistanceFog(colorOut, renderSettings.fogColor, dist, renderSettings.fogSight, cameraZFar);
	}

	BlendABuffer(ivec2(gl_FragCoord.xy), colorOut.rgb, fragDepth);

	if (renderSettings.volumetricLightEnabled == 1)
	{
		colorOut.rgb += volumetricLight;
	}

	float brightness = dot(colorOut.rgb, renderSettings.bloomThreshold);
	if (brightness > 1.f && fi.bitfield != SHADERMODEL_SKY && fi.bitfield != SHADERMODEL_SUN)
	{
		brightColorOut = colorOut;
	}
	else
	{
		brightColorOut = vec4(vec3(0), 1.f);
	}
}
