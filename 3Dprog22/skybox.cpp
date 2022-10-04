#include "skybox.h"

Skybox::Skybox(const Mesh& skyboxMesh, const Texture& skyboxCubemap)
	:mesh(skyboxMesh), texture(skyboxCubemap)
{
}
