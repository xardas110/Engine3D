#pragma once
#include "Texture.h"
#include "shader.h"
#include "GameMode.h"
#include "Include/entt/entt.hpp"

class Sponza : public GameMode
{
	virtual void Create(class World* world, entt::registry& registry);

	virtual void BeginPlay(class World* world, entt::registry& registry) override;

	virtual void EndPlay(class World* world, entt::registry& registry) override;

	virtual void Update(class World* world, entt::registry& registry, float deltatime) override;

	virtual void UpdateEditor(class World* world, entt::registry& registry, float deltatime) override;

	virtual void Reset(class World* world, entt::registry& registry) override;

private:
	void CreateScene(class World* world);
};

