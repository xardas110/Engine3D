#pragma once
#include "Blur.h"
#include "Texture.h"
#include "Mesh.h"

class BlurSystem
{
	friend class DeferredRenderer;
	void Init();
	void InitShader();
	void InitShaderBindings();
	void InitFBO();

	void Filter(const Texture& source, const Texture& target, const Mesh& quad);

	void Clean();
private:
	Blur blur;
	BlurBindings bb;
};

