#pragma once
#include "shader.h"
#include "Texture.h"
#include "FrameBuffer.h"

struct SMAABindings
{
	int source, targetDim;
};

struct SMAA
{
	Shader shader;
	SMAABindings bind;
	FrameBuffer fbo;
};

