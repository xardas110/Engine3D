#pragma once
#include "shader.h"
#include "Texture.h"
#include "FrameBuffer.h"

struct BilatreralBindings
{
	int source, targetDim;
};

struct Bilatreral
{
	Shader shader;
	FrameBuffer	fbo;
	BilatreralBindings bind;
};

