#version 460 core
layout (location = 0) in vec3 pos; 
layout (location = 1) in vec2 texCoords;

#include "../../common/Camera.frag"

uniform mat4 model;

out vec3 WorldPos_FS;
out vec2 TexCoords_FS;

void main()
{
	TexCoords_FS = texCoords;

	mat4 newView = mat4(mat3(view));

	mat4 MVP = proj * newView * model;

	WorldPos_FS = (model * vec4(pos, 1.f)).xyz;
    gl_Position = (MVP * vec4(pos, 1.0)).xyww;
}  