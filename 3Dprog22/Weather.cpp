#include "Weather.h"
#include "World.h"
#include "RenderEngine.h"

void Weather::Init(World* world)
{
	initializeOpenGLFunctions();
	InitTexture();
	InitShader();
	InitShaderBindings();
	Compute();
}

void Weather::InitTexture()
{
	unsigned int id;
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dims.x, dims.y, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

	texture.textureID = id;
	texture.width = dims.x;
	texture.height = dims.y;
	texture.format = GL_RGBA16F;
}

void Weather::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::Weather, shader);
}

void Weather::InitShaderBindings()
{
	auto* re = RenderEngine::Get();
	re->BindShader(shader);

	wb.perlinAmplitude = re->GetUniformLocation("perlinAmplitude");
	wb.perlinFrequency = re->GetUniformLocation("perlinFrequency");
	wb.perlinScale = re->GetUniformLocation("perlinScale");
	wb.perlinOctaves = re->GetUniformLocation("perlinOctaves");
	wb.seed = re->GetUniformLocation("seed");
}

void Weather::Compute()
{
	auto* re = RenderEngine::Get();
	re->BindShader(shader);
	re->BindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

	re->SetFloat(wb.perlinAmplitude, perlinAmplitude);
	re->SetFloat(wb.perlinFrequency, perlinFrequency);
	re->SetFloat(wb.perlinScale, perlinScale);
	re->SetInt(wb.perlinOctaves, perlinOctaves);
	
	re->SetVec3(wb.seed, seed);
	
	re->DispatchCompute(texture.width / 16, texture.width / 16, 1);
	glFinish();
}

void Weather::Clean()
{
	glDeleteTextures(1, &texture.textureID);
}
