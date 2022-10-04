#pragma once
#include "ShaderManager.h"
#include <QOpenGLFunctions_4_5_Core>
#include "shader.h"
#include "UBO.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "SSBOManager.h"
#include "FrameBufferManager.h"
#include "texturemanager.h"
#include "ACBO.h"
#include "Include/glm/glm.hpp"

/*Singleton class*/
class RenderEngine : protected QOpenGLFunctions_4_5_Core
{
	friend class TerrainSystem;
public:
	RenderEngine();
	~RenderEngine();

	void BindShader(const Shader& shader);
	void BindTexture(const Texture& texture, int textureType, const std::string& uniformName, int textureSlot);

	void BindInt(const std::string& uniformName, int value);
	void BindFloat(const std::string& uniformName, float value);

	//Should really pass by reference. 
	void BindVec2(const std::string& uniformName, glm::vec2 value);
	void BindVec3(const std::string& uniformName, glm::vec3 value);
	void BindVec4(const std::string& uniformName, glm::vec4 value);
	void BindMat4(const std::string& uniformName, glm::mat4 value);

	int GetUniformLocation(const std::string& uniformName);

	void SetInt(int bindingId, int value);
	void SetFloat(int bindingId, float value);
	void SetVec2(int bindingId, const glm::vec2& value);
	void SetVec3(int bindingId, const glm::vec3& value);
	void SetVec4(int bindingId, const glm::vec4& value);

	void SetMat4(int bindingId, const glm::mat4& value);

	void SetTexture(const Texture& texture, int textureType, const int textureBindingId, int textureSlot);

	void DrawArrays(int drawType, int vao, int first, int count);

	void EnableWireframe();
	void DisableWireframe();

	void BindImageTexture(int bindingId, const Texture& texture, int lodLevel, bool layered, int layer, int access, int format);

	void SetCulling(int mode);

	/*Waits for finish too*/
	void DispatchCompute(int numX, int numY, int numZ);

	bool GetShader(ShaderManager::ShaderType type, Shader& outShader);

	void BindUBO(UBO ubo);

	void BindUBOData(unsigned offsetInBytes, unsigned sizeInBytes, void* data);

	void BindSSBO(SSBO ssbo);

	void BindSSBOData(unsigned offsetInBytes, unsigned sizeInBytes, void* data);

	void BindACBO(ACBO acbo, unsigned index);

	void EnableDepthTest();
	void DisableDepthTest();

	void EnableCulling();
	void DisableCulling();

	void EnableBlending();
	void DisableBlending();

	void EnableStencil();
	void DisableStencil();

	void EnableSRGB();
	void DisableSRGB();

	void SetStencilFunc(unsigned func, unsigned ref,unsigned mask);
	
	void SetStencilOp(unsigned fail, unsigned zFail, unsigned zPass);

	void SetBlendFunc(int mode, int mode1);

	void SetBlendFunc(int drawBuffer, int mode, int mode1);

	void SetBlendEquation(int mode);

	void SetStencilMask(unsigned mask);

	void SetDepthMask(int mode);
	void SetCullFace(int mode);

	void SetDepthFunc(int mode);

	void BindFrameBuffer(int id);
	void BindFrameBuffer(FrameBuffer& fbo);
	/*Generates FBO*/
	bool GenerateFBO(const std::string& name, FrameBuffer& outFBO);
	/*Generates a texture*/
	bool GenerateTexture(const std::string& name, Texture& outTexture);
	void Bind2DTexture(const Texture& texture);
	void Bind2DBilinearFilter();
	void Bind2DNoFilter();
	void Bind2DClampToEdge();
	void Bind2DWrapRepeat();

	void SetMemoryBarrier(unsigned mode);


	bool DeleteFBO(const std::string& name);
	bool DeleteTexture(const std::string& name);

	/*Creates or changes a texture from the bound texture*/
	bool CreateTexture2D(Texture& outTexture, unsigned internalFormat, unsigned format, unsigned width, unsigned height, unsigned type, unsigned border = 0, unsigned level = 0, const void* pixels = nullptr);

	/*Creates or changes a texture from the bound texture*/
	bool CreateImage2D(Texture& outTexture, unsigned internalFormat, unsigned format, unsigned width, unsigned height, unsigned type, unsigned border = 0, unsigned level = 0, const void* pixels = nullptr);

	void BindFrameBufferTexture2D(int attachment, int textureType, const Texture& texture, int level);

	void SetViewport(int x, int y, int width, int height);

	void ClearDepthBuffer();

	void ClearBuffer(int bufferType, int renderTarget, const float* val);

	void GLClear(int bit);

	void DrawElements(const Mesh& mesh, int mode);

	void DrawElementsBaseInstanced(const Mesh& mesh, unsigned mode, unsigned amount, unsigned baseInstance);

	void DrawElementsInstanced(const Mesh& mesh, int mode, int amount);

	static RenderEngine* Get();
	ShaderManager* GetShaderManager();
	UBOManager* GetUBOManager();
	SSBOManager* GetSSBOManager();
	FrameBufferManager* GetFBOManager();
	TextureManager* GetTextureManager();
private:
	Shader boundShader;	

	UBO boundUBO;
	SSBO boundSSBO;
	ACBO boundACBO;

	FrameBuffer boundFBO;
	Texture boundTexture;

	SSBOManager ssboManager;
	ShaderManager shaderManager;	
	TextureManager textureManager;
};

