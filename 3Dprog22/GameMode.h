#pragma once

#include "Include/entt/entt.hpp"
#include "qevent.h"

class GameMode
{
	friend class World;

protected:
	virtual void Create(class World* world, entt::registry& registry);

	virtual void BeginPlay(class World* world, entt::registry& registry);

	virtual void EndPlay(class World* world, entt::registry& registry);

	virtual void Update(class World* world, entt::registry& registry, float deltatime);

	virtual void UpdateEditor(class World* world, entt::registry& registry, float deltatime);

	//Oppgave 14
	virtual void Reset(class World* world, entt::registry& registry);

	virtual void Create(class World* world);

	virtual void BeginPlay(class World* world);

	virtual void EndPlay(class World* world);

	virtual void Update(class World* world, float deltatime);

	virtual void UpdateEditor(class World* world, float deltatime);

	virtual void Reset(class World* world);

	virtual void OnKeyPress(QKeyEvent* event);
};

