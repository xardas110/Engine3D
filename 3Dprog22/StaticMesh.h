#pragma once
#include <vertex.h>
#include "Mesh.h"
#include <vector>

#include "Transform.h"
#include "Include/Assimp/matrix4x4.h"

/// <summary>
/// No ownership
/// </summary>
struct StaticMesh
{
	friend class StaticMeshManager;
	aiMatrix4x4 globalInverseTransform{};
	std::string pathDirectory{};

	int shaderModel = 0;

	void AddMesh(const Mesh& mesh);
	bool RemoveMesh(Mesh& mesh); // for caching I have to manually delete

	/*Gets a copy of existing meshes*/ //Im too lazy to make an iterator :)
	std::vector<Mesh>& GetMeshes(); //TODO: make const reference

	/*Sets color for all the meshes*/
	void SetColor(const glm::vec3& newColor);

	Transform transform;

	bool IsValid() const;

	unsigned cullFace = 0x0404;

	bool bCastShadow = true;

private:
	std::vector<Mesh> meshes;
};

