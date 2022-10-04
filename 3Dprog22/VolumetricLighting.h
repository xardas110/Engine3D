#pragma once
#include "shader.h"
#include "FrameBuffer.h"
#include "Texture.h"

struct VolumetricBindings
{
	int smCascades;
	int depthBuffer;
	int numSteps;
	int scattering;
	int sunColor;
};

struct VolumetricLighting
{
	Shader shader;
	FrameBuffer fbo;
	Texture filteredTexture;
	Texture upScaledTexture;

	VolumetricBindings bind;
};



