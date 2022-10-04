#pragma once
#include "Camera.h"
#include "UBO.h"
#include "StaticMesh.h"
#include "Material.h"
#include "Include/entt/entt.hpp"

/*Just updating camera*/
/*This class should have ownership, but too much work to refactor right now*/
class CameraSystem
{
	friend class World;
	friend class DeferredRenderer;

	void Init(class World* world, entt::registry& registry);
	void InitBuffer(class World* world);
	void InitCameraMesh(class World* world);
	void InitCameraShader();

	void Update(class World* world, entt::registry& registry, float deltaTime);

	//Oppgave 5b, alle kamera som er i scenen rendres her
	void Render(class World* world, entt::registry& registry);

private:
	void BindDirlight(class World* world);
	void DrawMesh(class RenderEngine* renderEngine, const Mesh& mesh);

	UBO ubo;

	Shader cameraShader;
	StaticMesh cameraMesh;
	int model, smCascades;
	MaterialTexturedBindings mtb;
};

