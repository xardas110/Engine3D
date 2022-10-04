#pragma once
#include "shader.h"
#include "Include/glm/glm.hpp"

struct FogBindings
{
	int posBuffer, colorBuffer;
	int zFar;
	int fogColor;
	int sightRange;
};

struct Fog
{
	Shader shader;
	glm::vec3 fogColor{ 0.60f, 0.75f, 0.85f };
	FogBindings bind;
};

