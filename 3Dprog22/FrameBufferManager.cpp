#include "FrameBufferManager.h"

#include <iostream>

FrameBuffer FrameBufferManager::CreateCascadedFBO(const std::string& name, unsigned width, unsigned height, unsigned dimensions)
{	
	assert(!FBOExists(name) && "FBO with that name already exists! FBO names needs to be unique");
	
	initializeOpenGLFunctions();
	
	FrameBuffer FBO;
	glGenFramebuffers(1, &FBO.fboId);

	FBO.texture.dimensions = dimensions;
	FBO.texture.width = width;
	FBO.texture.height = height;

	glGenTextures(1, &FBO.texture.textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, FBO.texture.textureID);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, width, height, dimensions, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO.fboId);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, FBO.texture.textureID, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	const auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("CASCADE FRAME BUFFER NOT COMPLETE!\n");
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Insert(name, FBO);
	
	return FBO;
}

FrameBuffer FrameBufferManager::CreateShadowGrid(const std::string& name, unsigned width, unsigned height, unsigned dimensions)
{
	assert(!FBOExists(name) && "FBO with that name already exists! FBO names needs to be unique");

	initializeOpenGLFunctions();

	FrameBuffer FBO;
	glGenFramebuffers(1, &FBO.fboId);

	FBO.texture.dimensions = dimensions;
	FBO.texture.width = width;
	FBO.texture.height = height;

	glGenTextures(1, &FBO.texture.textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, FBO.texture.textureID);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, width, height, dimensions, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO.fboId);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, FBO.texture.textureID, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	const auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("CASCADE FRAME BUFFER NOT COMPLETE!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Insert(name, FBO);

	return FBO;
}

bool FrameBufferManager::GenerateFBO(const std::string& name, FrameBuffer& outFBO)
{
	initializeOpenGLFunctions();
	if (FBOExists(name))
	{
		printf("FBO exists, returning instance\n");
		outFBO = fboMap[name];
		return false;
	}

	unsigned fbo;
	glGenFramebuffers(1, &fbo);

	outFBO.fboId = fbo;

	Insert(name, outFBO);

	return true;
}

bool FrameBufferManager::DeleteFBO(const std::string& name)
{
	if (!FBOExists(name))
	{
		printf("FBO doesnt exists: %s, deletion failed \n", name.c_str());
		return false;
	}

	glDeleteFramebuffers(1, &fboMap[name].fboId);

	fboMap.erase(name);

	return true;
}

bool FrameBufferManager::CheckFBOStatus()
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FBO creation failed\n");
		return false;
	}
	return true;
}

bool FrameBufferManager::FBOExists(const std::string& name)
{
	return fboMap.find(name) != fboMap.end();
}

bool FrameBufferManager::Insert(const std::string& name, FrameBuffer& fbo)
{
	assert(!FBOExists(name));
	fboMap[name] = fbo;
	return true;
}
