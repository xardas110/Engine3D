#pragma once
#include "Water.h"
#include "Include/entt/entt.hpp"
#include "shader.h"
#include "Mesh.h"
#include <qopenglfunctions_4_5_core.h>

struct WaterBindings
{
	int model;
	int moveFactor;
	int normalmap;
	int waterDUDV;
};

class WaterSystem : public QOpenGLFunctions_4_5_Core
{
	friend class World;
	friend class DeferredRenderer;

	~WaterSystem();

	void InitMesh();
	void InitShader();
	void InitShaderBindings();
	void Init(class World* world, entt::registry& registry);
	void Render(class World* world, entt::registry& registry);

	Mesh mesh;
	Shader shader;
	WaterBindings bind;
};

