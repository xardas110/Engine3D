#pragma once
#include <QOpenGLFunctions_4_1_Core>
#include "FrameBuffer.h"
#include "GBuffer.h"

class FrameBufferManager : protected QOpenGLFunctions_4_1_Core
{
	friend class World;
	
	using FrameBufferMap = std::map<std::string, FrameBuffer>;

	FrameBufferMap fboMap;
	
	bool FBOExists(const std::string& name);
	bool Insert(const std::string& name, FrameBuffer& fbo);

public:
	FrameBuffer CreateCascadedFBO(const std::string& name, unsigned width, unsigned height, unsigned dimensions);

	FrameBuffer CreateShadowGrid(const std::string& name, unsigned width, unsigned height, unsigned dimensions);

	bool GenerateFBO(const std::string& name, FrameBuffer& outFBO);

	bool DeleteFBO(const std::string& name);

	bool CheckFBOStatus();
};

