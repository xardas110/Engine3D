#pragma once
#include "Bilatreral.h"
#include "Mesh.h"

/*Framebuffers are faster than compute shaders for this task
glfinish will delay the cpu a lot with compute.
*/
class BilateralSystem
{
	friend class DeferredRenderer;

	void Init(class World* world);
	void Filter(const Texture& source, const Texture& target, const Mesh& quad);
private:
	void Clean();
	Bilatreral bilateral;
};

