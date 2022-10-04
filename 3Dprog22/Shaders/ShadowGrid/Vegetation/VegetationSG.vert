#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform int instanceOffset;

layout (std430, binding=1) buffer instancedMatrices
{
	mat4 instanceMat[];
};

out vec2 TexCoords_GS;

void main()
{

	TexCoords_GS = aTexCoords;

	mat4 model = instanceMat[instanceOffset + gl_InstanceID];

	gl_Position = model * vec4(aPos, 1.f);	

}