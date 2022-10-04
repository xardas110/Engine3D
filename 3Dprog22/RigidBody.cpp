#include "RigidBody.h"
#include "helpermath.h"

void RigidBody::SetMass(float newMass)
{
	mass = newMass;
}

float RigidBody::GetMass() const
{
	return mass;
}

float RigidBody::GetInvMass() const
{
	if (Cmp(mass, 0.f)) return 0.f;
	return 1.f/mass;
}

void RigidBody::SetPos(const glm::vec3& newPos)
{
	pos = newPos;

	if (HasCollider())
	{
		collider.SetPosition(newPos);
	}
}

glm::quat ConstructQuat(glm::vec3 n, const float angleRad)
{
	const float halfAngleRadians = 0.5f * angleRad;

	float w = cosf(halfAngleRadians);

	const float halfSine = sinf(halfAngleRadians);

	SafeNormal(n);

	float x = n.x * halfSine;
	float y = n.y * halfSine;
	float z = n.z * halfSine;

	return glm::quat(w, x, y, z);
}

void RigidBody::Update(float deltatime)
{
	SetPos(GetPos() + (linearVelocity * deltatime));

	glm::vec3 cm = GetCenterOfMassWorld();
	glm::vec3 cmToPos = pos - cm;

	glm::mat3 orient(rotation);
	glm::mat3 inertiaTensor = orient * collider.GetInertiaTensor() * glm::transpose(orient);
	glm::vec3 a = glm::inverse(inertiaTensor) * (glm::cross(angularVelocity, inertiaTensor * angularVelocity));

	angularVelocity += a * deltatime;

	glm::vec3 dAngle = angularVelocity * deltatime;
	glm::quat dq = ConstructQuat(dAngle, glm::length(dAngle));

	glm::quat newRot = dq * rotation;
	SafeNormal(newRot);
	SetRotation(newRot);

	glm::vec3 newPos = cm + dq * cmToPos;
	SetPos(newPos);
}

bool RigidBody::HasCollider() const
{
	return collider.IsValid();
}

Collideable RigidBody::GetCollider() const
{
	return collider;
}

const glm::vec3& RigidBody::GetPos() const
{
	return pos;
}

void RigidBody::SetRotation(const glm::quat& newRot)
{
	rotation = newRot;

	if (HasCollider())
	{
		collider.SetRotation(newRot);
	}
}

const glm::quat& RigidBody::GetRotation() const
{
	return rotation;
}

void RigidBody::SetFriction(const float newFriction)
{
	friction = newFriction;
}

float RigidBody::GetFriction() const
{
	return friction;
}

entt::entity RigidBody::GetEntityId() const
{
	return entityId;
}

void RigidBody::SetVelocity(const glm::vec3& newVel)
{
	linearVelocity = newVel;
}

const glm::vec3& RigidBody::GetVelocity() const
{
	return linearVelocity;
}

glm::mat3 RigidBody::GetInverseInertiaTensorLocal() const
{
	if (collider.IsValid())
	{
		return glm::inverse(collider.GetInertiaTensor()) * GetInvMass();
	}

	return glm::identity<glm::mat3>();
}

glm::mat3 RigidBody::GetInverseInertiaTensorWorld() const
{
	if (collider.IsValid())
	{
		glm::mat3 invInertiaTensor = glm::inverse(collider.GetInertiaTensor()) * GetInvMass();
		glm::mat3 orient(rotation);
		return orient * invInertiaTensor * glm::transpose(orient);
	}

	return glm::mat3(0.f);
}

glm::vec3 RigidBody::GetCenterOfMassWorld() const
{
	if (!HasCollider()) 
	{ 
		return pos;
	}

	const glm::vec3 shapeCOM = collider.GetCenterOfMass();
	return pos + rotation * shapeCOM;
}

glm::vec3 RigidBody::GetCenterOfMassLocal() const
{
	if (HasCollider())
	{
		return collider.GetCenterOfMass();
	}
	return glm::vec3(0.f);
}

glm::vec3 RigidBody::WsToLs(const glm::vec3& pt) const
{
	glm::mat3 invRot(rotation); invRot = glm::inverse(invRot);
	return invRot * (pt - GetCenterOfMassWorld());
}

glm::vec3 RigidBody::LsToWs(const glm::vec3& pt) const
{
	return GetCenterOfMassWorld() + (rotation * pt);
}

void RigidBody::ApplyLinearImpulse(const glm::vec3& impulse)
{
	linearVelocity += impulse * GetInvMass();
}

void RigidBody::ApplyAngularImpulse(const glm::vec3& impulse)
{
	if (Cmp(GetInvMass(), 0.f)) return;

	angularVelocity += GetInverseInertiaTensorWorld() * impulse;

	const float maxAngularSpeed = 30.f;

	if (glm::length2(angularVelocity) > maxAngularSpeed * maxAngularSpeed)
	{
		angularVelocity = glm::normalize(angularVelocity);
		angularVelocity *= maxAngularSpeed;
	}
}

void RigidBody::ApplyImpulse(const glm::vec3& impulsePt, const glm::vec3& impulse)
{
	if (Cmp(GetInvMass(), 0.f)) return;

	ApplyLinearImpulse(impulse);

	glm::vec3 position = GetCenterOfMassWorld();

	glm::vec3 r = impulsePt - position;
	glm::vec3 dl = glm::cross(r, impulse);

	ApplyAngularImpulse(dl);
}
