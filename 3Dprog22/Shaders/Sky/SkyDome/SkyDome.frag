#version 460 core
layout(location = 0) out uvec4 b0;

#include "../../Common/GBuffer.frag"

in vec3 WorldPos_FS;
in vec2 TexCoords_FS;

uniform vec3 botColor = vec3(0.2f, 0.33f, 0.4f);
uniform vec3 topColor = vec3(.4, .741, .819);
uniform vec3 cloudColor = vec3(1.f, 1.f, 1.f);

uniform sampler2D skydomeTexture;
uniform sampler2D perlinTex;

#define PI 3.14159265

void main()
{
	const float domeDistance = 5000.f;
	float y = WorldPos_FS.y;
	vec3 color = mix(botColor, topColor, y);

	b0 = PackGBuffer(
		color, 
		1.f,
		vec3(0),
		1,
		vec3(0.f, 1.f, 0.f),
		1.f,
		1.f,
		0.f);
}
