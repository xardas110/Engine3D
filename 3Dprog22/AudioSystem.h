#pragma once
#include "Include/entt/entt.hpp"

class AudioSystem
{
	friend class World;

	void Init(class World* world, entt::registry& registry);

	void Update(class World* world, entt::registry& registry);

	void UpdateListener(World* world);
	void UpdateTransforms(entt::registry& registry);

	void DeleteAll(entt::registry& registry);
};

