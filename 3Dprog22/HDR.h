#pragma once
#include "FrameBuffer.h"
#include "shader.h"
#include "Texture.h"

struct HDRBindings
{
	int colorTexture;
	int bloomTexture;
	int bloomEnabled;
};
