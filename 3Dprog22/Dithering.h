#pragma once
#include "shader.h"

struct DitheringBindings
{
	int source;
	int noiseTexture;
};

struct Dithering
{
	Shader shader;
	Texture noise;
	DitheringBindings db;
};

