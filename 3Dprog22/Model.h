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

struct ModelBoneInfo
{
    aiMatrix4x4 boneOffset{};
    aiMatrix4x4 finalTransform{};
};

struct NodeTransforms
{
   /// <summary>
   /// Saving the only transform data i need for now from the aiScene
   /// </summary>

   std::string name;
   aiMatrix4x4 transform;
   std::vector<NodeTransforms> children;
};

struct AnimationChannel
{
    std::string mChannelName;
	
    unsigned mNumScalingKeys{ 0 };
    unsigned mNumPositionKeys{0};
    unsigned mNumRotationKeys{ 0 };

    std::vector<aiVectorKey> mPositionKeys;
    std::vector<aiVectorKey> mScalingKeys;
    std::vector<aiQuatKey> mRotationKeys;
};

struct Animation
{
    std::string name;
    float mDuration{0.f};
    float mTicksPrSecond{ 0.f };
	
    unsigned numChannels{ 0 };
	
    std::map<std::string, int> mAnimationNodeName; /*Maps string to index*/
    std::vector<AnimationChannel> mChannels; /*Copy of all the channels for an animation*/
};

class Model
{
    friend class RenderSystem;
	
    using BoneMap = std::map<std::string, unsigned>;
    using BoneInfoVector = std::vector<ModelBoneInfo>;
    using Meshes = std::vector<Mesh>;
    using Animations = std::vector<Animation>;

    NodeTransforms mRoot; //Gets all the transforms for the model
	
    BoneMap boneMap;/*Bone name to index*/
    BoneInfoVector boneInfoVec;/*Bone transforms*/
    Meshes meshes; /*All meshes for the model*/
    Animations animations /*All animations*/;
    //AnimationNodeName animationNodeName;

    std::string pathDirectory;
	
    aiMatrix4x4 globalInverseTransform{};
	
    int numBones{ 0};
    
	/// <summary>
	/// The model should have no ownership of meshes and textures
	/// It only owns the bone transforms and animations
	/// </summary>
	void LoadMesh(aiMesh* mesh, const aiScene* scene, const std::string& path, class World* world);

    void PopulateBoneInfo(float animationTime, Animation& animation, NodeTransforms& nodeTransforms, const aiMatrix4x4& parentTransform);
    void GetBoneTransforms(float timeInSeconds, Animation& animation, std::vector<aiMatrix4x4>& transforms);
public:
    bool LoadModel(const std::string& path, class World* world);
    bool LoadAnimation(const std::string& path, class World* world);
};

