#pragma once
#include <utility>
#include "AudioData.h"
#include "Include/entt/entt.hpp"
#include "GameMode.h"

class PhysicsGameMode : public GameMode
{
	~PhysicsGameMode();

	virtual void Create(class World* world, entt::registry& registry);

	virtual void BeginPlay(class World* world, entt::registry& registry) override;

	virtual void EndPlay(class World* world, entt::registry& registry) override;

	virtual void Update(class World* world, entt::registry& registry, float deltatime) override;

	virtual void UpdateEditor(class World* world, entt::registry& registry, float deltatime) override;

	virtual void Reset(class World* world, entt::registry& registry) override;

	virtual void OnKeyPress(QKeyEvent* event) override;

	void SpawnBox(const glm::vec3& pos);
	void SpawnTriangle(const glm::vec3& pos);
	void SpawnStatue(const glm::vec3& pos);
	void SpawnCapsule(const glm::vec3& pos);

	void SpawnActualSphere(const glm::vec3& pos);

	void CreateTerrain(class World* world);
	void CreateWater(class World* world);
	void CreateTree(class World* world);
	void CreateRocks(class World* world);
	void CreateBSpline();

	bool bDrawOctreeLeafs = false;
	bool bDrawBoundingBoxes = false;
	bool bDrawConvexHulls = false;
private:
	void CreateScene(class World* world);
};

