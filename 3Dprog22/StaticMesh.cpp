#include "StaticMesh.h"

void StaticMesh::AddMesh(const Mesh& mesh)
{
	meshes.emplace_back(mesh);
}

bool StaticMesh::RemoveMesh(Mesh& mesh)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i] == mesh)
		{
			meshes[i].Delete();
			meshes.erase(meshes.begin() + i);
			return true;
		}
		
	}
	
	return false;
}

std::vector<Mesh>& StaticMesh::GetMeshes()
{
	return meshes;
}

void StaticMesh::SetColor(const glm::vec3& newColor)
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto& mesh = meshes[i];
		mesh.material.color = newColor;
	}
}

bool StaticMesh::IsValid() const
{

	if (meshes.empty()) return false;


	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto& mesh = meshes[i];
		if (mesh.IsValid())
		{
			return true;
		}
	}

	return false;
}
