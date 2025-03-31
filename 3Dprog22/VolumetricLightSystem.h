#pragma once
#include "VolumetricLighting.h"
#include "Texture.h"
#include "Mesh.h"
#include "DeferredRendererConfig.h"

class ABuffer;

class VolumetricLightSystem
{
	friend class DeferredRenderer;

	void Init();
	void InitShader();
	void InitShaderBindings();
	void InitFBOAndTextures();

	/*Posbuffer replaced by depth buffer*/
	void Process(class World* world, const Texture& posBuffer, const Mesh& quad, const DeferredRendererConfig& config, const ABuffer* aBuffer);
	void Clean();

	void OnResize(unsigned width, unsigned height);

private:
	int downSample = 4;
	VolumetricLighting volumetricLight;
};

