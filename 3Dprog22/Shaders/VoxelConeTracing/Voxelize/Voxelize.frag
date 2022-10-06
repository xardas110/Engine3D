#version 460 core

//https://github.com/jose-villegas/VCTRenderer with some modifications

#define MAX_CASCADES 4

#include "../../common/Camera.frag"

struct VoxelGPUData
{
	vec4 gridCenter;
	vec4 gridSize;
	ivec4 dims;
	mat4 projX, projY, projZ;
};

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

layout (std140, binding=6) uniform VoxelData
{
	VoxelGPUData voxelData;
};

layout (pixel_center_integer) in vec4 gl_FragCoord;

layout(binding = 0, r32ui) uniform volatile coherent uimage3D outAlbedo;
layout(binding = 1, r32ui) uniform volatile coherent uimage3D outNormal;

uniform sampler2DArray shadowmaps;
uniform sampler2D albedo;
uniform int numAlbedo;

in GeometryOut
{
	vec3 fragWS;
	vec3 lightWS;
	vec3 normal;
	vec3 pos;
	vec3 texCoords;
	mat4 projection;
	flat int axis;
	flat vec4 triangleAABB;
} In;

vec4 GetAlbedoColor()
{
	if (numAlbedo > 0) return texture(albedo, In.texCoords.xy);
	return vec4(1., 0.,0.,1.);
}

float GetShadowSimple(in vec3 fragPos)
{
	vec4 vs = view * vec4(fragPos, 1.f);
	int level = 3; float depth = abs(vs.z);
	for (int i = 0; i< MAX_CASCADES; i++) 
	{
		if (depth < cascades[i].cascadeOffsetX)
		{
			level = i;
			break;
		}
	}
	vs = cascades[level].cascadeProjection *  vec4(fragPos, 1.f);

	vec3 UVD = vec3(
		vs.x,
		vs.y,
		vs.z);
	UVD.xyz = UVD.xyz * 0.5f + 0.5f;

	float fragDepth = UVD.z - 0.0005f;
 
	return texture(shadowmaps, vec3(UVD.xy, level)).r > fragDepth ? 1.f : 0.00f;
}

vec3 CalcDirLight(
	vec3 viewDir, 
	vec3 normal, 
	vec3 albedoCol)
{
	vec3 lightDir = normalize(-dirlight.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 diffuse = dirlight.diffuse * diff * albedoCol * dirlight.itensity;
	return diffuse;
}

vec3 BRDF(DirLight light, vec3 normal, vec3 albedo)
{
    float nDotL = 0.0f;

    vec3 weight = normal * normal;
    // calculate directional normal attenuation
    float rDotL = dot(vec3(1.0, 0.0, 0.0), -light.direction);
    float uDotL = dot(vec3(0.0, 1.0, 0.0), -light.direction);
    float fDotL = dot(vec3(0.0, 0.0, 1.0), -light.direction);

    rDotL = normal.x > 0.0 ? max(rDotL, 0.0) : max(-rDotL, 0.0);
    uDotL = normal.y > 0.0 ? max(uDotL, 0.0) : max(-uDotL, 0.0);
    fDotL = normal.z > 0.0 ? max(fDotL, 0.0) : max(-fDotL, 0.0);
    // voxel shading average from all front sides
    nDotL = rDotL * weight.x + uDotL * weight.y + fDotL * weight.z;

    return light.diffuse * albedo * nDotL;
}

vec4 convRGBA8ToVec4(uint val)
{
    return vec4(float((val & 0x000000FF)), 
    float((val & 0x0000FF00) >> 8U), 
    float((val & 0x00FF0000) >> 16U), 
    float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val)
{
    return (uint(val.w) & 0x000000FF) << 24U | 
    (uint(val.z) & 0x000000FF) << 16U | 
    (uint(val.y) & 0x000000FF) << 8U | 
    (uint(val.x) & 0x000000FF);
}

/*
void imageAtomicRGBA8Avg(layout(r32ui) volatile coherent uimage3D grid, ivec3 coords, vec4 value)
{
    value.rgba *= 255.0;                 // optimize following calculations
    uint newVal = convVec4ToRGBA8(value);
    uint prevStoredVal = 0;
    uint curStoredVal;
    uint numIterations = 0;

    while((curStoredVal = imageAtomicCompSwap(grid, coords, prevStoredVal, newVal)) 
            != prevStoredVal
            && numIterations < 255)
    {
        prevStoredVal = curStoredVal;
        vec4 rval = convRGBA8ToVec4(curStoredVal);
        rval.rgb = (rval.rgb * rval.a); // Denormalize
        vec4 curValF = rval + value;    // Add
        curValF.rgb /= curValF.a;       // Renormalize
        newVal = convVec4ToRGBA8(curValF);

        ++numIterations;
    }
}
*/

vec3 EncodeNormal(vec3 normal)
{
    return normal * 0.5f + vec3(0.5f);
}

vec3 DecodeNormal(vec3 normal)
{
    return normal * 2.0f - vec3(1.0f);
}

void main()
{

	if( In.pos.x < In.triangleAABB.x || In.pos.y < In.triangleAABB.y || 
	In.pos.x > In.triangleAABB.z || In.pos.y > In.triangleAABB.w )
	{
		discard;
	}
	ivec3 p = ivec3(In.fragWS);

	/*
	ivec3 pos = ivec3(gl_FragCoord.x, gl_FragCoord.y, voxelData.dims.z * gl_FragCoord.z);

	ivec3 p;
	switch(In.axis)
	{
		case 1:
		{
			p.x = voxelData.dims.x - pos.z;
			p.z = pos.x;
			p.y = pos.y;
		}
		break;
		case 2:
		{
			p.z = pos.y;
			p.y = voxelData.dims.y - pos.z;
			p.x = pos.x;
		}
		break;
		default:
		{
			p = pos;
		}
		break;
	}

	p.z = voxelData.dims.z - p.z - 1;
	*/
	vec3 albedo = texture(albedo, In.texCoords.xy).rgb;
	vec4 normal = vec4(EncodeNormal(normalize(In.normal)), 1.0f);

	{
		vec4 value =  vec4(albedo, 1.f);
		value.rgba *= 255.0;                 // optimize following calculations
		uint newVal = convVec4ToRGBA8(value);
		uint prevStoredVal = 0;
		uint curStoredVal;
		uint numIterations = 0;

		while((curStoredVal = imageAtomicCompSwap(outAlbedo, p, prevStoredVal, newVal)) 
				!= prevStoredVal
				&& numIterations < 255)
		{
			prevStoredVal = curStoredVal;
			vec4 rval = convRGBA8ToVec4(curStoredVal);
			rval.rgb = (rval.rgb * rval.a); // Denormalize
			vec4 curValF = rval + value;    // Add
			curValF.rgb /= curValF.a;       // Renormalize
			newVal = convVec4ToRGBA8(curValF);

			++numIterations;
		}
	}
	{
		vec4 value =  normal;
		value.rgba *= 255.0;                 // optimize following calculations
		uint newVal = convVec4ToRGBA8(value);
		uint prevStoredVal = 0;
		uint curStoredVal;
		uint numIterations = 0;

		while((curStoredVal = imageAtomicCompSwap(outAlbedo, p, prevStoredVal, newVal)) 
				!= prevStoredVal
				&& numIterations < 255)
		{
			prevStoredVal = curStoredVal;
			vec4 rval = convRGBA8ToVec4(curStoredVal);
			rval.rgb = (rval.rgb * rval.a); // Denormalize
			vec4 curValF = rval + value;    // Add
			curValF.rgb /= curValF.a;       // Renormalize
			newVal = convVec4ToRGBA8(curValF);

			++numIterations;
		}
	}

//	imageAtomicRGBA8Avg(outAlbedo, p, vec4(albedo, 1.f));
//	imageAtomicRGBA8Avg(outNormal, p, normal);
}
