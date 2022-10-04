#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <texturemanager.h>
#include "SkeletalMesh.h"
#include "vertex.h"

/// <summary>
/// Ownership:
/// * Meshes(meshes inside each skeletalMesh)
/// </summary>
class SkeletalMeshManager : protected QOpenGLFunctions_4_5_Core
{
	using SkeletalMeshMap = std::unordered_map<std::string, SkeletalMesh>;
	SkeletalMeshMap skeletalMeshMap;

	TextureManager textureManager;

	bool Exists(const std::string& path);
	/*Adds a skeletal mesh to the map*/
	bool Insert(const std::string& path, SkeletalMesh& skm);

	bool UploadSkeletalMesh(const std::string& name, std::vector<SkeletalMeshVertex>& vertices, std::vector<unsigned>& indices, std::vector<Texture>& textures, Mesh& outMesh);
	bool LoadMesh(class aiMesh* mesh, const class aiScene* scene, const std::string& path, SkeletalMesh& skeletalMesh);
public:

	bool LoadSkeletalMesh(const std::string& path, SkeletalMesh& outSkeletalMesh);

	~SkeletalMeshManager();
};

