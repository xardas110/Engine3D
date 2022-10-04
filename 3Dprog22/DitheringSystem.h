#pragma once
#include "Texture.h"
#include "Mesh.h"
#include "Dithering.h"

class DitheringSystem
{
	friend class DeferredRenderer;

	void Init(class TextureManager* textureManager);
	void Filter(const Texture& source, const Texture& target, const Mesh& quad);
private:
	void Clean();
	Dithering dithering;
};

