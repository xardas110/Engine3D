#include "ConstraintManifold.h"

#include "ContactManifold.h"
#include "Include/glm/gtx/norm.hpp"
#include "Constraint.h"
#include "RigidBody.h"
#include "HelperMath.h"
#include "Entity.h"

void ConstraintManifoldCollector::AddContact(const ContactManifold& contact)
{
	int foundIdx = -1;
	for (int i = 0; i < mManifolds.size(); i++) {
		const ConstraintManifold& manifold = mManifolds[i];
		bool hasA = (manifold.mEntityA == contact.aId || manifold.mEntityB == contact.aId);
		bool hasB = (manifold.mEntityA == contact.bId || manifold.mEntityB == contact.bId);
		if (hasA && hasB) {
			foundIdx = i;
			break;
		}
	}

	// Add contact to manifolds
	if (foundIdx >= 0) {
		mManifolds[foundIdx].AddContact(contact);
	}
	else {
		ConstraintManifold manifold;
		manifold.mEntityA = contact.aId;
		manifold.mEntityB = contact.bId;

		manifold.AddContact(contact);
		mManifolds.push_back(manifold);
	}
}

void ConstraintManifoldCollector::PreSolve(float deltatime)
{
	for (int i = 0; i < mManifolds.size(); i++) {
		mManifolds[i].PreSolve(deltatime);
	}
}

void ConstraintManifoldCollector::Solve()
{
	for (int i = 0; i < mManifolds.size(); i++) {
		mManifolds[i].Solve();
	}
}

void ConstraintManifoldCollector::PostSolve()
{
	for (int i = 0; i < mManifolds.size(); i++) {
		mManifolds[i].Solve();
	}
}

void ConstraintManifoldCollector::RemoveExpired()
{
	// Remove expired manifolds
	for (int i = (int)mManifolds.size() - 1; i >= 0; i--) {
		ConstraintManifold& manifold = mManifolds[i];
		manifold.RemoveExpiredContacts();

		if (0 == manifold.mNumContacts) {
			mManifolds.erase(mManifolds.begin() + i);
		}
	}
}

void ConstraintManifold::AddContact(const ContactManifold& contact_old)
{
	ContactManifold contact = contact_old;
	if (contact_old.aId != mEntityA || contact_old.bId != mEntityB) 
	{
		contact.ptLsOnA = contact_old.ptLsOnB;
		contact.ptLsOnB = contact_old.ptLsOnA;
		contact.ptOnA = contact_old.ptOnB;
		contact.ptOnB = contact_old.ptOnA;

		contact.aId = mEntityA;
		contact.bId = mEntityB;
	}

	// If this contact is close to another contact, then keep the old contact
	for (int i = 0; i < mNumContacts; i++) {
		RigidBody* bodyA = &Entity(mContacts[i].aId, World::Get()).GetComponent<PhysicsComponent>().body;
		RigidBody* bodyB = &Entity(mContacts[i].bId, World::Get()).GetComponent<PhysicsComponent>().body;

		const glm::vec3 oldA = bodyA->LsToWs(mContacts[i].ptLsOnA);
		const glm::vec3 oldB = bodyB->LsToWs(mContacts[i].ptLsOnB);

		const glm::vec3 newA = Entity(mEntityA, World::Get()).GetComponent<PhysicsComponent>().body.LsToWs(contact.ptLsOnA);
		const glm::vec3 newB = Entity(mEntityB, World::Get()).GetComponent<PhysicsComponent>().body.LsToWs(contact.ptLsOnB);

		const glm::vec3 aa = newA - oldA;
		const glm::vec3 bb = newB - oldB;

		const float distanceThreshold = 0.02f;
		if (glm::length2(aa) < distanceThreshold * distanceThreshold) {
			return;
		}
		if (glm::length2(bb) < distanceThreshold * distanceThreshold) {
			return;
		}
	}

	// If we're all full on contacts, then keep the contacts that are furthest away from each other
	int newSlot = mNumContacts;
	if (newSlot >= MAX_CONTACTS) {
		glm::vec3 avg = glm::vec3(0.f, 0.f, 0.f);
		avg += mContacts[0].ptLsOnA;
		avg += mContacts[1].ptLsOnA;
		avg += mContacts[2].ptLsOnA;
		avg += mContacts[3].ptLsOnA;
		avg += contact.ptLsOnA;
		avg *= 0.2f;

		float minDist = glm::length2(avg - contact.ptLsOnA);
		int newIdx = -1;
		for (int i = 0; i < MAX_CONTACTS; i++) 
		{
			float dist2 = glm::length2(avg - mContacts[i].ptLsOnA);

			if (dist2 < minDist) {
				minDist = dist2;
				newIdx = i;
			}
		}

		if (-1 != newIdx) {
			newSlot = newIdx;
		}
		else {
			return;
		}
	}

	mContacts[newSlot] = contact;

	mConstraints[newSlot].entityA = contact.aId;
	mConstraints[newSlot].entityB = contact.bId;
	mConstraints[newSlot].anchorA = contact.ptLsOnA;
	mConstraints[newSlot].anchorB = contact.ptLsOnB;

	glm::mat3 invARot(Entity(mEntityA, World::Get()).GetComponent<PhysicsComponent>().body.GetRotation());
	invARot = glm::inverse(invARot);
	glm::vec3 normal = invARot * -contact.normal;
	mConstraints[newSlot].mNormal = normal;
	SafeNormal(mConstraints[newSlot].mNormal);

	mConstraints[newSlot].mCachedLambda.Zero();

	if (newSlot == mNumContacts) {
		mNumContacts++;
	}
}

void ConstraintManifold::RemoveExpiredContacts()
{
	for (int i = 0; i < mNumContacts; i++) {
		ContactManifold& contact = mContacts[i];

		RigidBody* bodyA = &Entity(contact.aId, World::Get()).GetComponent<PhysicsComponent>().body;
		RigidBody* bodyB = &Entity(contact.bId, World::Get()).GetComponent<PhysicsComponent>().body;

		const glm::vec3 a = bodyA->LsToWs(contact.ptLsOnA);
		const glm::vec3 b = bodyB->LsToWs(contact.ptLsOnB);

		glm::vec3 normal = mConstraints[i].mNormal;
		normal = bodyA->GetRotation() * normal;

		const glm::vec3 ab = b - a;
		float penetrationDepth = glm::dot(normal, ab);
		glm::vec3 abNormal = normal * penetrationDepth;
		glm::vec3 abTangent = ab - abNormal;

		const float distanceThreshold = 0.02f;
		if (glm::length2(abTangent) < distanceThreshold * distanceThreshold && penetrationDepth <= 0.0f)
		{
			continue;
		}

		for (int j = i; j < MAX_CONTACTS - 1; j++)
		{
			mConstraints[j] = mConstraints[j + 1];
			mContacts[j] = mContacts[j + 1];
			if (j >= mNumContacts) {
				mConstraints[j].mCachedLambda.Zero();
			}
		}
		mNumContacts--;
		i--;
	}
}

void ConstraintManifold::PreSolve(float deltatime)
{
	for (int i = 0; i < mNumContacts; i++) {
		mConstraints[i].PreSolve(deltatime);
	}
}

void ConstraintManifold::Solve()
{
	for (int i = 0; i < mNumContacts; i++) {
		mConstraints[i].Solve();
	}
}

void ConstraintManifold::PostSolve()
{
	for (int i = 0; i < mNumContacts; i++) {
		mConstraints[i].PostSolve();
	}
}
