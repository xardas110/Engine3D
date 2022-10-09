#include "PhysX.h"
#include "Components.h"
#include <iostream>
#include "Entity.h"

using namespace physx;

void PhysX::Init(entt::registry* registry)
{
	this->registry = registry;

	std::cout << "Creating foundation" << std::endl;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocatorCallback, mErrorCallback);

	if (!mFoundation)
	{
		std::cout << "Failed to create physics foundation" << std::endl;
		return;
	}

	mToleranceScale.length = 100.f;
	mToleranceScale.speed = 981;

	mPvd = PxCreatePvd(*mFoundation);
	transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		mToleranceScale, true, mPvd);

	if (!mPhysics)
	{ 
		std::cout << "Failed to create physics scene " << std::endl;
		return;
	}

	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());	
	sceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	
	//mDispatcher = PxDefaultCpuDispatcherCreate(32);
	//sceneDesc.cpuDispatcher = mDispatcher;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	
	mScene = mPhysics->createScene(sceneDesc);

	if (!mScene)
	{
		std::cout << "Failed to create physics scene " << std::endl;
		return;
	}

	PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();

	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	std::cout << "Successfully created physX" << std::endl;
	Test();
}

void PhysX::Test()
{
	auto* world = World::Get();
	auto sm = world->GetStaticMeshManager();
	{
		Entity e = world->CreateEntity("BoXGround");
		auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
		sm->LoadStaticMesh("../3dprog22/assets/models/box/box.obj", boxMesh);
		boxMesh.SetColor({ 1.f, 0.f, 0.f });
		boxMesh.bCastShadow = false;

		auto& body = e.AddComponent<PhysXBody>().body;

		auto mat = mPhysics->createMaterial(1.f, 0.f, 0.f);
	
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(500.f, 500.f, 500.f), *mat);
		body = mPhysics->createRigidDynamic(physx::PxTransform());
		body->attachShape(*shape);
		mScene->addActor(*body);
	
		e.SetScale({ 1000.0f, 1000.0f, 1000.0f });
		e.SetPosition({ 0.f, -1000.f, 0.f });
	}
	{
		Entity e = world->CreateEntity("box2");
		auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
		sm->LoadStaticMesh("../3dprog22/assets/models/box/box.obj", boxMesh);
		boxMesh.SetColor({ 1.f, 0.f, 0.f });
		boxMesh.bCastShadow = false;

		auto& body = e.AddComponent<PhysXBody>().body;

		auto mat = mPhysics->createMaterial(1.f, 0.f, 0.f);

		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(5.f, 5.f, 5.f), *mat);
		body = mPhysics->createRigidDynamic(physx::PxTransform());
		body->attachShape(*shape);
		body->setMass(10.f);
		mScene->addActor(*body);

		e.SetScale({ 5.0f, 5.0f, 5.0f });
		e.SetPosition({ 0.f, 500.f, 0.f });

		shape->release();
	}
}

void PhysX::Update(float deltatime)
{
	auto view = registry->view<TransformComponent, PhysXBody>();

	for (auto entity: view)
	{
		auto [trans, body] = view.get<TransformComponent, PhysXBody>(entity);
		PxTransform transform; transform.p = PxVec3( trans.GetPosition().x, trans.GetPosition().y, trans.GetPosition().z);
		transform.q = PxQuat(trans.GetRotation().x, trans.GetRotation().y, trans.GetRotation().z, trans.GetRotation().w);
		body.body->setGlobalPose(transform);
	}

	mScene->simulate(deltatime);
	mScene->fetchResults(true);

	for (auto entity : view)
	{
		auto [trans, body] = view.get<TransformComponent, PhysXBody>(entity);
		PxTransform transform = body.body->getGlobalPose();
		trans.SetPosition(glm::vec3(transform.p.x, transform.p.y, transform.p.z));
		trans.SetRotation(glm::quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
	}
}

void PhysX::Clean()
{	
	transport->disconnect();
	mPvd->disconnect();

	mScene->release();
	transport->release();
	mPvd->release();
	mPhysics->release();
	mFoundation->release();
}
