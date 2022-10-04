#pragma once
#include "Texture.h"
#include "Include/glm/glm.hpp"
#include "shader.h"
#include <qopenglfunctions_4_5_core.h>

struct Worley3DBindings
{
	int outWorley;
};


struct Worley3D : protected QOpenGLFunctions_4_5_Core
{
	void Init(class World* world);
	void InitTexture();
	void InitShader();

	/*Computes the noise*/
	void Compute();

	void Clean();

	int numOcts = 3;

	glm::ivec3 dims{ 32 };
	Texture texture;
	Shader shader;
	Worley3DBindings wb;
};

