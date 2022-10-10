#pragma once
#include "Include/glm/glm.hpp"
#include "Include/entt/entt.hpp"
#include "Include/glm/gtx/norm.hpp"
#include "HelperMath.h"
#include "Collider.h"

struct RigidBody
{
	friend class Entity;
	friend class Constraint;
	friend class PhysicsSystem;
private:
	glm::vec3 pos{ 0.f };
	entt::entity entityId{entt::tombstone};
	glm::vec3 linearVelocity{0.f};
	float elasticity{ 0.2f };
	glm::vec3 angularVelocity{ glm::vec3(0.f, 0.f, 0.f) };
	float mass{100.f};
	glm::quat rotation{glm::identity<glm::quat>()};
	Collideable collider; // self init
	float friction{ 0.5f };
	void Update(float deltatime);
public:
	bool bLinearOnly{ false };
	glm::vec3 force{ 0.f };
	bool HasCollider() const;

	Collideable GetCollider() const;

	void SetMass(float newMass);
	float GetMass() const;

	float GetInvMass() const;

	void SetPos(const glm::vec3& newPos);
	const glm::vec3& GetPos() const;

	void SetVelocity(const glm::vec3& newVel);
	const glm::vec3& GetVelocity() const;

	void SetRotation(const glm::quat& newRot);
	const glm::quat& GetRotation() const;

	void SetFriction(const float newFriction);
	float GetFriction() const;
	
	void SetElasticity(float val);
	float GetElasticity() const;

	entt::entity GetEntityId() const;

	glm::mat3 GetInverseInertiaTensorLocal() const;
	glm::mat3 GetInverseInertiaTensorWorld() const;

	glm::vec3 GetCenterOfMassWorld() const;
	glm::vec3 GetCenterOfMassLocal() const;

	glm::vec3 WsToLs(const glm::vec3& pt) const;
	glm::vec3 LsToWs(const glm::vec3& pt) const;

	void ApplyLinearImpulse(const glm::vec3& impulse);
	void ApplyAngularImpulse(const glm::vec3& impulse);
	void ApplyImpulse(const glm::vec3& impulsePt, const glm::vec3& impulse);
};

