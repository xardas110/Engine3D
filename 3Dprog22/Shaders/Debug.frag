#version 460 core

layout(early_fragment_tests) in;

out uvec4 b0;

in vec4 FS_Color;

#include "Common/GBuffer.frag"
#include "Common/ShaderModelDefines.frag"

void main()
{
	b0 = PackGBuffer(
		FS_Color.rgb, 
		1.f,
		vec3(0),
		SHADERMODEL_DEBUG,
		vec3(0.f, 1.f, 0.f),
		1.f,
		1.f,
		0.f);
}
