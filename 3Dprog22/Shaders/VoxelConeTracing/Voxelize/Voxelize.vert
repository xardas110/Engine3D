#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

#include "../../common/camera.frag"

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

out Vertex
{
	vec3 texCoords;
	vec3 normal;
}Out;

uniform mat4 model;

void main()
{
	Out.texCoords = vec3(aTexCoords, 0.);	

	vec4 ws = model *  vec4(aPos, 1.f);

	Out.normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	gl_Position = ws;	
}