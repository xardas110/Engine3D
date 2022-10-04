#pragma once
#include "Texture.h"
#include "Material.h"
#include "shader.h"
#include "Transform.h"

/// <summary>
/// Internal variables, ecs system will contain color, transform etc.
/// No ownership of VAO
/// </summary>
struct Mesh
{
	unsigned vao{0};
	unsigned vbo{ 0 };
	unsigned indices{0};

	bool bIsTransparent{ false };
	bool bIsHighlighted{ false };
	bool bWireframe{ false };
	
	MaterialTextured material;
	Transform transform;

	//std::vector<glm::vec3> mVertices;

	[[nodiscard]]
	bool IsValid() const
	{
		return vao != 0;
	}

	void AddTexture(Texture& texture)
	{
		switch (texture.type)
		{
			case Texture::Types::Diffuse:
			{
				material.diffusemap = texture;
			}
			break;
			case Texture::Types::Specular:
			{
				material.specularmap = texture;
			}
			break;
			case Texture::Types::Ambient:
			{
				material.ambientmap = texture;
			}
			break;
			case Texture::Types::Height:
			{
				material.heightmap = texture;
			}
			break;
			case Texture::Types::Emissive:
			{
				material.emissivemap = texture;
			}
			break;
			case Texture::Types::Normals:
			{
				material.normalmap = texture;
			}
			break;
			case Texture::Types::Shininess:
			{
				material.shininessmap = texture;
			}
			break;
			case Texture::Types::Opacity:
			{
				material.opacitymap = texture;
			}
			break;
			case Texture::Types::Displacement:
			{
				material.displacementmap = texture;
			}
			break;
			case Texture::Types::LightMap:
			{
				material.lightmap = texture;
			}
			break;
			case Texture::Types::Reflection:
			{
				material.reflectionmap = texture;
			}
			break;
		default:
			break;
		}
	}

	void RemoveTexture(Texture::Types type)
	{
		switch (type)
		{
		case Texture::Types::Diffuse:
		{
			material.diffusemap = Texture();
		}
		break;
		case Texture::Types::Specular:
		{
			material.specularmap = Texture();
		}
		break;
		case Texture::Types::Ambient:
		{
			material.ambientmap = Texture();
		}
		break;
		case Texture::Types::Height:
		{
			material.heightmap = Texture();
		}
		break;
		case Texture::Types::Emissive:
		{
			material.emissivemap = Texture();
		}
		break;
		case Texture::Types::Normals:
		{
			material.normalmap = Texture();
		}
		break;
		case Texture::Types::Shininess:
		{
			material.shininessmap = Texture();
		}
		break;
		case Texture::Types::Opacity:
		{
			material.opacitymap = Texture();
		}
		break;
		case Texture::Types::Displacement:
		{
			material.displacementmap = Texture();
		}
		break;
		case Texture::Types::LightMap:
		{
			material.lightmap = Texture();
		}
		break;
		case Texture::Types::Reflection:
		{
			material.reflectionmap = Texture();
		}
		break;
		default:
			break;
		}
	}

	bool operator==(Mesh& b) const
	{
		if (!this->IsValid() || !b.IsValid())
			return false;

		if (this->vao == b.vao)
			return true;

		return false;
	}
	
	void Delete()
	{
		vao = 0; // it has no ownership of VAO
		indices = 0;
		material = MaterialTextured();
	}
};

struct MeshAlphaPass
{
	Shader parentShader;
	glm::mat4 parentTransform;
	Mesh mesh;
};