#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include <vector>
#include <string>
#include <memory>
#include <QOpenGLFunctions_4_5_Core>
#include "Texture.h"
#include "Include/Assimp/matrix4x4.h"
#include "Include/Assimp/mesh.h"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/Importer.hpp"
#include "Include/Assimp/scene.h"
#include "Include/Assimp/postprocess.h"

class TextureManager : protected QOpenGLFunctions_4_5_Core
{
	friend class World;
	friend class MeshManager;
	friend class MaterialManager;
	friend class StaticMeshManager;
	friend class SkeletalMeshManager;
	friend class RenderEngine;
	
	using TextureMap = std::unordered_map<std::string, Texture>;
	using CubeTextureMap = std::map<std::string, CubemapDescriptor>;
	
	TextureMap textureMap;
	CubeTextureMap cubeTextureMap;
	
	TextureManager() = default;
	
	/// <summary>
	/// Does not work with QtGL use constructor instead.
	/// </summary>
	static std::unique_ptr<TextureManager> Create();
	
	bool InsertIntoCubeTextureMap(const CubemapDescriptor& descriptor);
	bool InsertIntoTextureMap(const std::string& path, const Texture& texture);

public:

	~TextureManager() override;
	
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;

	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager& operator= (TextureManager&&) = delete;

    bool LoadCubemapTextures(CubemapDescriptor& descriptor, Texture &outTexture);

	bool LoadTexture(const std::string& texturePath, Texture &outTexture, Texture::Types type = Texture::Types::Diffuse, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR, bool bFlip = false);

	bool LoadTextureTerrain(const std::string& texturePath, Texture& outTexture, Texture::Types type = Texture::Types::Diffuse, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR, bool bFlip = false);

	bool GenerateTexture(const std::string& name, Texture& outTexture);

	bool LoadHeightmapBuffer(const Texture& texture, float* outData);

	bool LoadSlopeBuffer(const Texture& texture, float* outData);

	bool LoadNormalBuffer(const Texture& texture, float* outData);

	bool CreateTextureStorage(Texture& outTexture, int x, int y, Texture::Types type = Texture::Types::Normals, int minFilter = GL_LINEAR, int magFilter = GL_LINEAR, int format = GL_RGBA32F);

	bool LoadEmbeddedTexture(aiTexture* texture, const std::string& texturePath, Texture& outTexture, Texture::Types type = Texture::Types::Diffuse);

	bool CreateLazTexture(const std::string& lazPath, Texture& outTexture);

	bool DeleteTexture(const std::string& name);

	/// <summary>
	/// Checks if a cubemap with the same texture paths exists
	/// </summary>
	bool HasCubemap(const CubemapDescriptor& descriptor);

	/// <summary>
	/// Checks if a cubemap with the same tag exists
	/// </summary>
	bool HasCubemap(const std::string& tag);

	/// <summary>
	/// Gets a cubemap by tag, if it exists
	/// </summary>
	/// <param name="outCubemapDescriptor">A populated CubemapDescriptor if the function succeeds</param>
	/// <returns>true if success, false otherwise</returns>
	bool GetCubemap(const std::string& tag, Texture& outCubemapTexture);

	/// <summary>
	/// Searches the BTN for a cubemap that has the same descriptor
	/// </summary>
	/// <param name="inDescriptor">The descriptor to search for</param>
	/// <param name="outDescriptor">The populated descriptor if the function succeeds</param>
	/// <returns>
	/// true -  if success
	/// false - if fails
	/// </returns>
	bool GetCubemap(const CubemapDescriptor& inDescriptor, Texture& outTexture);

	bool HasTexture(const std::string& texturePath);

	bool GetTexture(const std::string& texturePath, Texture& outTexture);
};

#endif // TEXTUREMANAGER_H
