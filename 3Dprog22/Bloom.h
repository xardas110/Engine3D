#pragma once
#include "Texture.h"

struct Bloom
{
	int numHorizontalPasses{ 5 };
	int downsample{ 4 };

	Texture blur;
	Texture blurUpscaled;
};

