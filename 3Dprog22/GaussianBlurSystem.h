#pragma once
#include "GaussianBlur.h"
#include "Texture.h"
#include "Mesh.h"

class GaussianBlurSystem
{
	friend class DeferredRenderer;
	void Init();
	void InitShader();
	void InitShaderBindings();
	void InitFBOs();

	void Filter(const Texture& source, const Texture& target, const Mesh& quad, int n = 5);

	void Clean();
private:
	GaussianBlur gb;
	GaussianBlurBindings gbb;
};

