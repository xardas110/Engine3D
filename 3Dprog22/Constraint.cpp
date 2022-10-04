#include "Constraint.h"
#include "Include/glm/glm.hpp"
#include "HelperMath.h"
#include "Entity.h"

MatMN Constraint::GetInverseMassMatrix() const
{
	MatMN invMassMatrix(12, 12);
	invMassMatrix.Zero();

	Entity entA(entityA, World::Get()); Entity entB(entityB, World::Get());
	auto& bodyA = entA.GetComponent<PhysicsComponent>().body;
	auto& bodyB = entB.GetComponent<PhysicsComponent>().body;

	invMassMatrix.rows[0][0] = bodyA.GetInvMass();
	invMassMatrix.rows[1][1] = bodyA.GetInvMass();
	invMassMatrix.rows[2][2] = bodyA.GetInvMass();

	glm::mat3 invInertiaA = bodyA.GetInverseInertiaTensorWorld();
	for (int i = 0; i < 3; i++) {
		invMassMatrix.rows[3 + i][3 + 0] = invInertiaA[i][0];
		invMassMatrix.rows[3 + i][3 + 1] = invInertiaA[i][1];
		invMassMatrix.rows[3 + i][3 + 2] = invInertiaA[i][2];
	}

	invMassMatrix.rows[6][6] = bodyB.GetInvMass();
	invMassMatrix.rows[7][7] = bodyB.GetInvMass();
	invMassMatrix.rows[8][8] = bodyB.GetInvMass();

	glm::mat3 invInertiaB = bodyB.GetInverseInertiaTensorWorld();
	for (int i = 0; i < 3; i++) {
		invMassMatrix.rows[9 + i][9 + 0] = invInertiaB[i][0];
		invMassMatrix.rows[9 + i][9 + 1] = invInertiaB[i][1];
		invMassMatrix.rows[9 + i][9 + 2] = invInertiaB[i][2];
	}

	return invMassMatrix;
}

VecN Constraint::GetVelocities() const
{
	VecN q_dt(12);

	Entity entA(entityA, World::Get()); Entity entB(entityB, World::Get());
	auto& bodyA = entA.GetComponent<PhysicsComponent>().body;
	auto& bodyB = entB.GetComponent<PhysicsComponent>().body;

	q_dt[0] = bodyA.linearVelocity.x;
	q_dt[1] = bodyA.linearVelocity.y;
	q_dt[2] = bodyA.linearVelocity.z;

	q_dt[3] = bodyA.angularVelocity.x;
	q_dt[4] = bodyA.angularVelocity.y;
	q_dt[5] = bodyA.angularVelocity.z;

	q_dt[6] = bodyB.linearVelocity.x;
	q_dt[7] = bodyB.linearVelocity.y;
	q_dt[8] = bodyB.linearVelocity.z;

	q_dt[9] = bodyB.angularVelocity.x;
	q_dt[10] = bodyB.angularVelocity.y;
	q_dt[11] = bodyB.angularVelocity.z;

	return q_dt;
}

void Constraint::ApplyImpulses(const VecN& impulses)
{
	glm::vec3 forceInternalA(0.0f);
	glm::vec3 torqueInternalA(0.0f);
	glm::vec3 forceInternalB(0.0f);
	glm::vec3 torqueInternalB(0.0f);

	forceInternalA[0] = impulses[0];
	forceInternalA[1] = impulses[1];
	forceInternalA[2] = impulses[2];

	torqueInternalA[0] = impulses[3];
	torqueInternalA[1] = impulses[4];
	torqueInternalA[2] = impulses[5];

	forceInternalB[0] = impulses[6];
	forceInternalB[1] = impulses[7];
	forceInternalB[2] = impulses[8];

	torqueInternalB[0] = impulses[9];
	torqueInternalB[1] = impulses[10];
	torqueInternalB[2] = impulses[11];

	Entity entA(entityA, World::Get()); Entity entB(entityB, World::Get());
	auto& bodyA = entA.GetComponent<PhysicsComponent>().body;
	auto& bodyB = entB.GetComponent<PhysicsComponent>().body;

	bodyA.ApplyLinearImpulse(forceInternalA);
	bodyA.ApplyAngularImpulse(torqueInternalA);

	bodyB.ApplyLinearImpulse(forceInternalB);
	bodyB.ApplyAngularImpulse(torqueInternalB);
}

glm::vec3 GetOrtho(glm::vec3 n, glm::vec3& u, glm::vec3& v) 
{
	SafeNormal(n);

	const glm::vec3 w = (n.z * n.z > 0.9f * 0.9f) ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(0.f, 0.f, 1.f);
	u = glm::cross(w, n); SafeNormal(u);
	v = glm::cross(n, u); SafeNormal(v);
	u = glm::cross(v, n); SafeNormal(u);

	return n;
}

void ConstraintPenetration::PreSolve(float deltatime)
{
	Entity entA(entityA, World::Get()); Entity entB(entityB, World::Get());
	auto& bodyA = entA.GetComponent<PhysicsComponent>().body;
	auto& bodyB = entB.GetComponent<PhysicsComponent>().body;

	const glm::vec3 worldAnchorA = bodyA.LsToWs(anchorA);
	const glm::vec3 worldAnchorB = bodyB.LsToWs(anchorB);

	const glm::vec3 ra = worldAnchorA - bodyA.GetCenterOfMassWorld();
	const glm::vec3 rb = worldAnchorB - bodyB.GetCenterOfMassWorld();
	const glm::vec3 a = worldAnchorA; const glm::vec3 b = worldAnchorB;

	const float frictionA = bodyA.GetFriction(); const float frictionB = bodyB.GetFriction();
	mFriction = frictionA * frictionB;

	glm::vec3 u, v;
	mNormal = GetOrtho(mNormal, u, v);

	glm::vec3 normal = bodyA.GetRotation() * mNormal;
	u = bodyA.GetRotation() * u;
	v = bodyA.GetRotation() * v;

	mJacobian.Zero();

	glm::vec3 J1 = -normal;
	mJacobian.rows[0][0] = J1.x;
	mJacobian.rows[0][1] = J1.y;
	mJacobian.rows[0][2] = J1.z;

	glm::vec3 J2 = glm::cross(ra, -normal);
	mJacobian.rows[0][3] = J2.x;
	mJacobian.rows[0][4] = J2.y;
	mJacobian.rows[0][5] = J2.z;

	glm::vec3 J3 = normal;
	mJacobian.rows[0][6] = J3.x;
	mJacobian.rows[0][7] = J3.y;
	mJacobian.rows[0][8] = J3.z;

	glm::vec3 J4 = glm::cross(rb, normal);
	mJacobian.rows[0][9] = J4.x;
	mJacobian.rows[0][10] = J4.y;
	mJacobian.rows[0][11] = J4.z;

	if (mFriction > 0.0f) {
		glm::vec3 J1 = -u;
		mJacobian.rows[1][0] = J1.x;
		mJacobian.rows[1][1] = J1.y;
		mJacobian.rows[1][2] = J1.z;

		glm::vec3 J2 = glm::cross(ra, -u);
		mJacobian.rows[1][3] = J2.x;
		mJacobian.rows[1][4] = J2.y;
		mJacobian.rows[1][5] = J2.z;

		glm::vec3 J3 = u;
		mJacobian.rows[1][6] = J3.x;
		mJacobian.rows[1][7] = J3.y;
		mJacobian.rows[1][8] = J3.z;

		glm::vec3 J4 = glm::cross(rb, u);
		mJacobian.rows[1][9] = J4.x;
		mJacobian.rows[1][10] = J4.y;
		mJacobian.rows[1][11] = J4.z;
	}
	if (mFriction > 0.0f) {
		glm::vec3 J1 = -v;
		mJacobian.rows[2][0] = J1.x;
		mJacobian.rows[2][1] = J1.y;
		mJacobian.rows[2][2] = J1.z;

		glm::vec3 J2 = glm::cross(ra, -v);
		mJacobian.rows[2][3] = J2.x;
		mJacobian.rows[2][4] = J2.y;
		mJacobian.rows[2][5] = J2.z;

		glm::vec3 J3 = v;
		mJacobian.rows[2][6] = J3.x;
		mJacobian.rows[2][7] = J3.y;
		mJacobian.rows[2][8] = J3.z;

		glm::vec3 J4 = glm::cross(rb, v);
		mJacobian.rows[2][9] = J4.x;
		mJacobian.rows[2][10] = J4.y;
		mJacobian.rows[2][11] = J4.z;
	}

	const VecN impulses = mJacobian.Transpose() * mCachedLambda;
	ApplyImpulses(impulses);

	float C = glm::dot(b - a, normal);
	C = std::min(0.0f, C + 0.02f);	// Add slop
	float Beta = 0.25f;
	mBaumgarte = Beta * C / deltatime;
}

void ConstraintPenetration::Solve()
{
	const MatMN JacobianTranspose = mJacobian.Transpose();

	Entity entA(entityA, World::Get()); Entity entB(entityB, World::Get());
	auto& bodyA = entA.GetComponent<PhysicsComponent>().body;
	auto& bodyB = entB.GetComponent<PhysicsComponent>().body;

	// Build the system of equations
	const VecN q_dt = GetVelocities();
	const MatMN invMassMatrix = GetInverseMassMatrix();
	const MatMN J_W_Jt = mJacobian * invMassMatrix * JacobianTranspose;
	VecN rhs = mJacobian * q_dt * -1.f;
	rhs[0] -= mBaumgarte;

	// Solve for the Lagrange multipliers
	VecN lambdaN = LCP_GaussSeidel(J_W_Jt, rhs);

	// Accumulate the impulses and clamp to within the constraint limits
	VecN oldLambda = mCachedLambda;
	mCachedLambda += lambdaN;
	const float lambdaLimit = 0.0f;
	if (mCachedLambda[0] < lambdaLimit) {
		mCachedLambda[0] = lambdaLimit;
	}
	if (mFriction > 0.0f) {
		const float umg = mFriction * 10.0f * 1.0f / (bodyA.GetInvMass() + bodyB.GetInvMass());
		const float normalForce = fabsf(lambdaN[0] * mFriction);
		const float maxForce = (umg > normalForce) ? umg : normalForce;

		if (mCachedLambda[1] > maxForce) {
			mCachedLambda[1] = maxForce;
		}
		if (mCachedLambda[1] < -maxForce) {
			mCachedLambda[1] = -maxForce;
		}

		if (mCachedLambda[2] > maxForce) {
			mCachedLambda[2] = maxForce;
		}
		if (mCachedLambda[2] < -maxForce) {
			mCachedLambda[2] = -maxForce;
		}
	}
	lambdaN = mCachedLambda - oldLambda;

	// Apply the impulses
	const VecN impulses = JacobianTranspose * lambdaN;
	ApplyImpulses(impulses);
}
