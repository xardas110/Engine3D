#include "PerlinNoise.h"
#include "Include/glm/gtc/noise.hpp"
#include "World.h"

PerlinNoise::~PerlinNoise()
{
	glDeleteTextures(1, &texture.textureID);
}

void PerlinNoise::Init(World* world)
{
	initializeOpenGLFunctions();

	
	float* data = new float[width * height * 4];

	float xFactor = 1.f / (width - 1);
	float yFactor = 1.f / (height - 1);

	for (size_t row = 0; row < height; row++)
	{
		for (size_t col = 0; col < width; col++)
		{
			auto x = xFactor * col;
			auto y = yFactor * row;
			float sum = 0.f;
			float freq = a;
			float scale = b;

			for (int oct = 0; oct < numOcts; oct++)
			{
				glm::vec2 p(x * freq, y * freq);

				float val = 0.f;

				if (bPeriodic)
					val = glm::perlin(p, glm::vec2(freq)) / scale;
				else
					val = glm::perlin(p) / scale;

				
				sum += val;
				float result = (sum + 1.f) / 2.f;
				data[((row * width + col) * 4) + oct] = result;
				freq *= freqScale;
				scale *= b;
			}
		}
	}

	GLuint id;
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, width, height, GL_RGBA, GL_FLOAT, data);

	texture.textureID = id;
	texture.width = width;
	texture.height = height;
	texture.format = GL_RGBA16F;

	delete[] data;
}

void PerlinNoise::Clean()
{
	glDeleteTextures(1, &texture.textureID);
}

const Texture& PerlinNoise::GetTexture() const
{
	return texture;
}
