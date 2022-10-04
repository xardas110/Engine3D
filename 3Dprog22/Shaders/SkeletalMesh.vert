#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitTangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

#include "common/Camera.frag"

const int MAX_BONES = 100;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 gBones[MAX_BONES];
uniform vec2 scale;

void main()
{
	mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
	BoneTransform += gBones[aBoneIDs[1]] * aWeights[1];
	BoneTransform += gBones[aBoneIDs[2]] * aWeights[2];
	BoneTransform += gBones[aBoneIDs[3]] * aWeights[3];

	vec4 posBoneTransformed = BoneTransform * vec4(aPos, 1.0);
	vec3 normalBoneTransformed = mat3(transpose(inverse(BoneTransform))) * aNormal;

	Normal = (mat3(transpose(inverse(model))) * normalBoneTransformed); // for some reason I had to flip world up

	vec3 tangent = normalize(vec3(model * vec4(aTangent, 0.f)));
	
	tangent = normalize(tangent - dot(tangent, Normal) * Normal);

	vec3 bitTangent = cross(Normal, tangent);

	TBN = mat3(tangent, bitTangent, Normal);

	mat4 MVP = VP * model;

	gl_Position = MVP * posBoneTransformed;
	FragPos = vec3(model * posBoneTransformed);
	TexCoords = aTexCoords;
}