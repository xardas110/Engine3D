#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitTangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];
uniform vec2 scale;

void main()
{
	mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
	BoneTransform += gBones[aBoneIDs[1]] * aWeights[1];
	BoneTransform += gBones[aBoneIDs[2]] * aWeights[2];
	BoneTransform += gBones[aBoneIDs[3]] * aWeights[3];

	vec4 PosL = vec4(aPos, 1.0) * BoneTransform;
	vec4 newPos = vec4(aPos, 1.0) * BoneTransform;
	FragPos = vec3(model * newPos);
	vec3 NormalL = aNormal * vec3(BoneTransform);
	Normal = mat3(transpose(inverse(model))) * NormalL;

	gl_Position = projection * view * model * newPos;
	
	TexCoords = aTexCoords;
}