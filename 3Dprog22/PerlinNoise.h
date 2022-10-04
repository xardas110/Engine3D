#pragma once
#include "Texture.h"
#include "qopenglfunctions_4_5_core.h"

struct PerlinNoise : protected QOpenGLFunctions_4_5_Core
{
	~PerlinNoise();

	void Init(class World* world);
	void Clean();

	Texture texture;
	
	bool bPeriodic = true;
	float a = 1.0f;
	float b = 2.0f;
	float freqScale = 2.0f;
	int numOcts = 4;
	int width{ 64 }, height{ 64 };

public:
	const Texture& GetTexture() const;
};

