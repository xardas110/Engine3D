#include "Worley3D.h"
#include "RenderEngine.h"
#include "Include/glm/gtc/noise.hpp"

void Worley3D::Init(World* world)
{
	initializeOpenGLFunctions();
	InitTexture();
	InitShader();
	Compute();
}

void Worley3D::InitTexture()
{
	GLuint id;

	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, id);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, dims.x, dims.y, dims.z, 0, GL_RGBA, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_3D);
	glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

	texture.textureID = id;
	texture.width = dims.x;
	texture.height = dims.y;
	texture.dimensions = dims.z;
	texture.format = GL_RGBA16F;
}

void Worley3D::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::Worley3D, shader);
	re->BindShader(shader);
	wb.outWorley = re->GetUniformLocation("outWorley");
}

void Worley3D::Compute()
{
	auto* re = RenderEngine::Get();
	re->BindShader(shader);
	re->SetTexture(texture, GL_TEXTURE_3D, wb.outWorley, 0);
	re->BindImageTexture(0, texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	re->DispatchCompute(dims.x / 8, dims.y / 8, dims.z / 8);
	glGenerateMipmap(GL_TEXTURE_3D);
	glFinish();
}

void Worley3D::Clean()
{
	glDeleteTextures(1, &texture.textureID);
}
