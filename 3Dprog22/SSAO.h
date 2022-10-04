#pragma once
#include "shader.h"
#include "FrameBuffer.h"
#include "GlobalTypeDefs.h"
#include "Texture.h"

struct SSAOBindings
{
	int gBufferNormal;
	int gBufferPos;
	int gBufferDepth;

	int noisemap;
	int dim;

	int radius;
	int bias;

	int kernel[SSAO_KERNEL_SIZE];
};

struct SSAO
{
	Shader shader;
	Texture noiseTex;
	FrameBuffer fbo;//fbo contains final SSAO image
	Texture blur;
	Texture upscaledColor;
	std::array<glm::vec3, SSAO_NOISE_SIZE> noise;
	std::array<glm::vec3, SSAO_KERNEL_SIZE> kernel;
};

