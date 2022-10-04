#pragma once
#include "Texture.h"
#include "shader.h"
#include "FrameBuffer.h"
#include "UBO.h"
#include "qopenglfunctions_4_5_core.h"
#include "Include/glm/glm.hpp"
#include "Mesh.h"
#include "GBuffer.h"
#include "FrameBuffer.h"

/// <summary>
///Based on:
///  https://research.nvidia.com/sites/default/files/pubs/2011-09_Interactive-Indirect-Illumination/GIVoxels-pg2011-authors.pdf
/// -Octree differes, this implementation uses opengl's built in mipmapping to create the octree representation, the paper uses a pointer structure.
/// -The paper builds the structure from the lights view, this implementation does it in ws
/// </summary>

struct VoxelGPUData
{
	glm::vec4 gridCenter{ 0.f, 0.f,0.f,0.f };
	glm::vec4 gridSize{ 512.f };
	const glm::ivec4 dims{ 512 };
	glm::mat4 projX, projY, projZ;
};

struct VoxelBindings
{
	int albedo;
	int numAlbedo;
	int model;
	int shadowmaps;
};

struct MipMipsBindings
{
	int mipLevel;
	int source[6];
};

struct AnisoMipBindings
{
	int source;
};

struct VoxelRadianceBindings
{
	int shadowMaps;
};

struct VoxelPropInjectBindings
{
	int voxelAlbedo;
	int voxelNormal;
	int voxelMips[6];
};

struct VoxelStructure
{
	Texture texture; //fbo texture contains the structure	
	Texture albedo;
	Texture normal;
	std::array<Texture, 6> mips; //6 aniso mip mips for quality!

	Shader shader;
	Shader radianceShader;
	Shader firstBounceShader;
	Shader mipShader;
	Shader anisoMipShader;
	Shader anisoMipMipsShader;
	Shader clearShader;

	const int numBaseLods = 0; //max 4 -  min 0
	const int numMipLods = 8;

	UBO ubo;
	VoxelGPUData gpuData;
	VoxelBindings vb;
	MipMipsBindings mmb;
	AnisoMipBindings amb;
	VoxelRadianceBindings vrb;
	VoxelPropInjectBindings vpib;
};

struct VoxelDisplayBindings
{
	int model;
	int voxelStructure;
};

struct VoxelDisplay
{
	Shader shader;
	VoxelDisplayBindings vdb;
};

struct IndirectLight
{
	Texture diffuseAO;
	Texture diffuseAOFiltered;
	Texture specularShadow;
	Texture specularShadowFiltered;
};

struct VCTBindings
{
	int gBufferB0;
	int depth;
	int voxelStructure;
	int mips[6];
};

struct VCT
{
	IndirectLight il;

	Shader shader;
	FrameBuffer fbo;

	VCTBindings bind;
	float downScale = 1.;
};

class VoxelConeTracing : protected QOpenGLFunctions_4_5_Core
{
	friend class DeferredRenderer;

	void Init(class World* world);
	void InitVoxel();
	void InitVoxelDisplay();
	void InitVCT();
	
	void Update(class World* world, float deltatime);
	void UpdateVoxelUBO();

	void BindVoxelStructure(int lodLevel = 0);
	void UnbindVoxelStructure();

	void GenerateMipMap();
	void GenerateLinearMips(); // Cheaper than aniso
	void GenerateAnisoMips();
	void GenerateAnisoMipMips();
	void GenerateAllAnisoMips();

	void InjectDirectLight();
	void InjectFirstBounce();

	void RenderVoxelStructure(const Mesh& screenQuad);
	void TraceCones(const GBuffer& gbuffer, const Mesh& screenQuad);

	void OnResize(int width, int height);

	void ClearVoxel();
	void Clean();

private:
	VCT vct;
	VoxelDisplay voxelDisplay2;
	VoxelStructure voxelStructure;
};

