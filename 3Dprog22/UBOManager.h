#pragma once
#include <string>
#include <QOpenGLFunctions_4_5_Core>
#include "UBO.h"
#include <map>

/// <summary>
/// Have ownership of all UBOs
/// </summary>
class UBOManager : protected QOpenGLFunctions_4_5_Core
{
	using UBOMap = std::map<std::string, UBO>;
	using UBOBindings = std::vector<unsigned>;

	UBOMap	uboMap;
	// To avoid saving multiple buffers to the same id
	UBOBindings uboBindings;

	bool	UBOExists(const std::string& name);
	bool	UBOBindingExists(unsigned bindingID);
	bool	AddUBO(const std::string& name, UBO& ubo, unsigned bindingId);
public:
	UBO		CreateUBO(const std::string& name, unsigned sizeInBytes, unsigned bindingId);
	void	DeleteUBO(const std::string& name);
	void	BindShaderToBlock(unsigned shaderId, unsigned bindingId, const std::string& uniformName);
	void	BindData(UBO ubo, unsigned sizeOffsetInBytes, unsigned sizeInBytes, void* data);

	~UBOManager();
};

