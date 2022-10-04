#pragma once
#include <Windows.h>

typedef void (*CPhysicsSimulateFunction)(float, struct CPhysics_MemoryBlock*);

typedef void (*CPhysicsInitFunction)(struct CPhysics_MemoryBlock*);

struct CPhysicsModule
{
	CPhysicsSimulateFunction		CPhysics_Simulate{ nullptr };

	struct CPhysics_MemoryBlock*	CPhysics_Memory{ nullptr };

	HMODULE							Instance{ nullptr };
};