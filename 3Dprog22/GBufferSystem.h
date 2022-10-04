#pragma once
#include "GBuffer.h"
#include "qopenglfunctions_4_5_core.h"

class GBufferSystem : protected QOpenGLFunctions_4_5_Core
{
	friend class DeferredRenderer;

	void Init();
	void BindGBuffer();
	void UnbindGBuffer();
	void ClearGBuffer();
	void OnResize(int width , int height);
	void Clean();
private:
	GBuffer gBuffer;
};

