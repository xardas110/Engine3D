#version 460 core

layout (location = 0) in vec2 pos0;

uniform mat4 worldMatrix;
uniform mat4 localMatrix;
uniform sampler2D heightmap;

out vec2 texCoords_TC;

void main()
{
	//Localmatrix without LOD scaling
	vec2 localPos = (localMatrix * vec4(pos0.x, 0.f, pos0.y, 1.f)).xz;

	float height = texture(heightmap, localPos).r;

	vec4 worldPos = worldMatrix * vec4(localPos.x, height, localPos.y, 1.f);

	texCoords_TC = localPos;

	gl_Position = worldPos;	
}