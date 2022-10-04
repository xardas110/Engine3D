#version 460 core
layout (location = 0) in vec3 pos;   

#include "common/camera.frag"

struct GPUDATA
{
	mat4 model;
	vec4 color;
};

layout (std430, binding=6) buffer instancedMatrices
{
	GPUDATA gpuDatas[];
};

out vec4 FS_Color;

void main()
{
	GPUDATA gpuData = gpuDatas[gl_InstanceID];

	mat4 model = gpuData.model;
	FS_Color = gpuData.color;

    gl_Position = VP * model * vec4(pos.x, pos.y, pos.z,1.0);
}  