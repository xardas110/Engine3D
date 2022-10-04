#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


#include "../common/Camera.frag"

out vec3 Normal;
out vec4 ClipCoords;
out vec2 TexCoords;
out vec4 Position;

uniform mat4 model;

void main()
{
	TexCoords = aTexCoords;
	Normal = aNormal;
	Position = model * vec4(aPos, 1.f);
	ClipCoords = VP * Position;
	gl_Position = ClipCoords;	

}