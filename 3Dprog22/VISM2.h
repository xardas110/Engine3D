#pragma once

#include <utility>
#include "AudioData.h"
#include "Include/entt/entt.hpp"
#include "GameMode.h"
#include <vector>

class VISM2 : public GameMode
{
	~VISM2();

	virtual void Create(class World* world) override;
	void		 CreateWater(class World* world);
	virtual void BeginPlay(class World* world) override;

	virtual void EndPlay(class World* world) override;

	virtual void Update(class World* world, float deltatime) override;

	virtual void UpdateEditor(class World* world, float deltatime) override;

	virtual void Reset(class World* world) override;

	virtual void OnKeyPress(QKeyEvent* event) override;

	void SpawnBall(const glm::vec3& pos, bool bHighres = true, bool bAddSpline = false);
	void SpawnBalls(bool bAddSpline = false);
	void ClearBalls();
private:
	class World* world;

	int nextNameId = 0;

	int numBallsX = 50;
	int numBallsZ = 50;
	int ballSpacing = 20;

	entt::entity terrainHighRes;
	entt::entity terrainLowRes;

	std::vector<entt::entity> balls;

	bool bDrawOctreeLeafs = false;
	bool bDrawBoundingBoxes = false;
	bool bDrawConvexHulls = false;
	bool bCastShadowOnRain = false;
	bool bRainShaded = true;

	entt::entity CreateTerrain(class World* world, const std::string& jsonPath);
};


