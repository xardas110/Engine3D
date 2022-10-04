#pragma once
#include "RigidBody.h"
#include "MathSolvers.h"
#include "Include/glm/glm.hpp"
#include "Include/entt/entt.hpp"

struct Entity;

class Constraint
{
	friend class PhysicsSystem;
protected:
	virtual void PreSolve(float deltatime) {};
	virtual void Solve() {};
	virtual void PostSolve() {};

	MatMN GetInverseMassMatrix() const;
	VecN GetVelocities() const;
	void ApplyImpulses(const VecN& impulses);

	entt::entity entityA;
	entt::entity entityB;

	glm::vec3 anchorA{0.f};
	glm::vec3 axisA{ 0.f };

	glm::vec3 anchorB{ 0.f };
	glm::vec3 axisB{ 0.f };
};

class ConstraintPenetration : public Constraint
{
	friend class PhysicsSystem; friend class ConstraintManifold;
	ConstraintPenetration()
		: Constraint(), mCachedLambda(3), mJacobian(3, 12)
	{
		mCachedLambda.Zero();
	};

	virtual void PreSolve(float deltatime) override;
	virtual void Solve() override;

	VecN mCachedLambda;
	glm::vec3 mNormal{ 0.f, 1.f, 0.f }; //in a's local space

	MatMN mJacobian;
	float mBaumgarte{0.f};
	float mFriction{0.f};
};