#include "Model.h"
#include <iostream>
#include <vector>
#include "Texture.h"
#include "Mesh.h"
#include <vertex.h>
#include "AnimationHelper.h"
#include "World.h"

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene, const std::string& path, World* world)
{
	std::vector<ModelVertex> vertices;
	std::vector<unsigned> indices;
	std::vector<Texture> textures;

	for (auto i = 0; i < mesh->mNumVertices; i++)
	{
		ModelVertex vertex;

		{	//Position and normals
			for (auto j = 0; j < 3; j++)
			{
				vertex.pos[j] = mesh->mVertices[i][j];
				vertex.normal[j] = mesh->mNormals[i][j];
			}
		}
		{	//Texture coords
			if (mesh->mTextureCoords[0])
			{
				for (auto j = 0; j < 2; j++)
				{
					vertex.texCord[j] = mesh->mTextureCoords[0][i][j];
				}
			}
		}
		{	//Tangents
			for (auto j = 0; j < 3; j++)
			{
				vertex.tangent[j] = mesh->mTangents[i][j];
				vertex.bitTangent[j] = mesh->mBitangents[i][j];
			}
		}
		vertices.emplace_back(vertex);
	}
	{	//Indices
		for (auto i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (auto j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
	}
	{	//Bonedata
		for (auto i = 0; i < mesh->mNumBones; i++)
		{
			int boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.C_Str());

			if (boneMap.find(boneName) == boneMap.end())
			{
				ModelBoneInfo bi;
				boneIndex = numBones;
				numBones++;			
				boneInfoVec.push_back(bi);
			}
			else 
			{
				boneIndex = boneMap[boneName];
			}
			
			boneInfoVec[boneIndex].boneOffset = mesh->mBones[i]->mOffsetMatrix;
			boneMap[boneName] = boneIndex;
			
			for (auto j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				auto vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				vertices[vertexId].AddBoneData(boneIndex, weight);
			}
		}								
	}
	
	{	//loading materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		for (auto i = 1; i< (int)aiTextureType::aiTextureType_UNKNOWN; i++)
		{
			aiTextureType type = aiTextureType(i);
			for (auto j = 0; j < material->GetTextureCount(type); j++)
			{
				std::cout << "Texture types " << Texture::GetTypeString((Texture::Types)type);
				aiString texturePath;
				material->GetTexture(type, j, &texturePath);
				std::string txtPath(texturePath.C_Str());
				const std::string fullPath = (pathDirectory +'/'+ txtPath);
				Texture texture;
				if (world->textureManager->LoadTexture(fullPath.c_str(), texture, (Texture::Types)i))
				{
					textures.push_back(texture);
				}
				else
				{
					printf("Failed to load texture: %s", fullPath.c_str());
				}
			}
		}
		//TODO:Make better system
		Texture texture;
		const std::string fullPath = (pathDirectory + '/' + "textures/Nightshade_normal.png");
		if (world->textureManager->LoadTexture(fullPath.c_str(), texture, Texture::Normals))
		{
			textures.push_back(texture);
		}
		else
		{
			printf("Failed to load texture: %s", fullPath.c_str());
		}
	}
	
	{	// Use mesh manager class to upload into GPU
		Mesh inMesh; std::string meshName = mesh->mName.C_Str();
		if (!world->meshManager->UploadModelMesh(meshName, vertices, indices, textures, inMesh))
		{
			printf("Failed to upload mesh: %s", meshName.c_str());
		}
		meshes.emplace_back(inMesh);
	}
}

void Model::PopulateBoneInfo(float animationTime, Animation& animation, NodeTransforms& nodeTransforms, const aiMatrix4x4& parentTransform)
{
	auto nodeName = nodeTransforms.name;
	aiMatrix4x4 NodeTransformation(nodeTransforms.transform);
	
	if (animation.mAnimationNodeName.find(nodeName) != animation.mAnimationNodeName.end())
	{
		unsigned index = animation.mAnimationNodeName[nodeName];
		const AnimationChannel pNodeAnim = animation.mChannels[index];
		aiVector3D scaling;
		aiVector3D position;
		aiQuaternion rotation;
		
		aiMatrix4x4 scalingM;
		aiMatrix4x4 positionM;
		aiMatrix4x4 rotationM;
		scalingM.Identity();
		positionM.Identity();
		
		AnimationHelper::CalculateInterpolatedScaling(animationTime, pNodeAnim, scaling);

		AnimationHelper::CalculateInterpolatedRotation(animationTime, pNodeAnim, rotation);

		rotationM = aiMatrix4x4(rotation.GetMatrix());
		
		AnimationHelper::CalculateInterpolatedPosition(animationTime, pNodeAnim, position);
		positionM.a4 = position.x;
		positionM.b4 = position.y;
		positionM.c4 = position.z;
		positionM.d4 = 1.f;
		
		scalingM.a1 = scaling.x;
		scalingM.b2 = scaling.y;
		scalingM.c3 = scaling.z;
		scalingM.d4 = 1.f;
				
		NodeTransformation = positionM * rotationM * scalingM;
	}

	aiMatrix4x4 GlobalTransformation = parentTransform * NodeTransformation;
	
	if (boneMap.find(nodeName) != boneMap.end())
	{
		unsigned int BoneIndex = boneMap[nodeName];	//right hand system	//modelspace		//ModelSpace->BoneSpace
		boneInfoVec[BoneIndex].finalTransform = globalInverseTransform * GlobalTransformation* boneInfoVec[BoneIndex].boneOffset;			
	}

	for (auto i = 0; i< nodeTransforms.children.size(); i++)
	{
		PopulateBoneInfo(animationTime, animation, nodeTransforms.children[i], GlobalTransformation);
	}
	
}

void Model::GetBoneTransforms(float timeInSeconds, Animation& animation, std::vector<aiMatrix4x4>& transforms)
{
	aiMatrix4x4 globalTransformation;
	globalTransformation.Identity();
	
	const float ticksPerSec = !Cmp((float)animation.mTicksPrSecond, 0.f) ? (float)animation.mTicksPrSecond : 25.f;
	const float timeInTicks = timeInSeconds * ticksPerSec;
	const float animationTime = fmod(timeInTicks, (float)animation.mDuration);

	PopulateBoneInfo(animationTime, animation, mRoot, globalTransformation);

	transforms.resize(boneInfoVec.size());

	for (auto i= 0; i < boneInfoVec.size(); i++)
	{
		transforms[i] = boneInfoVec[i].finalTransform;
	}
}

bool Model::LoadModel(const std::string& path, World* world)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "Assimp failed to load " << importer.GetErrorString() << std::endl;
		return false;
	}
	std::string directory = path.substr(0, path.find_last_of('/'));

	globalInverseTransform = scene->mRootNode->mTransformation;
	globalInverseTransform.Inverse();	
	pathDirectory = path.substr(0, path.find_last_of('/'));
	
	std::function<void(aiNode*, const aiScene*, NodeTransforms&)> ProcessNode =
		[&ProcessNode, this, &path, world](aiNode* node, const aiScene* scene, NodeTransforms& nodeTransforms)
	{	
		nodeTransforms.name = node->mName.C_Str();
		nodeTransforms.transform = node->mTransformation;
			
		for (auto i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			LoadMesh(mesh, scene, path, world);
		}
		for (auto i = 0; i < node->mNumChildren; i++)
		{
			NodeTransforms newNode;
			nodeTransforms.children.push_back(newNode);
			ProcessNode(node->mChildren[i], scene, nodeTransforms.children[i]);
		}
	};

	ProcessNode(scene->mRootNode, scene, mRoot);
	return true;
}

bool Model::LoadAnimation(const std::string& path, World* world)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene) 
	{
		printf ("Failed to load animation: %s", importer.GetErrorString());
		return false;
	}

	if (!scene->HasAnimations())
	{
		printf("File has no animations: %s", path.c_str());
		return false;
	}
	
	for (auto i = 0; i < scene->mNumAnimations; i++)
	{
		auto* currentAnim = scene->mAnimations[i];
		
		Animation animStruct;
		animStruct.mDuration = (float)currentAnim->mDuration;
		animStruct.mTicksPrSecond = (float)currentAnim->mTicksPerSecond;
		animStruct.name = currentAnim->mName.C_Str();
		for (auto j = 0 ; j< currentAnim->mNumChannels; j++)
		{
			auto* currentChannel = currentAnim->mChannels[j];
			AnimationChannel animationChannel;
			animationChannel.mNumScalingKeys = currentChannel->mNumScalingKeys;
			animationChannel.mNumPositionKeys = currentChannel->mNumPositionKeys;
			animationChannel.mNumRotationKeys = currentChannel->mNumRotationKeys;
			
			{ //f to all the work needed to load a model with animations :)
				for (auto f = 0; f< currentChannel->mNumScalingKeys; f++)
				{
					animationChannel.mScalingKeys.push_back(currentChannel->mScalingKeys[f]);
				}
			}
			{
				for (auto f = 0; f < currentChannel->mNumPositionKeys; f++)
				{
					animationChannel.mPositionKeys.push_back(currentChannel->mPositionKeys[f]);
				}
			} {
				for (auto f = 0; f < currentChannel->mNumRotationKeys; f++)
				{
					animationChannel.mRotationKeys.push_back(currentChannel->mRotationKeys[f]);
				}
			}
			
			animStruct.mAnimationNodeName[scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str()] = j;
			animStruct.mChannels.push_back(animationChannel);
			
			//animStruct.mChannels.push_back(*scene->mAnimations[i]->mChannels[j]);
		}	
		animations.push_back(animStruct);
	}
	
	return true;
}
