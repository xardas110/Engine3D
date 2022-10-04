#pragma once

#include "shader.h"
#include "SSBO.h"
#include "AlphaBuffer.h"

class WeightedBlendSystem
{
	friend class DeferredRenderer;

	void Init();
	void InitShader();
	void InitSSBO();

	void Clear();

	void Clean();

	Shader clearShader;
	SSBO ssbo;
};

