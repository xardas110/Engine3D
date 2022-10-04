#include "SkeletalMeshManager.h"
#include "Include/glm/ext/quaternion_geometric.hpp"
#include "Include/Assimp/matrix4x4.h"
#include "Include/Assimp/mesh.h"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/Importer.hpp"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/postprocess.h"

bool SkeletalMeshManager::LoadSkeletalMesh(const std::string& path, SkeletalMesh& outSkeletalMesh)
{
    if (Exists(path))
    {
        outSkeletalMesh = skeletalMeshMap[path];
        return true;
    }
	
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "Assimp failed to load " << importer.GetErrorString() << std::endl;
        return false;
    }
    std::string directory = path.substr(0, path.find_last_of('/'));

    outSkeletalMesh.globalInverseTransform = scene->mRootNode->mTransformation;
    outSkeletalMesh.globalInverseTransform.Inverse();
    outSkeletalMesh.pathDirectory = path.substr(0, path.find_last_of('/'));

    std::function<void(aiNode*, const aiScene*, SkeletalMeshNodeTransform&)> ProcessNode =
        [&ProcessNode, this, &path, &outSkeletalMesh](aiNode* node, const aiScene* scene, SkeletalMeshNodeTransform& nodeTransforms)
    {
        nodeTransforms.name = node->mName.C_Str();
        nodeTransforms.transform = node->mTransformation;

        for (auto i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
           if (!LoadMesh(mesh, scene, path, outSkeletalMesh))
           {
               std::cout << "Failed to load mesh: " << path << std::endl;
           }

        }
        for (auto i = 0; i < node->mNumChildren; i++)
        {
            SkeletalMeshNodeTransform newNode;
            nodeTransforms.children.push_back(newNode);
            ProcessNode(node->mChildren[i], scene, nodeTransforms.children[i]);
        }
    };

    ProcessNode(scene->mRootNode, scene, outSkeletalMesh.mRoot);

    Insert(path, outSkeletalMesh);
    return true;
}

bool SkeletalMeshManager::Exists(const std::string& path)
{
    return skeletalMeshMap.find(path) != skeletalMeshMap.end();
}

bool SkeletalMeshManager::Insert(const std::string& path, SkeletalMesh& skm)
{
    assert(!Exists(path) && "Skeletal mesh path already inserted! Failed to insert!");
    skeletalMeshMap[path] = skm;
    return true;
}

bool SkeletalMeshManager::LoadMesh(aiMesh* mesh, const aiScene* scene, const std::string& path, SkeletalMesh& outSkeletalMesh)
{
    std::vector<SkeletalMeshVertex> vertices;
    std::vector<unsigned> indices;
    std::vector<Texture> textures;

    for (auto i = 0; i < mesh->mNumVertices; i++)
    {
        SkeletalMeshVertex vertex;

        //Position and normals
        for (auto j = 0; j < 3; j++)
        {
            vertex.pos[j] = mesh->mVertices[i][j];
            vertex.normal[j] = mesh->mNormals[i][j];
        }
        
        //Texture coords
        if (mesh->mTextureCoords[0])
        {
            for (auto j = 0; j < 2; j++)
            {
                vertex.texCord[j] = mesh->mTextureCoords[0][i][j];
            }
        }
        
        //Tangents
        for (auto j = 0; j < 3; j++)
        {
            vertex.tangent[j] = mesh->mTangents[i][j];
            vertex.bitTangent[j] = mesh->mBitangents[i][j];
        }
        
        vertices.emplace_back(vertex);
    }
    //Indices
    for (auto i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (auto j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    //Bonedata
    for (auto i = 0; i < mesh->mNumBones; i++)
    {
        int boneIndex = 0;
        std::string boneName(mesh->mBones[i]->mName.C_Str());

        if (outSkeletalMesh.boneMap.find(boneName) == outSkeletalMesh.boneMap.end())
        {
            SkeletalMeshBoneInfo bi;
            boneIndex = outSkeletalMesh.numBones;
            outSkeletalMesh.numBones++;
            outSkeletalMesh.boneInfoVec.push_back(bi);
        }
        else
        {
            boneIndex = outSkeletalMesh.boneMap[boneName];
        }

        outSkeletalMesh.boneInfoVec[boneIndex].boneOffset = mesh->mBones[i]->mOffsetMatrix;
        outSkeletalMesh.boneMap[boneName] = boneIndex;

        for (auto j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            auto vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            vertices[vertexId].AddBoneData(boneIndex, weight);
        }
    }
    

    bool bIsTransparent{ false };	
    {	//loading materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        for (auto i = 1; i < (int)aiTextureType::aiTextureType_UNKNOWN; i++)
        {
            aiTextureType type = aiTextureType(i);
            for (auto j = 0; j < material->GetTextureCount(type); j++)
            {
               // std::cout << "Texture types " << Texture::GetTypeString((Texture::Types)type);
                aiString texturePath;
                material->GetTexture(type, j, &texturePath);
                std::string txtPath(texturePath.C_Str());
                const std::string fullPath = (outSkeletalMesh.pathDirectory + '/' + txtPath);
                Texture texture;
                if (textureManager.LoadTexture(fullPath.c_str(), texture, (Texture::Types)i))
                {
                    textures.push_back(texture);
                	if (type == aiTextureType_OPACITY)
                	{
                        bIsTransparent = true;
                	}
                }
                else
                {
                    printf("Failed to load texture: %s", fullPath.c_str());
                }
            }
        }
    }
    {	// Use mesh manager class to upload into GPU
        Mesh inMesh; std::string meshName = mesh->mName.C_Str();
        if (!UploadSkeletalMesh(meshName, vertices, indices, textures, inMesh))
        {
            printf("Failed to upload mesh: %s", meshName.c_str());
            return false;
        }
        inMesh.bIsTransparent = bIsTransparent;

    	//Just a check if bHasTransparency was set before to true or not by other meshes
    	if (!outSkeletalMesh.bHasTransparency)
			outSkeletalMesh.bHasTransparency = bIsTransparent;
    	
        outSkeletalMesh.meshes.emplace_back(inMesh);
    }

    return true;
}

bool SkeletalMeshManager::UploadSkeletalMesh(const std::string& name, std::vector<SkeletalMeshVertex>& vertices, std::vector<unsigned>& indices, std::vector<Texture>& textures, Mesh& outMesh)
{
    initializeOpenGLFunctions();

    unsigned VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkeletalMeshVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertex), (void*)offsetof(SkeletalMeshVertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertex), (void*)offsetof(SkeletalMeshVertex, texCord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertex), (void*)offsetof(SkeletalMeshVertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertex), (void*)offsetof(SkeletalMeshVertex, bitTangent));

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(SkeletalMeshVertex), (void*)offsetof(SkeletalMeshVertex, id));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertex), (void*)offsetof(SkeletalMeshVertex, weight));

    glBindVertexArray(0);

    outMesh.vao = VAO;
    outMesh.indices = indices.size();

    for (auto& texture : textures)
        outMesh.AddTexture(texture);

    return true;
}

SkeletalMeshManager::~SkeletalMeshManager()
{
	//The texture manager will clean the textures.
	for (auto [name, skeletalMesh] : skeletalMeshMap)
	{
		for (auto& mesh : skeletalMesh.meshes)
		{
            glDeleteVertexArrays(1, &mesh.vao);
		}
	}
}
