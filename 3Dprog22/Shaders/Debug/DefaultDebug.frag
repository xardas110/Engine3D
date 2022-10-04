#version 460 core

layout(early_fragment_tests) in;

out uvec4 b0;

#include "../Common/GBuffer.frag"
#include "../Common/ShaderModelDefines.frag"

void main()
{
	b0 = PackGBuffer(
		vec3(0.f, 0.f, 1.f), 
		1.f,
		vec3(0),
		SHADERMODEL_DEBUG,
		vec3(0.f, 1.f, 0.f),
		1.f,
		1.f,
		0.f);
}
