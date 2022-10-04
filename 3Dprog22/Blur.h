#pragma once
#include "shader.h"
#include "FrameBuffer.h"

struct Blur
{
	FrameBuffer fbo;
	Shader shader;
};

struct BlurBindings
{
	int source;
	int targetDim;
};
