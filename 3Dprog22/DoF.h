#pragma once
#include "Texture.h"
#include "shader.h"
#include "Include/glm/glm.hpp"
#include "GlobalTypeDefs.h"
#include <array>
#include "FrameBuffer.h"

struct DoFBindings
{
	int colorTexture; //pre hdr pass color with light
	int depthTexture; // color with light depth
	int targetDim;
	int uc[DOF_UC_SIZE];

	int zNear, zFar;
	int fov, focusScale;
	int uFar;
	int maxBlurSize;
	int radScale;
};

struct DoF
{
	Texture blur;

	FrameBuffer fbo;
	Shader shader;

	//Distributed positions on the unit circle
	std::array< glm::vec2, DOF_UC_SIZE > uc;
};

