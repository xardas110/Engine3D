#version 460 core
layout(location = 0) out float SSAOColorOut;

#define SSAO_KERNEL_SIZE 32

#include "../../Common/GBuffer.frag"
#include "../../Common/Camera.frag"

uniform sampler2D noisemap;
uniform vec2 dim;

uniform float zNear = 0.1f;
uniform float zFar = 2000.f;

uniform float radius = 2.5f;
uniform float bias = 1.5f;

uniform vec3 kernel[SSAO_KERNEL_SIZE];

in vec2 TexCoords;

mat4 invProj = inverse(proj);
mat4 invView = inverse(view);

//Source: https://learnopengl.com/Advanced-Lighting/SSAO
//Modifications made
void main()
{

	vec2 noiseTexSize = textureSize(noisemap, 0);
	vec2 noiseScale = dim / noiseTexSize;

	FragInfo_t fi = UnpackGBuffer(TexCoords);

	vec3 fragPos = (view * vec4(WorldPosFromDepth(TexCoords, invProj, invView), 1.f)).xyz;
	vec3 normal = normalize(fi.normal).rgb;
	vec3 randomVec = texture(noisemap, TexCoords * noiseScale).xyz;

	vec3 T = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 B = cross(normal, T);
	mat3 TBN = mat3(T,B,normal);

	float ao = 0.f;

	for (int i = 0; i < SSAO_KERNEL_SIZE; ++i)
	{
		vec3 samplePos = TBN * kernel[i];
		samplePos = fragPos + samplePos * radius;

		vec4 offset = proj * vec4(samplePos, 1.f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		float sampleDepth = (view * vec4(WorldPosFromDepth(offset.xy, invProj, invView), 1.f)).z;
		float rangeCheck = smoothstep(0.f, 1.f, radius / abs(fragPos.z - sampleDepth));
		ao += (sampleDepth >= samplePos.z + bias? 1.f:0.f) * rangeCheck;
	}

	float result = 1.f - (ao / SSAO_KERNEL_SIZE);
	
	SSAOColorOut = pow(result, 4);
}