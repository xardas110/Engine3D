#pragma once
#include "NodeGrid.h"
#include "Include/entt/entt.hpp"

class PathFindingSystem
{
	friend class World;

	void Init(class World* world, entt::registry& registry);
	void Update(class World* world, entt::registry& registry);

	void DrawDebugPF1();
	void DrawDebugPF2();
public:
	Node* GetNode(const glm::vec3& ws);
	bool IsOccupied(const glm::vec3& ws);

private:
	std::shared_ptr<NodeGrid> nodeGrid{ nullptr };
};

