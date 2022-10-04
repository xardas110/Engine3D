#pragma once
#include "shader.h"
#include "FrameBuffer.h"
#include "GlobalTypeDefs.h"

struct GaussianBlurBindings
{
	int source;
	int bHorizontal;
	int n;
	int weights[GB_WEIGHTS_SIZE];
};

struct GaussianBlur
{
	Shader shader;
	FrameBuffer fbo[2];
};

