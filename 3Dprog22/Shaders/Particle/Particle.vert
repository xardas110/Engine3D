#version 460 core

layout (location = 0) in vec2 Position0;
layout (location = 1) in vec2 TexCoords0;

#define MAX_PARTICLES 30000

out vec2 TexCoords_FS;
out vec4 ParticleColor_FS;

uniform mat4 projection;

struct Data
{
	vec3 pos;
	float lifetime;
	vec3 vel;
	float scale;
	vec4 color;
};

layout (std430, binding=2) buffer particleData
{
	Data data[MAX_PARTICLES]; 
};

void main()
{
	Data cd = data[gl_InstanceID];
	TexCoords_FS = TexCoords0;
	ParticleColor_FS = cd.color;

	mat4 posM = mat4(1.f);
	posM[3] = vec4(cd.pos, 1.f);

	gl_Position = projection * posM * vec4(Position0, 0.f, 1.f);
}