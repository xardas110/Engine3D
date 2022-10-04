#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include "Mesh.h"
#include "vertex.h"
#include "SkeletalMesh.h"
#include <vector>
#include <unordered_map>

namespace MeshType
{
	enum Type
	{
		Mesh,
		Size
	};
}

class MeshManager : protected QOpenGLFunctions_4_5_Core
{
	friend class World;
	friend class RenderSystem;
	friend class TextureManager;
	
	using MeshMap = std::unordered_map<std::string, Mesh>;
	MeshMap meshContainer[MeshType::Size];
	
	MeshManager();
	
	/// <summary>
	/// Creation of unique meshes that only the mesh manger class has access to
	/// </summary>
	void CreateBox();

	void CreateTriangle();

	void CreatePlane();

	void CreatePoint();
	
	void CreateSkybox();

	void CreateSegment();

	void CreateDebugBoundingBox();

	/*It is scaled as a screen quad*/
	void CreateQuad(float s = 1.f);

	void CreateUnitQuad(float s = 0.5f);

	void CreateTerrainPatch();

	/// ------------------------------------------------------------------------

	/// <summary>
	/// Default names for meshMap search, TODO: make enum with string returns instead
	/// </summary>
	const std::string boxString = "box";
	const std::string sphereString = "sphere";
	const std::string skyboxString = "skybox";
	const std::string boundingBoxString = "boundingBox";
	const std::string triangleString = "triangle";
	const std::string segmentString = "segment";
	const std::string planeString = "plane";
	const std::string pointString = "point";
	const std::string quadString = "quad";
	const std::string unitQuad = "unitQuad";

	const std::string terrainPatchString = "terrainPatch";

	void InsertMesh(const std::string& name, Mesh& mesh, MeshType::Type type);
	
	bool HasMesh(const std::string& name, MeshType::Type type);
	
	bool GetMesh(const std::string& name, Mesh& outMesh, MeshType::Type type);

public:
	
	MeshManager(const MeshManager& copy) = delete;
	MeshManager(MeshManager&&) = delete;

	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;

	~MeshManager() override;
	
	Mesh GetSegmentMesh();
	Mesh GetTriangleMesh();
	Mesh GetBoxMesh();
	Mesh GetPlaneMesh();
	Mesh GetPointMesh();
	Mesh GetTerrainPatchMesh();
	Mesh GetQuadMesh();
	Mesh GetUnitQuad();

	Mesh CreateOctahedronBall(int n);

	bool GetSkyboxMesh(Mesh& outMesh);
};

