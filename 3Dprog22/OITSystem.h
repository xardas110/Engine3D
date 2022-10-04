#pragma once
#include "ABuffer.h"
#include "shader.h"
#include <qopenglfunctions_4_5_core.h>

class OITSystem : protected QOpenGLFunctions_4_5_Core
{
	friend class DeferredRenderer;

	void Init();
	void InitIndexBuffer();
	void InitAtomicBuffer();
	void InitSSBO();
	void InitClearTexture();
	void BindShaderToBlock(const Shader& shader);

	void Clear();

	void OnResize(unsigned width, unsigned height);

	void Clean();
private:
	ABuffer aBuffer;
};

