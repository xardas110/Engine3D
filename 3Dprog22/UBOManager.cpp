#include "UBOManager.h"

#include <cassert>

bool UBOManager::UBOExists(const std::string& name)
{
	return uboMap.find(name) != uboMap.end();
}

bool UBOManager::UBOBindingExists(unsigned bindingID)
{
	for (unsigned id : uboBindings)
	{
		if (id == bindingID)
		{
			printf("UBO binding id exists, chose a different id\n");
			return true;
		}
	}

	return false;
}

bool UBOManager::AddUBO(const std::string& name, UBO& ubo, unsigned bindingId)
{
	if (UBOExists(name))
	{
		printf("ubo already exists %s", name.c_str());
		return false;
	}

	if (UBOBindingExists(bindingId))
		return false;

	uboMap[name] = ubo;
	uboBindings.emplace_back(bindingId);

	return true;
}

UBO UBOManager::CreateUBO(const std::string& name, unsigned sizeInBytes, unsigned bindingId)
{
	if (UBOExists(name))
	{
		printf("UBO exists: %s returning an instance\n", name.c_str());
		return uboMap[name];
	}

	if (UBOBindingExists(bindingId))
	{ 
		printf("UBO BINDING EXISTS binding id: %i", bindingId);
		return UBO();
	}

	initializeOpenGLFunctions();
	
	UBO ubo{0u};	
	glGenBuffers(1, &ubo.UBOId);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo.UBOId);
	glBufferData(GL_UNIFORM_BUFFER, sizeInBytes, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingId, ubo.UBOId);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	AddUBO(name, ubo, bindingId);	
	return ubo;
}

void UBOManager::DeleteUBO(const std::string& name)
{
	if (UBOExists(name))
	{
		glDeleteBuffers(1, &uboMap[name].UBOId);
	}
}

void UBOManager::BindShaderToBlock(unsigned shaderId, unsigned bindingId, const std::string& uniformName)
{
	initializeOpenGLFunctions();

	const auto uniformBlockIndex = glGetUniformBlockIndex(shaderId, uniformName.c_str());
	glUniformBlockBinding(shaderId, uniformBlockIndex, bindingId);
}

void UBOManager::BindData(UBO ubo, unsigned offsetInBytes, unsigned sizeInBytes, void* data)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo.UBOId);
	glBufferSubData(GL_UNIFORM_BUFFER, offsetInBytes, sizeInBytes, data);
}

UBOManager::~UBOManager()
{
	for (auto [name, ubo] : uboMap)
	{
		glDeleteBuffers(1, &ubo.UBOId); //TODO: Might be wrong to delete UBOs like this
	}
}
