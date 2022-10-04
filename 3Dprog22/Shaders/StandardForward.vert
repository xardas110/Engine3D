#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitTangent;

#include "common/Camera.frag"

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;

void main()
{

	FragPos = vec3(model * vec4(aPos, 1.f));
	Normal = mat3(transpose(inverse(model))) * aNormal; //if no normal map is specified
	TexCoords = aTexCoords;

	vec3 tangent = normalize(vec3(model * vec4(aTangent, 0.f)));	
	tangent = normalize(tangent - dot(tangent, Normal) * Normal);
	vec3 bitTangent = cross(Normal, tangent);
	TBN = mat3(tangent, bitTangent, Normal);
	
	mat4 MVP = VP * model;

	gl_Position = MVP * vec4(aPos, 1.f);	

}