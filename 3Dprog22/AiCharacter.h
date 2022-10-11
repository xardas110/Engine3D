#pragma once
#include "Entity.h"
#include <functional>

namespace AiState
{
	enum AiState
	{
		None,
		Patrolling,
		Follow
	};	
}

namespace AiMoveState
{
	enum AiMoveState
	{
		None,
		Moving,
		DestinationReached
	};
}

namespace AiPatrolState
{
	enum AiPatrolState
	{
		None,
		Function,
		WayPoints,
		Bezier
	};
}

class AiCharacter : public ScriptableEntity
{
	glm::vec3 cachedWorldUp{ 0.f, 1.f, 0.f };
	
	entt::entity entityToFollow{entt::null};
	float stopFollowDist2{ 1000.f };
	float stopFollowThreshold{ 250.f };
	
	float (*PatrolFunction)(float x) { nullptr };

	std::function<glm::vec2(float)> bezierFunc{nullptr};

	float startX{ 0.f }, endX{ 0.f };
	float currentX{ 0.f }, step{ 0.25f };
	int sign{ 1 };

	std::vector<glm::vec3> waypoints;

	glm::mat4 patrolFunctionTransform{ 1.f };
	
	glm::vec3 targetPosition{ 0.f };

	float movementSpeed{ 20.f };
	
	const float stunTimer{ 2.f };
	float currentStunTimer{ 0.f };

	bool bShowPath{ true };

	void OnCreate() override;
	
	void OnBeginPlay() override;
	
	void OnUpdate(float deltaTime) override;

	void UpdateStunTimer(float deltatime);

	void DrawPatrolFunction() const;
	void DrawWayPoints() const;
	
	AiState::AiState aiState{ AiState::None };	
	AiMoveState::AiMoveState aiMoveState{ AiMoveState::None };
	AiPatrolState::AiPatrolState aiPatrolState{ AiPatrolState::None };
	
	void SetAiState(AiState::AiState state);
	[[nodiscard]] AiState::AiState GetAiState() const;
	[[nodiscard]] bool IsAiState(AiState::AiState state) const;

	void SetMoveState(AiMoveState::AiMoveState state);
	[[nodiscard]] AiMoveState::AiMoveState GetMoveState() const;
	[[nodiscard]] bool IsMoveState(AiMoveState::AiMoveState state) const;

	void SetPatrolState(AiPatrolState::AiPatrolState state);
	[[nodiscard]] AiPatrolState::AiPatrolState GetPatrolState() const;
	[[nodiscard]] bool IsPatrolState(AiPatrolState::AiPatrolState state) const;

	void UpdateAiState(float deltaTime);
	void UpdateMovement(float deltaTime);
	
	void MoveToIn2D(const glm::vec3& pos2D);
	void RotateTo2DDirection(const glm::vec3& dir2D);
	bool HasReachedDestination();
	
	void UpdatePatrolState(float deltaTime);
	void UpdatePatrolFunction(float deltaTime);

	void UpdateFollow(float deltaTime);

	void UpdateAnimationState(float deltaTime);

	void UpdateBezierFunc(float deltatime);

	void UpdateWayPoint(float deltatime);

	// -1 current is obscured
	// -2 target is obscured
	// 1 success
	int CalculateAStarWayPts(const glm::vec3& current, const glm::vec3& target);

public:
	void SetPatrolFunctionTransform(const glm::mat4& newTransform);
	void SetPatrolFunction(float (*func)(float x), float startX, float endX, float step);
	void ResetPatrolFunction();

	void SetPatrolBezierTransform(const glm::mat4& newTransform);
	void SetPatrolBezier(std::function<glm::vec2(float)> func, float startX, float endX, float step);
	void ResetPatrolBezier();

	void SetEntityToFollow(entt::entity entity);
	void SetStopFollowDist2(float dist2);
	void SetStopFollowThreshold(float val);

	void SetMovementSpeed(const float newSpeed);
	float GetMovementSpeed() const;

	/*AStar*/
	void MoveTo(const glm::vec3& target);

	virtual void OnHit(entt::entity otherEnt) override;
};

