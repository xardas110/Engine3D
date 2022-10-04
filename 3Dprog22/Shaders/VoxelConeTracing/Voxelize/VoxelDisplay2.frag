#version 460 core
out vec4 finalColorOut;
  
#include "../../Common/RaycastUtility.frag"
#include "../../Common/Camera.frag"

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

vec3 gridCenter = voxelData.gridCenter.rgb;
vec3 gridSize = voxelData.gridSize.rgb;
vec3 halfGrid = gridSize / 2.;

uniform sampler3D voxelStructure;

vec3 ToVoxel(vec3 pos)
{
	vec3 minBB = voxelData.gridCenter.xyz - vec3(halfGrid);
	vec3 scale = 1.f / vec3(gridSize);
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

#define BAYER_FACTOR 1.0/16.0
uniform float bayerFilter[16u] = float[]
(
	0.0*BAYER_FACTOR, 8.0*BAYER_FACTOR, 2.0*BAYER_FACTOR, 10.0*BAYER_FACTOR,
	12.0*BAYER_FACTOR, 4.0*BAYER_FACTOR, 14.0*BAYER_FACTOR, 6.0*BAYER_FACTOR,
	3.0*BAYER_FACTOR, 11.0*BAYER_FACTOR, 1.0*BAYER_FACTOR, 9.0*BAYER_FACTOR,
	15.0*BAYER_FACTOR, 7.0*BAYER_FACTOR, 13.0*BAYER_FACTOR, 5.0*BAYER_FACTOR
);

/*Local to texture*/
vec4 Raytrace(vec3 sp, vec3 ep, float stepSize)
{
	vec3 toEnd = ep - sp;
	float len = length(toEnd);
	vec3 dir = toEnd / len;

	vec4 result;

	int a = int(gl_FragCoord.x) % 4;
	int b = int(gl_FragCoord.y) % 4;

	vec3 pos = sp + dir * bayerFilter[a*4+b];
	float dist = 0;

	while(result.a < 1. && dist < 768.)
	{
		if (PtBoxIntersect(pos, -halfGrid, halfGrid))
		{
			vec3 uv = ToVoxel(pos);
			result += textureLod(voxelStructure, uv, 0);		
		}
		pos += (dir * stepSize);
		dist += stepSize;
	}

	return result;
}

in vec2 TexCoords;

void main()
{
	vec3 worldDir = ComputeWorldDir(TexCoords, inverse(view), inverse(proj));

	vec3 minBB = gridCenter + vec3(-halfGrid);
	vec3 maxBB = gridCenter + vec3(halfGrid);

	vec3 t =  RayBoxIntersect(viewPos, worldDir, minBB, maxBB);

	if (t.x != 0)
	{	
		vec3 sp = vec3(0.);
		if (PtBoxIntersect(viewPos, minBB, maxBB))
		{
			sp = viewPos;
		}
		else
		{
			sp = viewPos + worldDir * t.y; 
		}
		vec3 ep = viewPos + worldDir * t.z; 

		finalColorOut = Raytrace(sp, ep, 1.);
	}
	else
	{
		finalColorOut = vec4(0.f, 1.f, 0.f, 1.0f);
	}
}

