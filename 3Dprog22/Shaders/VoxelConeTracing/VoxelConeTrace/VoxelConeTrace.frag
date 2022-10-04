#version 460 core

layout(location = 0) out vec4 outDiffuseAO;
layout(location = 1) out vec4 outSpecularShadow;
  
#include "../../Common/RaycastUtility.frag"
#include "../../Common/Gbuffer.frag"
#include "../../Common/camera.frag"

const float PI = 3.14159265f;
const float HALF_PI = 1.57079f;
const float EPSILON = 1e-30;
const float SQRT_3 = 1.73205080f;

#define BAYER_FACTOR 1.0/16.0
uniform float bayerFilter[16u] = float[]
(
	0.0*BAYER_FACTOR, 8.0*BAYER_FACTOR, 2.0*BAYER_FACTOR, 10.0*BAYER_FACTOR,
	12.0*BAYER_FACTOR, 4.0*BAYER_FACTOR, 14.0*BAYER_FACTOR, 6.0*BAYER_FACTOR,
	3.0*BAYER_FACTOR, 11.0*BAYER_FACTOR, 1.0*BAYER_FACTOR, 9.0*BAYER_FACTOR,
	15.0*BAYER_FACTOR, 7.0*BAYER_FACTOR, 13.0*BAYER_FACTOR, 5.0*BAYER_FACTOR
);

const int NUM_CONES = 6;
vec3 coneDirections[6] = vec3[](   
	vec3(0, 1, 0),
    vec3(0, 0.5, 0.866025),
    vec3(0.823639, 0.5, 0.267617),
    vec3(0.509037, 0.5, -0.700629),
    vec3(-0.509037, 0.5, -0.700629),
    vec3(-0.823639, 0.5, 0.267617));
    
float coneWeights[6] = float[]( PI / 4.0f,
    3.0f * PI / 20.0f,
    3.0f * PI / 20.0f,
    3.0f * PI / 20.0f,
    3.0f * PI / 20.0f,
    3.0f * PI / 20.0f);

struct VoxelGPUData
{
	vec4 gridCenter;
	vec4 gridSize;
	ivec4 dims;
	mat4 projX, projY, projZ;
};

layout (std140, binding=6) uniform VoxelData
{
	VoxelGPUData voxelData;
};

uniform sampler3D voxelStructure;
uniform sampler3D voxelMips[6];
uniform float aoFalloff = 800.0f;
uniform float aoAlpha = 0.0f;

vec3 gridCenter = voxelData.gridCenter.rgb;
vec3 gridSize = voxelData.gridSize.rgb;
vec3 halfGrid = gridSize / 2.;

vec3 ToVoxel(vec3 pos)
{
	vec3 minBB = voxelData.gridCenter.xyz - halfGrid;
	vec3 scale = 1.f / gridSize;
	vec3 vp = pos - minBB;
	return vp * scale;
}

vec3 ComputeClipSpaceCoord(in vec2 texCoords){
	vec2 ray_nds = 2.0*texCoords - 1.0;
	return vec3(ray_nds, 1.0);
}

vec3 ComputeWorldDir(in vec2 texCoords, in mat4 invView, in mat4 invProj)
{
	vec4 rayClip = vec4(ComputeClipSpaceCoord(texCoords), 1.f);
	vec4 rayView = invProj * rayClip;
	rayView = vec4(rayView.xy, -1.f, 0.f);
	vec3 worldDir = (invView * rayView).xyz;
	return normalize(worldDir);
}

vec4 LinearSample(vec3 coord,  float lod)
{
	return textureLod(voxelStructure, coord, lod);
}

vec4 AnistropicSample(vec3 coord, vec3 weight, uvec3 face, float lod)
{
    float anisoLevel = max(lod - 1.0f, 0.0f);
    vec4 anisoSample = weight.x * textureLod(voxelMips[face.x], coord, anisoLevel)
                     + weight.y * textureLod(voxelMips[face.y], coord, anisoLevel)
                     + weight.z * textureLod(voxelMips[face.z], coord, anisoLevel);

    if(lod < 1.0f)
    {
        vec4 baseColor = texture(voxelStructure, coord);
        anisoSample = mix(baseColor, anisoSample, clamp(lod, 0.0f, 1.0f));
    }

    return anisoSample;                    
}

vec4 ConeTrace(vec3 sp, vec3 dir, vec3 surfaceNormal, float tanAngle, float stepSize, out float occlusion)
{
	vec4 result = vec4(0);

	uvec3 visibleFace;
	visibleFace.x = (dir.x < 0.0) ? 0 : 1;
    visibleFace.y = (dir.y < 0.0) ? 2 : 3;
    visibleFace.z = (dir.z < 0.0) ? 4 : 5;

	vec3 weight = dir * dir;

	float voxelScale = 1. / float(gridSize.x);
	float voxelSize = 2.f / (float(gridSize.x) * voxelScale);

	float falloff = 0.5f * aoFalloff * voxelScale;

	float dist = voxelSize;
	sp = sp + surfaceNormal * dist;

	float occ = 1.;

	while(result.a < 1.f && dist < 200)
	{	
		vec3 pos = sp + dir * dist;
		vec3 uv = ToVoxel(pos);
		float d = 2. * tanAngle * dist;

		float lod = log2(d / voxelSize);
		vec4 voxelColor = AnistropicSample(uv, weight, visibleFace, lod);//LinearSample(uv, lod);//	

		result += (1.0f - result.a) * voxelColor;
		occ -= voxelColor.a;

		if (occlusion < 1.f) occlusion += ((1.0f - occlusion) * voxelColor.a) / (1.0f + falloff * d);
			
		dist += (stepSize * d);	
	}

	return result;
}

vec4 IndirectDiffuse(in vec3 fragPos, in vec3 fragNorm, out float occlusion)
{
	vec4 color = vec4(0.f);

	vec3 sp = fragPos;

	const float aperture = 0.57735f;

	vec3 norm = fragNorm;

	vec3 guide = vec3(0.0f, 1.0f, 0.0f);

    if (abs(dot(fragNorm,guide)) == 1.0f)
    {
        guide = vec3(0.0f, 0.0f, 1.0f);
    }
    vec3 right = normalize(guide - dot(fragNorm, guide) * fragNorm);
    vec3 up = cross(right, fragNorm);

	vec3 coneDirection = vec3(0.0f);
	for(int i = 0; i < NUM_CONES; i++)
	{
		coneDirection = fragNorm;
        coneDirection += coneDirections[i].x * right + coneDirections[i].z * up;
        coneDirection = normalize(coneDirection);
		color += (coneWeights[i] * ConeTrace(sp, coneDirection, fragNorm, aperture, 0.5, occlusion));	
	}
	return color;
}

vec4 IndirectSpecular(in vec3 fragPos, in vec3 fragNorm, out float specOcclusion)
{
	vec4 color = vec4(0.f);

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-viewDir, fragNorm);
	reflectDir = normalize(reflectDir);
	
	float aperture = clamp(tan(HALF_PI * 0.5f), 0.0174533f, PI);

	color = ConeTrace(fragPos, reflectDir, fragNorm, aperture, 0.5, specOcclusion);
	
	return color;
}

in vec2 TexCoords;

void main()
{

	FragInfo_t fi = UnpackGBuffer(TexCoords);

	vec3 fragPos = WorldPosFromDepth(TexCoords, inverse(proj), inverse(view));
	vec3 fragNorm = fi.normal;

	float occlusion = 0.f;
	float specularOcclusion = 0.f;

	vec4 id = IndirectDiffuse(fragPos, fragNorm, occlusion);

	occlusion = clamp(1.0f - occlusion + aoAlpha, 0.0f, 1.0f);
	outDiffuseAO = vec4(id.rgb, occlusion);
	outSpecularShadow = IndirectSpecular(fragPos, fragNorm, specularOcclusion);
}

