#include "SSBOManager.h"

bool SSBOManager::SSBOExists(const std::string& name)
{
	return ssboMap.find(name) != ssboMap.end();
}

bool SSBOManager::AddSSBO(const std::string& name, SSBO& ubo)
{
	if (SSBOExists(name))
	{
		printf("ssbo already exists %s", name.c_str());
		return false;
	}

	ssboMap[name] = ubo;
	return true;
}

SSBO SSBOManager::CreateSSBO(const std::string& name, unsigned sizeInBytes, unsigned bindingId)
{
	if (SSBOExists(name))
	{
		printf("SSBO exists, only unique names allowed\n");
		printf("Returning the existing SSBO.\n");

		return ssboMap[name];
	}

	unsigned id;
	glGenBuffers(1, &id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingId, id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeInBytes, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	SSBO ssbo;

	ssbo.id = id;

	AddSSBO(name, ssbo);
	return ssbo;
}

void SSBOManager::DeleteSSBO(const std::string& name)
{
	if (!SSBOExists(name))
	{
		printf("Trying to delete a SSBO that doesnt exist: %s\n", name.c_str());
		return;
	}

	glDeleteBuffers(1, &ssboMap[name].id);
	ssboMap.erase(name);
}

void SSBOManager::BindShaderToBlock(unsigned shaderId, unsigned ssbo, unsigned bindingId, const std::string& uniformName)
{
	const auto blockIndex = glGetProgramResourceIndex(shaderId, GL_SHADER_STORAGE_BLOCK, uniformName.c_str());

	glShaderStorageBlockBinding(shaderId, blockIndex, bindingId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingId, ssbo);
}

void SSBOManager::BindData(SSBO ssbo, unsigned offsetInBytes, unsigned sizeInBytes, void* data)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo.id);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offsetInBytes, sizeInBytes, data);
}

SSBOManager::SSBOManager()
{
	initializeOpenGLFunctions();
}

SSBOManager::~SSBOManager()
{
	for (auto [name, buffer] : ssboMap)
	{
		glDeleteBuffers(1, &buffer.id);
	}
}
