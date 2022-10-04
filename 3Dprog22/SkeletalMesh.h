#pragma once
#include "Include/Assimp/matrix4x4.h"
#include "Include/Assimp/mesh.h"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/Importer.hpp"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/postprocess.h"
#include <map>
#include <vector>
#include "Mesh.h"
#include <string>
#include <unordered_map>

#include "Transform.h"

struct SkeletalMeshBoneInfo
{
    aiMatrix4x4 boneOffset{};
    aiMatrix4x4 finalTransform{};
};

struct SkeletalMeshNodeTransform
{
   /// <summary>
   /// Saving the only transform data i need for now from the aiScene
   /// </summary>

   std::string name;
   aiMatrix4x4 transform;
   std::vector<SkeletalMeshNodeTransform> children;
};

struct AnimationChannel
{
    std::string mChannelName;
	
    unsigned mNumScalingKeys{ 0 };
    unsigned mNumPositionKeys{0};
    unsigned mNumRotationKeys{ 0 };

    std::vector<aiVectorKey> mPositionKeys; //TODO:Fix for caching
    std::vector<aiVectorKey> mScalingKeys;
    std::vector<aiQuatKey> mRotationKeys;
};

struct Animation
{
    std::string path;
	
    float mDuration{0.f};
    float mTicksPrSecond{ 0.f };
    float mSpeed{ 1.f };
    float mCurrentTime{ 0.f };
	
    unsigned numChannels{ 0 };
	
    std::map<std::string, int> mAnimationNodeName; /*Maps string to index, do cache tests later*/
    std::vector<AnimationChannel> mChannels; /*Copy of all the channels for an animation*/
};

class SkeletalMesh
{
    friend class RenderSystem;
    friend class SkeletalMeshManager;
    friend class SkeletalMeshSystem;
    friend class World;
	
    using BoneMap = std::map<std::string, unsigned>;
    using BoneInfoVector = std::vector<SkeletalMeshBoneInfo>;
    using Meshes = std::vector<Mesh>;
    using Animations = std::map<std::string, Animation>;

    SkeletalMeshNodeTransform mRoot; //Gets all the transforms for the model

	//Will do performance testing later to check if these maps lead to cache misses
    BoneMap boneMap;/*Bone name to index*/
    BoneInfoVector boneInfoVec;/*Bone transforms*/
    Meshes meshes; /*All meshes for the model*/
    Animations animations /*All animations*/;
    //AnimationNodeName animationNodeName;

    std::string pathDirectory;

    aiMatrix4x4 globalInverseTransform{};
	
    int numBones{ 0};
    bool bHasTransparency{ false };

    std::string currentPlayingAnimation{ "none" };
	
	/*Inserts animation by name and path, to avoid duplicates and for serialization*/
    void InsertAnimation(const std::string& name, const std::string& path, Animation& animation);
	
    bool Exists(const std::string& name);
	
	/*Populates the boneinfo struct */
    void PopulateBoneInfo(float animationTime, Animation& animation, SkeletalMeshNodeTransform& nodeTransforms, const aiMatrix4x4& parentTransform);
	
    void GetBoneTransforms(std::vector<glm::mat4>& transforms); // for rendersystem

    void UpdateAnimation(float deltaTime);
public:
    glm::mat4 GetLocalTransform();
	
    Transform transform;

	/*Loads only a single animation, more than 1 animation in a file is not supported right now.*/
    bool LoadAnimation(const std::string& name, const std::string& path);

    void PlayAnimation(const std::string& name, const float speed);

	/*Resets animation to start position*/
    void ResetAnimation(const std::string& name);
	
    Meshes& GetMeshes();
};

