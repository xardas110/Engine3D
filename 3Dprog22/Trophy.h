#pragma once
#include "Entity.h"
#include "Include/glm/glm.hpp"

class Trophy : public ScriptableEntity
{
	void OnCreate() override;
	void OnBeginPlay() override;
	void OnUpdate(float deltaTime) override;
	void OnHit(std::uint32_t otherEntity);

	void RotateTrophy(float deltaTime);
	void OffsetTrophy(float deltaTime);

	float maxYOffset{ 5.f };
	float currentYOffset{ 0.f };
	float sign{ 1.f };

	float spinSpeed{ 1.f };//radians
	float moveSpeed{ 5.f };

public:
	void SetTrophyColor(const glm::vec3& newColor);
};