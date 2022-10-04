#include "SkeletalMesh.h"
#include <iostream>
#include <vector>
#include "Texture.h"
#include "Mesh.h"
#include <vertex.h>
#include "AnimationHelper.h"
#include "World.h"

bool SkeletalMesh::LoadAnimation(const std::string& name, const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene)
	{
		printf("Failed to load animation: %s", importer.GetErrorString());
		return false;
	}

	if (!scene->HasAnimations())
	{
		printf("File has no animations: %s", path.c_str());
		return false;
	}

	auto* currentAnim = scene->mAnimations[0];

	Animation animStruct;
	animStruct.mDuration = (float)currentAnim->mDuration;
	animStruct.mTicksPrSecond = (float)currentAnim->mTicksPerSecond;

	for (auto j = 0; j < currentAnim->mNumChannels; j++)
	{
		auto* currentChannel = currentAnim->mChannels[j];
		
		AnimationChannel animationChannel;
		animationChannel.mNumScalingKeys = currentChannel->mNumScalingKeys;
		animationChannel.mNumPositionKeys = currentChannel->mNumPositionKeys;
		animationChannel.mNumRotationKeys = currentChannel->mNumRotationKeys;

		for (auto f = 0; f < currentChannel->mNumScalingKeys; f++)
		{
			animationChannel.mScalingKeys.push_back(currentChannel->mScalingKeys[f]);
		}

		for (auto f = 0; f < currentChannel->mNumPositionKeys; f++)
		{
			animationChannel.mPositionKeys.push_back(currentChannel->mPositionKeys[f]);
		}

		for (auto f = 0; f < currentChannel->mNumRotationKeys; f++)
		{				
			animationChannel.mRotationKeys.push_back(currentChannel->mRotationKeys[f]);
		}

		animStruct.mAnimationNodeName[currentAnim->mChannels[j]->mNodeName.C_Str()] = j;
		animStruct.mChannels.push_back(animationChannel);

		//animStruct.mChannels.push_back(*scene->mAnimations[i]->mChannels[j]);
	}
	
	InsertAnimation(name, path, animStruct);
	return true;
}

void SkeletalMesh::PopulateBoneInfo(float animationTime, Animation& animation, SkeletalMeshNodeTransform& nodeTransforms, const aiMatrix4x4& parentTransform)
{
	auto nodeName = nodeTransforms.name;
	aiMatrix4x4 nodeTransform(nodeTransforms.transform);
	
	if (animation.mAnimationNodeName.find(nodeName) != animation.mAnimationNodeName.end())
	{
		unsigned index = animation.mAnimationNodeName[nodeName];
		
		const AnimationChannel animationNode = animation.mChannels[index];
		
		aiVector3D s; aiVector3D p; aiQuaternion r; aiMatrix4x4 S; aiMatrix4x4 P; aiMatrix4x4 R;
		P.Identity(); S.Identity(); R.Identity();
		
		AnimationHelper::InterpolateScale(animationTime, animationNode, s);
		AnimationHelper::InterpolateRotation(animationTime, animationNode, r);
		AnimationHelper::InterpolatePosition(animationTime, animationNode, p);
		
		R = aiMatrix4x4(r.GetMatrix());

		for(auto i = 0; i< 3; i++)
		{
			P[i][3] = p[i];
			S[i][3] = s[i];
		}
			
		nodeTransform = P * R * S;
	}

	aiMatrix4x4 globalTransform = parentTransform * nodeTransform;
	
	if (boneMap.find(nodeName) != boneMap.end())
	{
		unsigned int BoneIndex = boneMap[nodeName];	//right hand system	//modelspace		//ModelSpace->BoneSpace
		boneInfoVec[BoneIndex].finalTransform = globalInverseTransform * globalTransform* boneInfoVec[BoneIndex].boneOffset;			
	}

	for (auto i = 0; i< nodeTransforms.children.size(); i++)
	{
		PopulateBoneInfo(animationTime, animation, nodeTransforms.children[i], globalTransform);
	}
	
}

void SkeletalMesh::UpdateAnimation(float deltaTime)
{
	aiMatrix4x4 globalTransformation;
	globalTransformation.Identity();

	auto& currentAnimation = animations[currentPlayingAnimation];
	auto& currentTime = currentAnimation.mCurrentTime += deltaTime;
	auto& animationSpeed = currentAnimation.mSpeed;

	const float ticksPerSec = (!Cmp((float)currentAnimation.mTicksPrSecond, 0.f) ? (float)currentAnimation.mTicksPrSecond : 25.f) * animationSpeed;
	const float timeInTicks = currentTime * ticksPerSec;
	const float animationTime = fmod(timeInTicks, (float)currentAnimation.mDuration);

	PopulateBoneInfo(animationTime, currentAnimation, mRoot, globalTransformation);
}

glm::mat4 SkeletalMesh::GetLocalTransform()
{
	glm::mat4 result(1.f);
	result[0] = glm::vec4(globalInverseTransform[0][0], globalInverseTransform[1][0], globalInverseTransform[2][0], globalInverseTransform[3][0]);
	result[1] = glm::vec4(globalInverseTransform[0][1], globalInverseTransform[1][1], globalInverseTransform[2][1], globalInverseTransform[3][1]);
	result[2] = glm::vec4(globalInverseTransform[0][2], globalInverseTransform[1][2], globalInverseTransform[2][2], globalInverseTransform[3][2]);
	result[3] = glm::vec4(globalInverseTransform[0][3], globalInverseTransform[1][3], globalInverseTransform[2][3], globalInverseTransform[3][3]);
	return glm::inverse(result);
}

void SkeletalMesh::GetBoneTransforms(std::vector<glm::mat4>& transforms)
{
	transforms.resize(boneInfoVec.size());

	for (auto i= 0; i < boneInfoVec.size(); i++)
	{
		transforms[i][0] = glm::vec4(boneInfoVec[i].finalTransform[0][0], boneInfoVec[i].finalTransform[1][0], boneInfoVec[i].finalTransform[2][0], boneInfoVec[i].finalTransform[3][0]);
		transforms[i][1] = glm::vec4(boneInfoVec[i].finalTransform[0][1], boneInfoVec[i].finalTransform[1][1], boneInfoVec[i].finalTransform[2][1], boneInfoVec[i].finalTransform[3][1]);
		transforms[i][2] = glm::vec4(boneInfoVec[i].finalTransform[0][2], boneInfoVec[i].finalTransform[1][2], boneInfoVec[i].finalTransform[2][2], boneInfoVec[i].finalTransform[3][2]);
		transforms[i][3] = glm::vec4(boneInfoVec[i].finalTransform[0][3], boneInfoVec[i].finalTransform[1][3], boneInfoVec[i].finalTransform[2][3], boneInfoVec[i].finalTransform[3][3]);
	}
}

void SkeletalMesh::InsertAnimation(const std::string& name, const std::string& path, Animation& animation)
{
	assert(!Exists(name) && "Animation already exists!");
	
	animation.path = path;
	animations[name] = animation;
}

bool SkeletalMesh::Exists(const std::string& name)
{
	return animations.find(name) != animations.end();
}

void SkeletalMesh::PlayAnimation(const std::string& name, const float speed)
{
	if (!Exists(name)) return;

	auto& animation = animations[name];
	animation.mSpeed = speed;
	currentPlayingAnimation = name;
}

void SkeletalMesh::ResetAnimation(const std::string& name)
{
	if (!Exists(name)) return;

	auto& animation = animations[name];
	animation.mCurrentTime = 0.f;
}

SkeletalMesh::Meshes& SkeletalMesh::GetMeshes()
{
	return meshes;
}

