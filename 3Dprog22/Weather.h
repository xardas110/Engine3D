#pragma once
#include "PerlinNoise.h"
#include "shader.h"
#include "Texture.h"
#include "Include/glm/glm.hpp"
#include <qopenglfunctions_4_5_core.h>

struct WeatherBindings
{
	int perlinAmplitude;
	int perlinFrequency;
	int perlinScale;
	int perlinOctaves;
	int seed;
};

/// <summary>
/// GPU GEM PRO 7, see p. 103
/// Brief: A weather texture which will consist of perlin noise, 
/// R: Cloud coverage[0-1]
/// G: Precipiation: "The chance that the clouds overhead will produce rain" As of now this feature won't	be added
/// B: Cloud Type [0-1] represents the range stratus-stratocumulus-cumulus
/// A: Open for vectorization
/// </summary>
class Weather : protected QOpenGLFunctions_4_5_Core
{
	friend class VolumetricClouds;

	void Init(class World* world);
	void InitTexture();
	void InitShader();
	void InitShaderBindings();
	
	/*Computes weather texture*/
	void Compute();

	void Clean();

private:
	float perlinAmplitude{0.5f};
	float perlinFrequency{0.8f};
	float perlinScale{100.f};
	int perlinOctaves{ 4 };
	glm::vec3 seed{ 0.f };

	Texture texture;
	Shader shader;
	glm::ivec2 dims{ 1024 };
	WeatherBindings wb;
};

