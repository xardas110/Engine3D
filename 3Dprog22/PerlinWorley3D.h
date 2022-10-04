#pragma once
#include "Texture.h"
#include "Include/glm/glm.hpp"
#include "shader.h"
#include <qopenglfunctions_4_5_core.h>

struct PerlinWorley3DBindings
{
	int perlinWorleyOut;
};


/// <summary>
/// Perlin-Worley red channel
/// Worley with increasing freq from green to alpha
/// </summary>
struct PerlinWorley3D : protected QOpenGLFunctions_4_5_Core
{
	void Init(class World* world);
	void InitTexture();
	void InitShader();

	/*Computes the noise*/
	void Compute();

	void Clean();

	int numOcts = 3;

	glm::ivec3 dims{128};
	Texture texture;
	Shader shader;
	PerlinWorley3DBindings pwb;
};
