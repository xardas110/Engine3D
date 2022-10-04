#include "GameMode.h"
#include <utility>
#include "AudioData.h"

class Entity;

class GrassGameMode : public GameMode
{
	~GrassGameMode();

	virtual void Create(class World* world, entt::registry& registry);

	virtual void BeginPlay(class World* world, entt::registry& registry) override;

	virtual void EndPlay(class World* world, entt::registry& registry) override;

	virtual void Update(class World* world, entt::registry& registry, float deltatime) override;

	virtual void UpdateEditor(class World* world, entt::registry& registry, float deltatime) override;

	virtual void Reset(class World* world, entt::registry& registry) override;

private:
	entt::entity playerEnt;

	void CreateTerrain(class World* world);
	void CreateScene(class World* world);
	void CreateTrees(class World* world);

	void UpdatePlayer(class World* world, float deltatime);
};
