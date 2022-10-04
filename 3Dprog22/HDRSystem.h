#pragma once
#include "HDR.h"
#include "shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "DeferredRendererConfig.h"

class HDRSystem
{
	friend class DeferredRenderer;
	void Init();
	void InitShader();
	void InitShaderBindings();
	void Render(const Texture& lightPassTexture, const Texture& bloomTexture, const Mesh& mesh, const DeferredRendererConfig& config);
	void Clean();
private:
	Shader hdrShader;
	HDRBindings hdrb;
};

