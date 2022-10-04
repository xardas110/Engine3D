#pragma once
#include <map>
#include <string>
#include "shader.h"
#include <QOpenGLFunctions_4_5_Core>
#include "FrameBufferManager.h"
#include "UBOManager.h"
#include "Include/glm/fwd.hpp"

class ShaderManager : protected QOpenGLFunctions_4_5_Core
{
public:
	/// <summary>
	/// Temp
	/// </summary>
	enum ShaderType
	{
		Null,
		Standard,
		StandardTextured,
		Voxel,
		VoxelMip,
		VoxelAnisoMip,
		VoxelAnisoMipMips,
		VoxelRadiance,
		VoxelFirstBounce,
		VoxelClear,
		VoxelFilter,
		VoxelDisplay2,
		VoxelConeTrace,
		Skybox,
		Debug,
		SkeletalMesh,
		DirectionalLightDepthPass,
		DirectionalLightDepthPassSM,
		Terrain,
		TerrainNormalCompute,
		TerrainSplatCompute,
		Sun,
		LensFlare,
		ForwardInstanced,
		ForwardInstancedSM,
		Vegetation,
		VegetationSM,
		Particle,
		DeferredStandard,
		DeferredLightPass,
		HDR,
		SkyDome,
		VolumetricLight,
		DitheringFilter,
		Bilateral,
		SMAA,
		BilateralCompute,
		SMAACompute,
		Fog,
		TerrainShadowGrid,
		TerrainShadowMap,
		VegetationSG,
		SSAO,
		Blur,
		GaussianBlur,
		DoF,
		Text,
		PerlinWorley3D,
		Worley3D,
		Weather,
		VolumetricClouds,
		WBClearCompute,
		DefaultDebug,
		Water,
		Size
	};

private:
	friend class World;
	friend class RenderEngine;
	friend class RenderSystem;
	
	using ShaderMap = std::map<ShaderType, Shader>;
	using UniformMap = std::unordered_map<std::string, int>;

	ShaderMap shaderMap;

	bool StripIncludeLine(std::string& inOutline);
	bool LoadIncludeString(const std::string& path, std::string& outString);

	const bool LoadInclude(const std::string& relativePath, std::string& code, std::vector<std::string>& pragmaOnce);

	void LoadIncludes(const std::string& relativePath, std::string& code, std::vector<std::string>& pragmaOnce);

	GLuint LoadShaderType(unsigned type, const std::string& path);

	//I can make smaller functions for these, but too lazy rn
	Shader LoadShader(ShaderType shaderType, const std::string& vertexPath, const std::string& fragmentPath);
	Shader LoadShader(ShaderType shaderType, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geoPath);
	Shader LoadShader(ShaderType shaderType, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geoPath, const std::string& tesePath, const std::string& tescPath);

	/*Maybe a nono, but as far as I know, you have to specify a vertex and frament shader, so this is ok*/
	Shader LoadShader(ShaderType shaderType, const std::string& computePath);

	unsigned CreateGeomShader(const char* path, unsigned shaderID);

	FrameBufferManager	FBOManager;
	UBOManager			uboManager;

public:
	ShaderManager();

	//ShaderManager(const ShaderManager&) = delete;
	//ShaderManager(ShaderManager&&) = delete;

	//ShaderManager& operator=(const ShaderManager&) = delete;
	//ShaderManager& operator=(ShaderManager&&) = delete;

	~ShaderManager() override;
	
	bool HasShader(ShaderType type);

	bool GetShader(ShaderType type, Shader& outShader);

	bool FindShaderById(unsigned shaderId, Shader& outShader);

	void BindMat4(unsigned shaderId, const std::string& name, glm::mat4 mat);
	void BindVec4(unsigned shaderId, const std::string& name, glm::vec4 vec);
	void BindVec3(unsigned shaderId, const std::string& name, glm::vec3 vec);
	void BindVec2(unsigned shaderId, const std::string& name, glm::vec3 vec);
	void BindInt(unsigned shaderId, const std::string& name, int id);
	void BindFloat(unsigned shaderId, const std::string& name, float id);

	void BindMat4(const Shader& shader, const std::string& name, glm::mat4 mat);
	void BindVec4(const Shader& shader, const std::string& name, glm::vec4 vec);
	void BindVec3(const Shader& shader, const std::string& name, glm::vec3 vec);
	void BindVec2(const Shader& shader, const std::string& name, glm::vec3 vec);

	void BindVec2f(const Shader& shader, const std::string& name, glm::vec2 vec);
	void BindVec2i(const Shader& shader, const std::string& name, glm::vec2 vec);

	void BindInt(const Shader& shader, const std::string& name, int id);
	void BindFloat(const Shader& shader, const std::string& name, float id);

	/*Faster alternatives glgetuniform is expensive*/
	int GetUniformLocation(const Shader& shader, const std::string& uniformName);

	void SetMat4(int bindingId, const glm::mat4& mat);
	void SetVec4(int bindingId, const glm::vec4& vec);
	void SetVec3(int bindingId, const glm::vec3& vec);
	void SetVec2(int bindingId, const glm::vec2& vec);

	void SetInt(int bindingId, const int value);
	void SetFloat(int bindingId, const float value);

	void Use(unsigned shaderId);
	void Use(const Shader& shader);
};

