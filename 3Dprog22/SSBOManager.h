#pragma once
#include <string>
#include <QOpenGLFunctions_4_5_Core>
#include "SSBO.h"
#include <map>

class SSBOManager : protected QOpenGLFunctions_4_5_Core
{
	using SSBOMap = std::map<std::string, SSBO>;
	SSBOMap	ssboMap;

	bool	SSBOExists(const std::string& name);
	bool	AddSSBO(const std::string& name, SSBO& ubo);
public:
	SSBO	CreateSSBO(const std::string& name, unsigned sizeInBytes, unsigned bindingId);
	void	DeleteSSBO(const std::string& name);
	void	BindShaderToBlock(unsigned shaderId, unsigned ssbo, unsigned bindingId, const std::string& uniformName);
	void	BindData(SSBO ssbo, unsigned offsetInBytes, unsigned sizeInBytes, void* data);

	SSBOManager();
	~SSBOManager();

};
