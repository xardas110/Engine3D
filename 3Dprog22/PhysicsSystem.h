#pragma once
#include "Include/entt/entt.hpp"
#include "Include/glm/glm.hpp"
#include "ConvexHull.h"
#include "Collision.h"
#include <vector>
#include "boundingvolumes.h"
#include "Octree.h"
#include "CollisionHeightmap.h"

class ThreadPoolx;
struct CollisionComponent;

#define NUM_THREADS 32

struct BroadPhase
{
	std::vector<BoundingBox> mBounds;
};

struct NarrowPhase
{
	CollisionHeightmap heightmap;
	std::vector<BoundingSphere> mSpheres;
	std::vector<ConvexHull> mConvexHulls;
};

class PhysicsSystem
{
	friend class World;
	friend class Entity;
	friend class Collideable;

	PhysicsSystem(class World* world, entt::registry& registry);
	~PhysicsSystem();

	void Init();

	void PreSyncTransforms();
	void Update(float deltatime);
	void ApplyForces(float deltatime);
	void MoveBodies(float deltatime);
	void ComputeContacts(std::vector<ContactManifold>& outContacts);
	void ResolveCollision(ContactManifold& contact);
	void ResolveCollision(RigidBody& bodyA, RigidBody& bodyB, ContactManifold& contact);
	void ResolveContacts(std::vector<ContactManifold>& inContacts);
	void PostSyncTransforms();

	BroadPhase& GetBroadPhaseDate();
	NarrowPhase& GetNarrowPhaseData();

	void RegisterCollider(Collideable& inOutCollider);
public:
	void THMoveBodies(int thIndex);
	void InsertOctree(Collideable& collidable, entt::entity);
	void THInsertOctree(int thIndex);
	void ResolveHeightmapCollisions(Collideable& collidable, entt::entity entity);
	void THResolveHeightmapCollisions(int thIndex);
	void ResolveNarrowCollisions(CollisionPair& pair);
	void THResolveNarrowCollisions(int thIndex);
	void THPostSyncBodies(int thIndex);

	void OnConstructCollider(entt::registry& registry, entt::entity entity);
	void OnConstructBody(entt::registry& registry, entt::entity entity);	

	void RegisterHeightmap(const CollisionHeightmap& hm);

	const BroadPhase& GetBroadPhaseData() const;
	const NarrowPhase& GetNarrowPhaseData() const;
	
	const Octree* GetOctree() const;

	void DrawConvexHulls() const;
	void DrawBoundingBoxes() const;

	static PhysicsSystem* Get();

	bool bSimulateThreaded{ true };

	void SetCastShadowOnPhysicBalls(bool bCast);
	void SetShaderModelOnPhysicsBalls(int shaderModel);

	glm::vec3 externalImpulse{ 0.f, 0.f, 0.f };
private:
	glm::vec3 gravity = glm::vec3(0.f, -9.81f, 0.f);

	class World* world;
	entt::registry* registry;	

	Octree* octree; //Raskere å reloade octreet med bare dynamiske objekter
	BoundingBox octreeSize{ glm::vec3(-2000.f), glm::vec3(2000.f) };

	entt::entity instancedPhysicsBalls;

	BroadPhase mBp; //Broadphase data
	NarrowPhase mNp; //Narrowphase data

	std::vector<Octree*> octLeafsCache;

	bool bShowHeightmapCollisionTriangles{ false };
	bool bShowAVelocity{ false };
	float colorAVel[3]{ 1.f, 0.f, 0.f };
	bool bShowBVelocity{ false };
	float colorBVel[3]{ 0.f, 1.f, 0.f };
	bool bShowAImpulse{ false };
	float colorAImpulse[3]{ 1.f, 1.f, 1.f };
	bool bShowBImpulse{ false };
	float colorBImpulse[3]{ 1.f, 0.f, 1.f };
	bool bShowATangentImpulse{ false };
	float colorATangentImpulse[3]{ 0.f, 0.f, 1.f };
	bool bShowBTangentImpulse{ false };
	float colorBTangentImpulse[3]{ 0.f, 0.5f, 1.f };
	
	bool bShowACollisionPoints{ false };
	float aCollisionPointsColor[3]{ 0.f, 1.f, 0.f };

	bool bShowBCollisionPoints{ false };
	float bCollisionPointsColor[3]{ 0.f, 0.f, 1.f };

	float debugRenderTimer{ 0.f };

	bool bShowCollisionNormal{ false };
	float collisonNormalColor[3]{ 1.f, 1.f, 0.f };

	bool bEnableStepMode{ false };
	float stepValue = 0.01f;
	
	float deltatime = 0;

	ThreadPoolx* th{nullptr};
	ThreadPoolx* thOctInsertion{ nullptr };
	ThreadPoolx* thResolveNarrowCollision{ nullptr };
	ThreadPoolx* thMoveBodies{ nullptr };
	ThreadPoolx* thPostSyncBodies{ nullptr };
};

