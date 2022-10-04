#pragma once
#include "FrameBuffer.h"
#include "Texture.h"
#include "shader.h"
#include "GBuffer.h"
#include "Fog.h"
#include "Include/glm/glm.hpp"

struct LightPassBindings
{
	int shadowMaps;
	int volumetricLightmap;
	int cloudColorTex;
	int cloudShadowVLTex;

	int SSAOmap;

	int indirectDiffuseAO;
	int indirectSpecularShadow;

	int fTime;

	GBufferBindings gBuf;
};

struct LightPass
{
	FrameBuffer fbo;
	Texture color;
	Texture brightColor;
	Shader shader;
};

