#pragma once
#include "DoF.h"
#include "Texture.h"
#include "Mesh.h"
#include "DeferredRendererConfig.h"

class DoFSystem
{
	friend class DeferredRenderer;

	void Init();
	void InitShader();
	void InitShaderBindings();
	void InitFBO();
	void InitKernel();
	void UploadKernel();

	void Process(class World* world, const Texture& colorTexture, const Texture& depthTexture, const Mesh& quad, const DeferredRendererConfig& config);

	void OnResize(int width, int height);
	void Clean();
private:
	DoF dof;
	DoFBindings db;
};

