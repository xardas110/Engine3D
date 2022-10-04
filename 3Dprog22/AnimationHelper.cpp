#include "AnimationHelper.h"

#include <cassert>

void AnimationHelper::InterpolateScale(const float animationTime, const AnimationChannel& animNode, aiVector3D& outVec)
{
	if (animNode.mNumScalingKeys == 1) { outVec = animNode.mScalingKeys[0].mValue; return; }

	unsigned currentIndex{0u};

	for (auto i = 0; i < animNode.mNumScalingKeys -1 ; i++)
	{
		if (animationTime < (float)animNode.mScalingKeys[i+1].mTime)
		{
			currentIndex = i;
			break;
		}
	}

	const unsigned nextIndex = currentIndex + 1;

	auto& currentScalingKey = animNode.mScalingKeys[currentIndex];
	auto& nextScalingKey = animNode.mScalingKeys[nextIndex];
	
	const float delta = (float)nextScalingKey.mTime - (float)currentScalingKey.mTime;
	const float t = (animationTime - (float)currentScalingKey.mTime) / delta;
		
	const auto start = currentScalingKey.mValue;
	const auto end = nextScalingKey.mValue;
	const auto se = end - start;
	outVec = start + (se * t);
}

void AnimationHelper::InterpolatePosition(const float animationTime, const AnimationChannel& animNode, aiVector3D& outVec)
{
	if (animNode.mNumPositionKeys == 1){ outVec = animNode.mPositionKeys[0].mValue; return;	}

	unsigned currentIndex{ 0u };

	for (auto i = 0; i < animNode.mNumPositionKeys - 1; i++)
	{
		if (animationTime < (float)animNode.mPositionKeys[i + 1].mTime)
		{
			currentIndex = i;
			break;
		}
	}

	const unsigned nextIndex = currentIndex + 1;
	auto& currentPositionKey = animNode.mPositionKeys[currentIndex];
	auto& nextPositionKey = animNode.mPositionKeys[nextIndex];

	const float delta = (float)nextPositionKey.mTime - (float)currentPositionKey.mTime;
	const float t = (animationTime - (float)currentPositionKey.mTime) / delta;
	
	const auto start = currentPositionKey.mValue;
	const auto end = nextPositionKey.mValue;
	const auto se = end - start;
	outVec = start + (se * t);
}

void AnimationHelper::InterpolateRotation(const float animationTime, const AnimationChannel& animNode, aiQuaternion& outQuat)
{
	if (animNode.mNumRotationKeys == 1) {outQuat = animNode.mRotationKeys[0].mValue; return;	}

	unsigned currentIndex{ 0u };

	for (auto i = 0; i < animNode.mNumRotationKeys - 1; i++)
	{
		if (animationTime < (float)animNode.mRotationKeys[i + 1].mTime)
		{
			currentIndex = i;
			break;
		}
	}

	const unsigned nextIndex = currentIndex + 1;
	auto& currentRotationKey = animNode.mRotationKeys[currentIndex];
	auto& nextRotationKey = animNode.mRotationKeys[nextIndex];

	const float delta = (float)nextRotationKey.mTime - (float)currentRotationKey.mTime;
	const float t = (animationTime - (float)currentRotationKey.mTime) / delta;
	const auto start = currentRotationKey.mValue;
	const auto end = nextRotationKey.mValue;
	
	aiQuaternion::Interpolate(outQuat, start, end, t);
	outQuat = outQuat.Normalize();
}
