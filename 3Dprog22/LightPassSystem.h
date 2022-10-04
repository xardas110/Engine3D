#pragma once

#include "shader.h"
#include "Mesh.h"
#include "LightPass.h"
#include "Texture.h"
#include "GBuffer.h"
#include "ABuffer.h"
#include "DeferredRendererConfig.h"
#include "VolumetricClouds.h"
#include "qopenglfunctions_4_5_core.h"
#include "VoxelConeTracing.h"

class LightPassSystem : protected QOpenGLFunctions_4_5_Core
{
	friend class DeferredRenderer;

	void Init();
	void InitShader();
	void InitShaderBindings();
	void InitLightPass();

	void Render(
		class World* world, 
		const GBuffer& gBuffer, 
		const ABuffer* aBuffer, 
		const Texture& volumetricLightmap, 
		const Texture& SSAOmap, const Mesh& quad, 
		const IndirectLight& indirectLight,
		const VolumetricClouds& vc);

	void OnResize(int width, int height);
	void OnClear();
	void Clean();
private:
	LightPass lp;
	LightPassBindings lpb;

	Fog fog;
};

