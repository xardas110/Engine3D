#pragma once
#include "Include/entt/entt.hpp"
#include "ThreadPoolx.h"
#include "BSpline.h"
#include <vector>
#include <qopenglfunctions_4_5_core.h>
#include "shader.h"


class BSplineSystem : public QOpenGLFunctions_4_5_Core
{
	friend class World;
	friend class DeferredRenderer;

	BSplineSystem();
	~BSplineSystem();

	void InitShader();
	void Init(class World* world, entt::registry& registry);
	
	void GenerateLineSegment();
	void PreUpdate(float deltatime);
	void PostUpdate(float deltatime);
	void RenderSegments(std::vector<BSplineSegment>& inSegments);
	void Render();

	float currentTimer = 0.f;
	float waitTime = 1.f;

	class World* world{ nullptr };
	entt::registry* registry{ nullptr };

	float deltatime = 0.f;

	Shader shader;

	uint segmentVAO;
	uint segmentVBO;

	ThreadPoolx* thPool{nullptr};
	bool bDrawn[BSPLINE_CONTEXTS]{ false };
	std::vector<BSplineSegment> lines[BSPLINE_CONTEXTS];
public:
	void THUpdate(int thIndex);
};

