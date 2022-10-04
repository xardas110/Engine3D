#pragma once
#include "SMAA.h"
#include "Mesh.h"

/*Sampling and upscaling*/
class SMAASystem
{
	friend class DeferredRenderer;

	void Init(class World* world);
	void Filter(const Texture& source, const Texture& target, const Mesh& quad);

	void Clean();
private:
	SMAA smaa;
};

