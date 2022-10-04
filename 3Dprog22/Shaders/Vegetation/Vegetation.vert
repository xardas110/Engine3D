#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitTangent;

#include "../common/Camera.frag"

#define MAX_INSTANCES 300000

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform int instanceOffset;

layout (std430, binding=1) buffer instancedMatrices
{
	mat4 instanceMat[MAX_INSTANCES];
};

void main()
{

	mat4 model = instanceMat[instanceOffset + gl_InstanceID];

	FragPos = vec3(model * vec4(aPos, 1.f));

	mat3 model3 = mat3(transpose(inverse(model)));

	TexCoords = aTexCoords;
	
	Normal = normalize(model3 * aNormal); //if no normal map is specified
	vec3 tangent = normalize(vec3(model3 * aTangent));
	vec3 bitangent = normalize(vec3(model3 * aBitTangent));

	TBN = mat3(bitangent, Normal, tangent);

	gl_Position = VP * model * vec4(aPos, 1.f);	

}