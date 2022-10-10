#include "PhysicsSystem.h"
#include "Components.h"
#include "RigidBody.h"
#include "Components.h"
#include "Entity.h"
#include "Collision.h"
#include <iostream>
#include "Debug.h"
#include "RenderDebugger.h"
#include "EPA.h"
#include "ThreadPoolx.h"

PhysicsSystem* gPhysicsSystem = nullptr;

PhysicsSystem* PhysicsSystem::Get()
{
	return gPhysicsSystem;
}

PhysicsSystem::PhysicsSystem(World* world, entt::registry& registry)
{
	gPhysicsSystem = this;
	this->registry = &registry;
	this->world = world;

	th = new ThreadPoolx([this](int threadindex) {this->THResolveHeightmapCollisions(threadindex); }, NUM_THREADS);
	thOctInsertion = new ThreadPoolx([this](int threadindex) {this->THInsertOctree(threadindex); }, NUM_THREADS);
	thResolveNarrowCollision = new ThreadPoolx([this](int threadindex) {this->THResolveNarrowCollisions(threadindex); }, NUM_THREADS);
	thMoveBodies = new ThreadPoolx([this](int threadindex) {this->THMoveBodies(threadindex); }, NUM_THREADS);
	thPostSyncBodies = new ThreadPoolx([this](int threadindex) {this->THPostSyncBodies(threadindex); }, NUM_THREADS);

	registry.on_construct<CollisionComponent>().connect<&PhysicsSystem::OnConstructCollider>(this);
	registry.on_construct<PhysicsComponent>().connect<&PhysicsSystem::OnConstructBody>(this);
	
	BoundingBox box;
	box.min = glm::vec3(-2000.f);
	box.max = glm::vec3(2000.f);
	octree = new Octree(box);
}

PhysicsSystem::~PhysicsSystem()
{
	gPhysicsSystem = nullptr;

	delete octree;
	octree = nullptr;

	delete thMoveBodies;
	thMoveBodies = nullptr;

	delete thPostSyncBodies;
	thPostSyncBodies = nullptr;

	delete th;
	th = nullptr;

	delete thOctInsertion;
	thOctInsertion = nullptr;

	delete thResolveNarrowCollision;
	thResolveNarrowCollision = nullptr;

	registry->on_construct<CollisionComponent>().disconnect<&PhysicsSystem::OnConstructCollider>(this);
	registry->on_construct<PhysicsComponent>().disconnect<&PhysicsSystem::OnConstructBody>(this);
}

void PhysicsSystem::Init()
{
	auto entity = world->CreateEntity("Instanced Physics Balls");
	instancedPhysicsBalls = entity.GetEntityId();
	auto& mesh = entity.AddComponent<StaticMeshInstancedComponent>().staticMeshInstanced;

	glm::vec3 color = glm::vec3(0.1f, 0.1f, 0.7f);

	mesh.LOD[0].AddMesh(world->GetMeshManager()->CreateOctahedronBall(4));
	mesh.LOD[1].AddMesh(world->GetMeshManager()->CreateOctahedronBall(3));
	mesh.LOD[2].AddMesh(world->GetMeshManager()->CreateOctahedronBall(2));
	mesh.LOD[3].AddMesh(world->GetMeshManager()->CreateOctahedronBall(1));

	for (size_t i = 0; i < 4; i++)
	{
		mesh.LOD[i].cullFace = GL_BACK;
		mesh.LOD[i].SetColor(color);
		mesh.LOD[i].bCastShadow = false;
	}
}

void PhysicsSystem::SetCastShadowOnPhysicBalls(bool bCast)
{
	Entity ent(instancedPhysicsBalls, world);
	auto& mesh = ent.GetComponent<StaticMeshInstancedComponent>().staticMeshInstanced;

	for (size_t i = 0; i < 4; i++)
	{
		mesh.LOD[i].bCastShadow = bCast;
	}
}

void PhysicsSystem::SetShaderModelOnPhysicsBalls(int shaderModel)
{
	Entity ent(instancedPhysicsBalls, world);
	auto& mesh = ent.GetComponent<StaticMeshInstancedComponent>().staticMeshInstanced;

	for (size_t i = 0; i < 4; i++)
	{
		mesh.LOD[i].shaderModel = shaderModel;
	}
}

void PhysicsSystem::OnConstructCollider(entt::registry& registry, entt::entity entity)
{
	Entity ent(entity, world);
	if (ent.HasComponent<CollisionComponent>())
	{
		auto& col = ent.GetComponent<CollisionComponent>().col;
		RegisterCollider(col);

		if (ent.HasComponent<PhysicsComponent>())
		{
			auto& body = ent.GetComponent<PhysicsComponent>().body;
			body.collider = col;
		}
	}
}

void PhysicsSystem::OnConstructBody(entt::registry& registry, entt::entity entity)
{
	Entity ent(entity, world);
	auto& body = ent.GetComponent<PhysicsComponent>().body;
	body.entityId = entity;

	if (ent.HasComponent<CollisionComponent>())
	{
		auto& col = ent.GetComponent<CollisionComponent>().col;
		body.collider = col;
	}
}

void PhysicsSystem::RegisterCollider(Collideable& inOutCollider)
{
	auto& np = GetNarrowPhaseData();
	if (inOutCollider.type == CollideableType::Sphere)
	{
		BoundingSphere s;
		s.SetCenter(glm::vec3(0.f, 0.f, 0.f));
		s.SetRadius(1.f);
		np.mSpheres.emplace_back(s);

		inOutCollider.shapeIndex = np.mSpheres.size() - 1;
	};
	if (inOutCollider.type == CollideableType::ConvexHull)
	{
		ConvexHull ch;
		np.mConvexHulls.emplace_back(ch);
		inOutCollider.shapeIndex = np.mConvexHulls.size() - 1;
	};
	if (inOutCollider.type == CollideableType::Capsule)
	{
		BoundingCapsule caps;
		caps.SetCenter(glm::vec3(0.f, 1.f, 0.f));
		caps.SetRadius(1.f);

		np.mCapsules.emplace_back(caps);
		inOutCollider.shapeIndex = np.mCapsules.size() - 1;
	};
}

void PhysicsSystem::RegisterHeightmap(const CollisionHeightmap& hm)
{
	mNp.heightmap = hm;
}

const BroadPhase& PhysicsSystem::GetBroadPhaseData() const
{
	return mBp;
}

const NarrowPhase& PhysicsSystem::GetNarrowPhaseData() const
{
	return mNp;
}

const Octree* PhysicsSystem::GetOctree() const
{
	return octree;
}

void PhysicsSystem::DrawConvexHulls() const
{
	auto* rd = RenderDebugger::Get();
	for (auto& convexHull : mNp.mConvexHulls)
	{
		for (size_t i = 0; i < convexHull.geo.faces.size(); i++)
		{
			auto& face = convexHull.geo.faces[i];
			const glm::vec3 a = convexHull.transform.GetTransform() * glm::vec4(convexHull.geo.vertices[face.a], 1.f);
			const glm::vec3 b = convexHull.transform.GetTransform() * glm::vec4(convexHull.geo.vertices[face.b], 1.f);
			const glm::vec3 c = convexHull.transform.GetTransform() * glm::vec4(convexHull.geo.vertices[face.c], 1.f);

			rd->AddDebugSegment(0.f, a, b, glm::vec3(0.f, 0.f, 1.f));
			rd->AddDebugSegment(0.f, b, c, glm::vec3(0.f, 0.f, 1.f));
			rd->AddDebugSegment(0.f, a, c, glm::vec3(0.f, 0.f, 1.f));
		}
	}
}

void PhysicsSystem::DrawBoundingBoxes() const
{
	auto* rd = RenderDebugger::Get();
	auto view = registry->view<CollisionComponent>();
	for (auto entity : view)
	{
		auto& collidable = view.get<CollisionComponent>(entity).col;
		auto bounds = collidable.GetLocalBounds();
		rd->AddDebugBoundingBox(0.f, bounds.min, bounds.max, glm::vec3(0.f, 1.f, 0.f));
	}
}

void PhysicsSystem::Update(float deltatime)
{
	auto* rd = RenderDebugger::Get();
	ImGui::Begin("PhysicSystem debug");
	ImGui::Checkbox("Enable threading(32threads)", &bSimulateThreaded);
	if (!bSimulateThreaded)
	{		
		ImGui::Checkbox("Enable Step simulation(press q to simulate next step)", &bEnableStepMode);
		ImGui::SliderFloat("Step Value: ", &stepValue, 0.00001f, 0.016f, "ratio = %f", 1.f);
		ImGui::SliderFloat("Debug render timer: ", &debugRenderTimer, 0.f, 1.f, "ratio = %f", 1.f);
		ImGui::Checkbox("Show heightmap collision triangles", &bShowHeightmapCollisionTriangles);
		ImGui::Checkbox("Show CollisionNormal", &bShowCollisionNormal);
		ImGui::ColorEdit3("Collision Normal Color", collisonNormalColor);
		ImGui::Checkbox("Show aCollisionPoints", &bShowACollisionPoints);
		ImGui::ColorEdit3("aCollisionPoints color", aCollisionPointsColor);
		ImGui::Checkbox("Show bCollisionPoints", &bShowBCollisionPoints);
		ImGui::ColorEdit3("bCollisionPoints color", bCollisionPointsColor);
		ImGui::Checkbox("Show aVelocity", &bShowAVelocity);
		ImGui::ColorEdit3("aVel color", colorAVel);
		ImGui::Checkbox("Show bVelocity", &bShowBVelocity);
		ImGui::ColorEdit3("bVel color", colorBVel);
		ImGui::Checkbox("Show aImpulse", &bShowAImpulse);
		ImGui::ColorEdit3("aImpulse color", colorAImpulse);
		ImGui::Checkbox("Show bImpulse", &bShowBImpulse);
		ImGui::ColorEdit3("bImpulse color", colorBImpulse);
		ImGui::Checkbox("Show aTangentImpulse", &bShowATangentImpulse);
		ImGui::ColorEdit3("aTangentImpulse color", colorATangentImpulse);
		ImGui::Checkbox("Show bTangentImpulse", &bShowBTangentImpulse);
		ImGui::ColorEdit3("bTangentImpulse color", colorBTangentImpulse);
		
	}
	ImGui::End();
	if (bEnableStepMode)
	{
		deltatime = stepValue;
	}

	this->deltatime = deltatime;

	octLeafsCache.clear();

	//Reloading octree
	delete octree; octree = new Octree(octreeSize);

	if (bSimulateThreaded)
	{
		thMoveBodies->StartWork();
		thMoveBodies->WaitWork();

		th->StartWork();
		thOctInsertion->StartWork();

		th->WaitWork();
		thOctInsertion->WaitWork();

		octree->GetOctreeLeafs(octLeafsCache);

		thResolveNarrowCollision->StartWork();
		thResolveNarrowCollision->WaitWork();

		auto ent = Entity(instancedPhysicsBalls, world);
		auto& smm = ent.GetComponent<StaticMeshInstancedComponent>();
		smm.staticMeshInstanced.transforms.clear();
		smm.staticMeshInstanced.transforms.resize(registry->view<PhysicsBall>().size());

		thPostSyncBodies->StartWork();
		thPostSyncBodies->WaitWork();
	}
	else
	{
		PreSyncTransforms();
		
		ApplyForces(deltatime);
		
		MoveBodies(deltatime);

		auto view = registry->view<CollisionComponent>();
		for (auto entity : view)
		{
			auto& collidable = view.get<CollisionComponent>(entity).col;
			InsertOctree(collidable, entity);
			ResolveHeightmapCollisions(collidable, entity);
		}

		std::vector<CollisionPair> collisionPairs;
		octree->FindCollisionPairs(collisionPairs);
		for (auto& pair : collisionPairs)
		{
			ResolveNarrowCollisions(pair);
		}

		PostSyncTransforms();
	}	
}

void PhysicsSystem::ApplyForces(float deltatime)
{
	auto view = registry->view<PhysicsComponent>();
	for (auto entity : view)
	{
		Entity ent(entity, world);
		auto& body = view.get<PhysicsComponent>(entity).body;

		glm::vec3 gravityImpulse = gravity * body.GetMass() * deltatime;
		body.ApplyLinearImpulse(gravityImpulse);
		body.ApplyLinearImpulse(externalImpulse * deltatime);
	}
}

void PhysicsSystem::MoveBodies(float deltatime)
{
	auto view = registry->view<PhysicsComponent>();
	for (auto entity : view)
	{
		Entity ent(entity, world);
		auto& body = view.get<PhysicsComponent>(entity).body;
		body.Update(deltatime);
	}
}

void PhysicsSystem::ComputeContacts(std::vector<ContactManifold>& outContacts)
{
	
}

void PhysicsSystem::THMoveBodies(int thIndex)
{
	auto view = registry->view<PhysicsComponent>();
	for (int i = thIndex; i < view.size(); i += NUM_THREADS)
	{
		Entity ent(view[i], world);
		auto& body = view.get<PhysicsComponent>(view[i]).body;
		auto& transform = ent.GetComponent<TransformComponent>();

		body.SetPos(transform.GetPosition());
		body.SetRotation(transform.GetRotation());

		glm::vec3 gravityImpulse = gravity * body.GetMass() * deltatime;
		body.ApplyLinearImpulse(gravityImpulse);
		body.ApplyLinearImpulse(externalImpulse * deltatime);

		body.Update(deltatime);
	}
}

void PhysicsSystem::InsertOctree(Collideable& collidable, entt::entity entity)
{
	OctreeData octData;
	octData.mEntity = entity;
	//GetLocalBounds will return the world position now since it got synced above.
	octData.mBounds = collidable.GetLocalBounds();

	Entity ent(entity, world);
	
	if (ent.HasComponent<PhysicsComponent>())
	{
		auto& body = ent.GetComponent<PhysicsComponent>().body;

		if (Cmp(body.GetMass(), 0.f))
		{
			octData.bStatic = true;
		}
	}

	octree->Insert(octData);
}

void PhysicsSystem::THInsertOctree(int thIndex)
{
	auto view = registry->view<CollisionComponent>();
	for (int i = thIndex; i < view.size(); i += NUM_THREADS)
	{
		auto& collidable = view.get<CollisionComponent>(view[i]).col;
		InsertOctree(collidable, view[i]);
	}
}

void PhysicsSystem::ResolveHeightmapCollisions(Collideable& collideable, entt::entity entity)
{
	if (!mNp.heightmap.heightMap) return;

	auto* rd = RenderDebugger::Get();

	Entity entA(entity, world);

	PhysicsComponent* bodyComp;

	if (entA.HasComponent<PhysicsComponent>())
	{
		bodyComp = &entA.GetComponent<PhysicsComponent>();
	}

	if (Cmp(bodyComp->body.GetMass(), 0.f)) return;

	ContactManifold contact; CollisionHeightmapTriangles cht; RigidBody staticBody; Collideable temp; temp.type = CollideableType::Invalid;
	staticBody.SetFriction(0.5f); staticBody.SetMass(0.f); staticBody.collider = temp;

	if (mNp.heightmap.Intersect(collideable.GetLocalBounds(), cht, bShowHeightmapCollisionTriangles))
	{
		std::vector<Triangle> tets = cht.GetAsTriangles(mNp.heightmap.heightMap, mNp.heightmap.scale.x, mNp.heightmap.scale.y, mNp.heightmap.width, bShowHeightmapCollisionTriangles);
		
		for (auto& tri : tets)
		{
			ContactManifold contact;
			if (collideable.type == CollideableType::Sphere)
			{ 
				staticBody.SetPos((tri.a + tri.b + tri.c) * 0.33f);
				if (IntersectSphereTriangle(mNp.mSpheres[collideable.shapeIndex], tri, contact))
				{
					if (entA.HasComponent<BSplines>())
					{
						entA.GetComponent<BSplines>().bSimulate = true;
					}

					ResolveCollision(bodyComp->body, staticBody, contact);
				}
			}
			else
			{
				glm::vec3 d = tri.a - glm::vec3(0.f, 1.f, 0.f);

				BoundingTetrahedron btet(tri.a, tri.b, tri.c, d);
				staticBody.SetPos((btet.pts[0] + btet.pts[1] + btet.pts[2] + btet.pts[3]) * 0.25f);

				if (IntersectConvexPoly(collideable, bodyComp->body, btet, staticBody, contact))
				{
					ResolveCollision(bodyComp->body, staticBody, contact);
				}
			}
		}
	}
}

void PhysicsSystem::THResolveHeightmapCollisions(int thIndex)
{
	auto view = registry->view<CollisionComponent>();
	for (size_t i = thIndex; i < view.size(); i+= NUM_THREADS)
	{
		auto &collidable = view.get<CollisionComponent>(view[i]).col;
		ResolveHeightmapCollisions(collidable, view[i]);
	}
}

void PhysicsSystem::ResolveNarrowCollisions(CollisionPair& pair)
{
	auto [aCol, aBody] = registry->get<CollisionComponent, PhysicsComponent>(pair.a);
	auto [bCol, bBody] = registry->get<CollisionComponent, PhysicsComponent>(pair.b);

	if (Cmp(aBody.body.GetMass(), 0.f) && Cmp(bBody.body.GetMass(), 0.f)) return;
	
	ContactManifold contact;
	if (Collision::Intersect(aCol.col, aBody.body, bCol.col, bBody.body, mNp, contact))
	{
		ResolveCollision(contact);
	}
}

void PhysicsSystem::THResolveNarrowCollisions(int thIndex)
{
	for (int i = thIndex; i < octLeafsCache.size(); i += NUM_THREADS)
	{
		std::vector<CollisionPair> collisionPairs;
		octLeafsCache[i]->FindCollisionPairs(collisionPairs);

		for (int j = 0; j < collisionPairs.size(); j++)
		{				
			auto& pair = collisionPairs[j];
			ResolveNarrowCollisions(pair);
		}
	}
}

void PhysicsSystem::THPostSyncBodies(int thIndex)
{
	auto ent = Entity(instancedPhysicsBalls, world);
	auto& smm = ent.GetComponent<StaticMeshInstancedComponent>();

	auto view = registry->view<PhysicsComponent>();
	for (int i = thIndex; i < view.size(); i += NUM_THREADS)
	{
		auto& body = view.get<PhysicsComponent>(view[i]);
		auto& transform = registry->get<TransformComponent>(view[i]);

		transform.SetPosition(body.body.GetPos());
		transform.SetRotation(body.body.GetRotation());

		Entity otherEnt(view[i], world);

		if (otherEnt.HasComponent<PhysicsBall>())
		{
			auto& smm = ent.GetComponent<StaticMeshInstancedComponent>();
			smm.staticMeshInstanced.transforms[i] = transform.GetTransform();
		}
	}
}

void PhysicsSystem::ResolveCollision(ContactManifold& contact)
{
	auto& aBody = Entity(contact.aId, world).GetComponent<PhysicsComponent>().body;
	auto& bBody = Entity(contact.bId, world).GetComponent<PhysicsComponent>().body;

	ResolveCollision(aBody, bBody, contact);
}

void PhysicsSystem::ResolveCollision(RigidBody& aBody, RigidBody& bBody, ContactManifold& contact)
{
	auto* rd = RenderDebugger::Get();

	float aInvMass = aBody.GetInvMass();
	float bInvMass = bBody.GetInvMass();

	const glm::vec3& ptOnA = contact.ptOnA;
	const glm::vec3& ptOnB = contact.ptOnB;

	if (bShowACollisionPoints)
	{
		rd->AddDebugSphere(debugRenderTimer, ptOnA, 0.1f, glm::vec3(aCollisionPointsColor[0], aCollisionPointsColor[1], aCollisionPointsColor[2]));
	}
	if (bShowBCollisionPoints)
	{
		rd->AddDebugSphere(debugRenderTimer, ptOnB, 0.1f, glm::vec3(bCollisionPointsColor[0], bCollisionPointsColor[1], bCollisionPointsColor[2]));
	}

	float aElasicity = aBody.elasticity;
	float bElasicity = bBody.elasticity;

	glm::mat3 aInvInertiaWorld(aBody.GetInverseInertiaTensorWorld());
	glm::mat3 bInvInertiaWorld(bBody.GetInverseInertiaTensorWorld());

	float invMassSum = aInvMass + bInvMass;
	float elasicity = aElasicity * bElasicity;

	if (IsNan(contact.normal))
	{
		contact.normal = glm::vec3(1.f, 0.f, 0.f);
	}

	glm::vec3 n = contact.normal;

	if (bShowCollisionNormal)
	{
		rd->AddDebugSegment(0.f, ptOnA, ptOnA - n, glm::vec3(collisonNormalColor[0], collisonNormalColor[1], collisonNormalColor[2]));
	}

	const glm::vec3 ra = ptOnA - aBody.GetCenterOfMassWorld();
	const glm::vec3 rb = ptOnB - bBody.GetCenterOfMassWorld();

	const auto raXn = glm::cross(ra, n);
	const auto rbXn = glm::cross(rb, n);

	const glm::vec3 angJA = glm::cross(aInvInertiaWorld * raXn, ra);
	const glm::vec3 angJB = glm::cross(bInvInertiaWorld * rbXn, rb);

	const float angFactor = glm::dot(angJA + angJB, n);

	/*World space velocity of motion and rotation*/
	const glm::vec3 aVel = aBody.linearVelocity + glm::cross(aBody.angularVelocity, ra);
	const glm::vec3 bVel = bBody.linearVelocity + glm::cross(bBody.angularVelocity, rb);
	const glm::vec3 relVel = aVel - bVel;

	if (bShowAVelocity)
	{
		rd->AddDebugSegment(debugRenderTimer, aBody.GetPos(), aBody.GetPos() + aVel, glm::vec3(colorAVel[0], colorAVel[1], colorAVel[2]));
	}
	if (bShowBVelocity)
	{
		rd->AddDebugSegment(debugRenderTimer, bBody.GetPos(), bBody.GetPos() + bVel, glm::vec3(colorBVel[0], colorBVel[1], colorBVel[2]));
	}
	/*Impulse by collison*/
	const float num = (1.f + elasicity) * glm::dot(relVel, n);
	const float det = invMassSum + angFactor;

	const float J = num / det;
	const glm::vec3 vecJ = n * J;

	if (!Cmp(aInvMass, 0.f))
		aBody.ApplyImpulse(ptOnA, -vecJ);

	if (!Cmp(bInvMass, 0.f))
		bBody.ApplyImpulse(ptOnB, vecJ);

	if (bShowAImpulse)
	{
		rd->AddDebugSegment(debugRenderTimer, ptOnA, ptOnA - vecJ, glm::vec3(colorAImpulse[0], colorAImpulse[1], colorAImpulse[2]));
	}
	if (bShowBImpulse)
	{
		rd->AddDebugSegment(debugRenderTimer, ptOnB, ptOnB + vecJ, glm::vec3(colorBImpulse[0], colorBImpulse[1], colorBImpulse[2]));
	}

	/*Impulse by friction*/
	const float frictionA = aBody.friction;
	const float frictionB = bBody.friction;
	const float friction = frictionA * frictionB;

	/*Velocity of the collision normal with respect to relative velocity */
	const glm::vec3 velNorm = n * glm::dot(n, relVel);

	const glm::vec3 velTang = relVel - velNorm;

	glm::vec3 relVelTangDir = velTang;
	SafeNormal(relVelTangDir);

	glm::vec3 raXtang = glm::cross(ra, relVelTangDir);
	glm::vec3 rbXtang = glm::cross(rb, relVelTangDir);

	const glm::vec3 inertiaA = glm::cross(aInvInertiaWorld * raXtang, ra);
	const glm::vec3 inertiaB = glm::cross(bInvInertiaWorld * rbXtang, rb);
	const float invInertia = glm::dot(inertiaA + inertiaB, relVelTangDir);

	const float reducedMass = 1.0f / (invMassSum + invInertia);
	const glm::vec3 impulseFriction = velTang * reducedMass * friction;

	if (!Cmp(aInvMass, 0.f))
		aBody.ApplyImpulse(ptOnA, -impulseFriction);

	if (!Cmp(bInvMass, 0.f))
		bBody.ApplyImpulse(ptOnB, impulseFriction);

	if (bShowATangentImpulse)
	{
		rd->AddDebugSegment(debugRenderTimer, ptOnA, ptOnA - impulseFriction, glm::vec3(colorATangentImpulse[0], colorATangentImpulse[1], colorATangentImpulse[2]));
	}
	if (bShowBTangentImpulse)
	{
		rd->AddDebugSegment(debugRenderTimer, ptOnB, ptOnB + impulseFriction, glm::vec3(colorBTangentImpulse[0], colorBTangentImpulse[1], colorBTangentImpulse[2]));
	}

	const float tA = aBody.GetInvMass() / invMassSum;
	const float tB = bBody.GetInvMass() / invMassSum;

	const glm::vec3 ds = ptOnB - ptOnA;

	if (!Cmp(aInvMass, 0.f))
		aBody.SetPos(aBody.GetPos() + tA * ds);

	if (!Cmp(bInvMass, 0.f))
		bBody.SetPos(bBody.GetPos() - tB * ds);
}

void PhysicsSystem::ResolveContacts(std::vector<ContactManifold>& inContacts)
{
	for (size_t i = 0; i < inContacts.size(); i++)
	{	
		ResolveCollision(inContacts[i]);
	}
}

void PhysicsSystem::PreSyncTransforms()
{
	auto view = registry->view<TransformComponent, PhysicsComponent>();
	for (auto entity : view)
	{
		auto [transform, body] = view.get<TransformComponent, PhysicsComponent>(entity);
		body.body.SetPos(transform.GetPosition());
		body.body.SetRotation(transform.GetRotation());
	}
}

void PhysicsSystem::PostSyncTransforms()
{
	auto ent = Entity(instancedPhysicsBalls, world);
	auto& smm = ent.GetComponent<StaticMeshInstancedComponent>();
	smm.staticMeshInstanced.transforms.clear();

	auto view = registry->view<TransformComponent, PhysicsComponent>();
	for (auto entity : view)
	{
		auto [transform, body] = view.get<TransformComponent, PhysicsComponent>(entity);	
		transform.SetPosition(body.body.GetPos());
		transform.SetRotation(body.body.GetRotation());		

		Entity otherEnt(entity, world);

		if (otherEnt.HasComponent<PhysicsBall>())
		{
			auto& smm = ent.GetComponent<StaticMeshInstancedComponent>();
			smm.staticMeshInstanced.transforms.emplace_back(transform.GetTransform());
		}
	}
}

BroadPhase& PhysicsSystem::GetBroadPhaseDate()
{
	return mBp;
}

NarrowPhase& PhysicsSystem::GetNarrowPhaseData()
{
	return mNp;
}

