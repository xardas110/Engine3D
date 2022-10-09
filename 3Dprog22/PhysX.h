#pragma once

#include "Include/PhysX/PxPhysics.h"
#include "Include/PhysX/PxPhysicsAPI.h"
#include "foundation/PxErrors.h"
#include "Include/entt/entt.hpp"
#include <iostream>

class UserErrorCallback : public physx::PxErrorCallback
{
public:
	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file,
		int line)
	{
		std::cout << "Recieved error from physx:  " << std::endl;
		std::cout << message << std::endl;
	}
};

class PhysX
{
	friend class PhysicsSystem;

	void Init(entt::registry* registry);

	void Test();

	void Update(float deltatime);

	void Clean();

	UserErrorCallback mErrorCallback;
	physx::PxDefaultAllocator mAllocatorCallback;
	physx::PxFoundation* mFoundation{ nullptr };
	physx::PxDefaultCpuDispatcher* mDispatcher{ nullptr };
	physx::PxMaterial* mMaterial{ nullptr };
	physx::PxTolerancesScale mToleranceScale;
	physx::PxPvd* mPvd{ nullptr };
	physx::PxPvdTransport* transport{ nullptr };
public:
	physx::PxPhysics* mPhysics{ nullptr };
	physx::PxScene* mScene{ nullptr };

	entt::registry* registry;
};

