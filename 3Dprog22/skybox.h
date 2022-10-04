#ifndef SKYBOX_H
#define SKYBOX_H

#include "Texture.h"
#include "Mesh.h"

/// <summary>
/// This is an internal struct, a world should never contain more than 1 skybox
/// </summary>
struct Skybox
{
	friend class World;
	friend class RenderSystem;
	friend class SkySystem;
private:
   Skybox(const Mesh& skyboxMesh, const Texture& skyboxCubemap);

   Mesh mesh;
   Texture texture;

public:
	
	Skybox() = delete;
	
	Skybox(const Skybox&) = delete;
	Skybox(Skybox&&) = delete;

	Skybox& operator=(const Skybox&) = delete;
	Skybox& operator=(Skybox&&) = delete;

	~Skybox() = default;
};

#endif // SKYBOX_H
