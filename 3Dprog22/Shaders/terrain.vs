#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec2 aTex1Coords;
layout (location = 5) in vec2 aTex2Coords;

out vec3 Normal;
out vec3 FragPos;
out vec4 Color;
out vec2 TexCoords;
out vec2 Tex1Coords;
out vec2 Tex2Coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * vec4(FragPos, 1.0);
	
	Color = aColor;
	TexCoords = aTexCoords;
	Tex1Coords = aTex1Coords;
	Tex2Coords = aTex2Coords;
}