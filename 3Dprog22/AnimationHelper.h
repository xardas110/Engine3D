#pragma once

#include "SkeletalMesh.h"
#include "Include/Assimp/matrix4x4.h"
#include "Include/Assimp/mesh.h"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/Importer.hpp"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/postprocess.h"

struct AnimationHelper
{
	static void InterpolateScale(const float animationTime, const AnimationChannel& animNode, aiVector3D& outVec);
	static void InterpolatePosition(const float animationTime, const AnimationChannel& animNode, aiVector3D& outVec);
	static void InterpolateRotation(const float animationTime, const AnimationChannel& animNode, aiQuaternion& outQuat);
};

