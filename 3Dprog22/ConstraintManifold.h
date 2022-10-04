#pragma once
#include "ContactManifold.h"
#include "Constraint.h"

class ConstraintManifold
{
	friend class ConstraintManifoldCollector;
public:
	ConstraintManifold() {};

	void AddContact(const ContactManifold& contact);
	void RemoveExpiredContacts();

	void PreSolve(float deltatime);
	void Solve();
	void PostSolve();

	ContactManifold GetContact(int i) const { return mContacts[i]; }
	int GetNumContacts() const { return mNumContacts; }
private:
	static constexpr int MAX_CONTACTS{ 4 };
	ContactManifold mContacts[MAX_CONTACTS];

	int mNumContacts{ 0 };

	entt::entity mEntityA{ entt::tombstone };
	entt::entity mEntityB{ entt::tombstone };

	ConstraintPenetration mConstraints[MAX_CONTACTS];
};

class ConstraintManifoldCollector
{
	friend class PhysicsSystem;
	void AddContact(const ContactManifold& contact);

	void PreSolve(float deltatime);
	void Solve();
	void PostSolve();

	void RemoveExpired();

	void Clear() { mManifolds.clear(); }

	std::vector<ConstraintManifold> mManifolds;
};