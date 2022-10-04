#pragma once
#include "Fog.h"
#include "Mesh.h"
#include "DeferredRendererConfig.h"

class FogSystem
{
	friend class DeferredRenderer;

	void Init(class World* world);
	void Process(class World* world, const Texture& posBuffer, const Texture& colorBuffer, const Mesh& quad, const DeferredRendererConfig& config);
	void Clean();
public:
	void SetSight(float newSight);
	void SetColor(const glm::vec3 newColor);
	float GetSight();
	const glm::vec3& GetColor() const;
private:
	Fog fog;
};

