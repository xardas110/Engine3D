#pragma once

#include <utility>
#include "AudioData.h"
#include "Include/entt/entt.hpp"
#include "GameMode.h"

class VISM2 : public GameMode
{
	~VISM2();

	virtual void Create(class World* world) override;

	virtual void BeginPlay(class World* world) override;

	virtual void EndPlay(class World* world) override;

	virtual void Update(class World* world, float deltatime) override;

	virtual void UpdateEditor(class World* world, float deltatime) override;

	virtual void Reset(class World* world) override;

	virtual void OnKeyPress(QKeyEvent* event) override;

private:
	entt::entity terrainHighRes;
	entt::entity terrainLowRes;

	bool bShowWifreframe{ false };

	entt::entity CreateTerrain(class World* world, const std::string& jsonPath);
};


