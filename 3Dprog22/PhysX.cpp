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
	
	mDispatcher = PxDefaultCpuDispatcherCreate(32);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	
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

	auto* world = World::Get();
	auto sm = world->GetStaticMeshManager();

	Entity ib = world->CreateEntity("Instanced boxes");
	auto& mesh = ib.AddComponent<StaticMeshInstancedComponent>();
	sm->LoadStaticMesh("../3dprog22/assets/models/box/box.obj", mesh.staticMeshInstanced.LOD[0]);
	sm->LoadStaticMesh("../3dprog22/assets/models/box/box.obj", mesh.staticMeshInstanced.LOD[1]);
	sm->LoadStaticMesh("../3dprog22/assets/models/box/box.obj", mesh.staticMeshInstanced.LOD[2]);
	sm->LoadStaticMesh("../3dprog22/assets/models/box/box.obj", mesh.staticMeshInstanced.LOD[3]);

	this->instancedBoxes = ib.GetEntityId();

	thPreSync = new ThreadPoolx([this](int thId){this->THPreSync(thId); }, 32);
	thPostSync = new ThreadPoolx([this](int thId) {this->THPostSync(thId); }, 32);

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

		auto mat = mPhysics->createMaterial(1.f, 0.f, 0.f);
	
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(500.f, 500.f, 500.f), *mat);
		auto staticBody = mPhysics->createRigidStatic(physx::PxTransform());
		staticBody->attachShape(*shape);
		staticBody->setGlobalPose(PxTransform(physx::PxVec3(0.f, -200.f, 0.f)));
		mScene->addActor(*staticBody);
	
		e.SetScale({ 1000.0f, 1000.0f, 1000.0f });
		e.SetPosition({ 0.f, -200.f, 0.f });
	}

	int it = 7;
	int width = 20;
	int height = 20;
	int spacing = 10;
	int numBoxes = 0;
	for (size_t i = 0; i < it; i++)
	{	
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{	
				{
					Entity e = world->CreateEntity("box" + std::to_string(numBoxes++));
					auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
					auto& body = e.AddComponent<PhysXBody>().body;

					sm->LoadStaticMesh("../3dprog22/assets/models/box/box.obj", boxMesh);
					boxMesh.SetColor(glm::vec3(0.7f, 0.5f, 0.f));

					auto mat = mPhysics->createMaterial(1.f, 0.f, 0.f);

					physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(5.f, 5.f, 5.f), *mat);
					body = mPhysics->createRigidDynamic(physx::PxTransform());
					body->attachShape(*shape);
					body->setMassSpaceInertiaTensor(PxVec3(10.f, 10.f, 10.f));
					mScene->addActor(*body);
					shape->release();

					e.SetScale({ 10.0f, 10.0f, 10.0f });
					e.SetPosition({ x * spacing - ((float)width * 0.5f * spacing), 500.f + (i * spacing), y * spacing - (float)height * spacing * 0.5f });
				}
			}
		}
	}
}

void PhysX::THPreSync(int thId)
{
	auto view = registry->view<PhysXBody>();
	for (int i = thId; i < view.size(); i += 32)
	{
		auto entity = view[i];
		auto &body = view.get<PhysXBody>(entity);
		auto& trans = registry->get<TransformComponent>(entity);

		PxTransform transform; transform.p = PxVec3(trans.GetPosition().x, trans.GetPosition().y, trans.GetPosition().z);
		transform.q = PxQuat(trans.GetRotation().w, trans.GetRotation().z, trans.GetRotation().y, trans.GetRotation().x);
		body.body->setGlobalPose(transform);
	}
}

void PhysX::Update(float deltatime)
{
	auto* world = World::Get();
	Entity ib(instancedBoxes, world);
	auto& mesh = ib.GetComponent<StaticMeshInstancedComponent>();
	mesh.staticMeshInstanced.transforms.clear();
	mesh.staticMeshInstanced.transforms.resize(registry->view<PhysicsBox>().size());

	//thPreSync->StartWork();
	//thPreSync->WaitWork();
	
	auto view = registry->view<PhysXBody>();
	for (int i = 0; i < view.size(); i ++)
	{
		auto entity = view[i];
		auto& body = view.get<PhysXBody>(entity);
		auto& trans = registry->get<TransformComponent>(entity);

		PxTransform transform; transform.p = PxVec3(trans.GetPosition().x, trans.GetPosition().y, trans.GetPosition().z);
		transform.q = PxQuat(trans.GetRotation().w, trans.GetRotation().z, trans.GetRotation().y, trans.GetRotation().x);
		body.body->setGlobalPose(transform);
	}

	mScene->simulate(deltatime);
	mScene->fetchResults(true);

	for (int i = 0; i < view.size(); i ++)
	{
		auto entity = view[i];

		auto& body = view.get<PhysXBody>(entity);
		auto& trans = registry->get<TransformComponent>(entity);

		Entity ent(entity, World::Get());
		PxTransform transform = body.body->getGlobalPose();

		trans.SetPosition(glm::vec3(transform.p.x, transform.p.y, transform.p.z));
		trans.SetRotation(glm::quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w));

		if (ent.HasComponent<PhysicsBox>())
		{
			mesh.staticMeshInstanced.transforms[i] = trans.GetTransform();
		}
	}
}

void PhysX::THPostSync(int thId)
{
	auto view = registry->view<PhysXBody>();
	auto* world = World::Get();

	Entity ib(instancedBoxes, world);
	auto& mesh = ib.GetComponent<StaticMeshInstancedComponent>();

	for (int i = thId; thId < view.size(); i += 32)
	{
		auto entity = view[i];

		auto &body = view.get<PhysXBody>(entity);
		auto& trans = registry->get<TransformComponent>(entity);

		Entity ent(entity, World::Get());
		PxTransform transform = body.body->getGlobalPose();

		trans.SetPosition(glm::vec3(transform.p.x, transform.p.y, transform.p.z));
		trans.SetRotation(glm::quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w));

		if (ent.HasComponent<PhysicsBox>())
		{
			mesh.staticMeshInstanced.transforms[i] = trans.GetTransform();
		}
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

	delete thPreSync;
	delete thPostSync;
}
