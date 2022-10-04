#pragma once
#include "AlphaMesh.h"
#include <queue>

class AlphaPassSystem
{
	friend class DeferredRenderer;
	friend class StaticMeshSystem;
	friend class SkeletalMeshSystem;

	using PQ = std::priority_queue<AlphaPassInformation, std::vector<AlphaPassInformation>, std::greater<AlphaPassInformation>>;

	PQ alphaQue;

public:
	AlphaPassSystem();
	~AlphaPassSystem();

	static AlphaPassSystem* Get();

	void AddAlphaMesh(const AlphaPassInformation& alphaPassInfo);

	void AlphaPass(class World* world, entt::registry& registry);
};

