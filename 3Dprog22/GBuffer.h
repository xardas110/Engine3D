#pragma once
#include "Texture.h"
#include "FrameBuffer.h"

struct GBufferBindings
{
	int b0;
	int depth;
};

struct GBuffer
{
	Texture b0;
	/*FBO texture contains the depth texture*/
	FrameBuffer fbo;
};

