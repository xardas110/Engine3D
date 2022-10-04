#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include <texturemanager.h>
#include "Mesh.h"
#include "StaticMesh.h"
#include "vertex.h"
#include "ThreadPoolx.h"
#include <vector>
#include <mutex>
#include <atomic>
#include "Include/Assimp/Exporter.hpp"
#include "ConvexHull.h"

#define SMM_CONTEXTS 8

struct AsyncMeshInfo
{
	std::string path;
	std::vector<StaticMeshVertex> vertices;
	std::vector<unsigned> indices;
	std::vector<std::pair<std::string, int>> texturePaths;
};

struct MeshData
{
	std::vector<glm::vec3> verts;
	std::vector<unsigned> indices;
};

namespace MeshCollision
{ 
	enum MeshCollision
	{
		None,
		KDOP,
		ConvexHull,
		Size
	};
}

/// <summary>
/// Ownership:
/// * Meshes
/// </summary>
class StaticMeshManager : protected QOpenGLFunctions_4_5_Core
{
	using StaticMeshMap = std::unordered_map<std::string, StaticMesh>;
	using MeshDataMap = std::unordered_map<std::string, MeshData>;
	using StaticMeshHulls = std::unordered_map<std::string, ConvexHull>;

	StaticMeshMap staticMeshMap; std::mutex smmMutex;
	MeshDataMap meshDataMap;
	StaticMeshHulls convexHullMap;

	TextureManager textureManager;

	unsigned int aiFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_FlipUVs;

	bool Insert(const std::string& path, StaticMesh& sMesh);
	bool InsertMeshDataMap(const std::string& path, MeshData& meshData);
	bool InsertHullMap(const std::string& path, ConvexHull& hull);
	bool HullExists(const std::string& path);
	bool Exists(const std::string& path);

	StaticMesh Get(const std::string& path);
	
	bool UploadStaticMesh(const std::string& name, std::vector<StaticMeshVertex>& vertices, std::vector<unsigned>& indices, std::vector<Texture>& textures, Mesh& outMesh);
	bool LoadMesh(class aiMesh* mesh, const class aiScene* scene, const std::string& path, StaticMesh& staticMeshInstanced, MeshData& outMeshData);

	//Threading -------------------------------------------------------------
	ThreadPoolx* tp{nullptr};	
	std::atomic<int> tpIndex{0};
	std::vector<std::string> tpPaths;
	Assimp::Importer importer[SMM_CONTEXTS];
	Assimp::Exporter exporter[SMM_CONTEXTS];
	std::vector<AsyncMeshInfo> asyncMeshInfo[SMM_CONTEXTS];

	void ClearAsyncMeshInfo();
	bool LoadStaticMeshAsync(const std::string& path, StaticMesh& outStaticMesh, int threadIndex);
	bool LoadMeshAsync(class aiMesh* mesh, const class aiScene* scene, const std::string& path, StaticMesh& outStaticMesh, int threadIndex);
	void UploadStaticMeshAsync();
	//Threading -------------------------------------------------------------
public:
	bool LoadStaticMesh(const std::string& path, StaticMesh& outStaticMesh, bool bCreateConvexHull = false, bool bSaveMeshData = false);
	bool LoadStaticMeshesAsync(std::vector<std::string>& paths);

	const MeshData* GetMeshData(const std::string& path);
	ConvexHull&		GetConvexHull(const std::string& path);

	void WorkerThread(int index);

	StaticMeshManager();
	~StaticMeshManager() override;
};