#pragma once
#include "Bloom.h"

class BloomSystem
{
	friend class DeferredRenderer;

	void Init();
	
	void OnResize(int width, int height);

	void Clean();
private:
	Bloom bloom;
};

