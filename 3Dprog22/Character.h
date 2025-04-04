#pragma once
#include "Entity.h"

namespace Audio
{
	enum Audio
	{
		FootSteps,
		Size
	};
}

namespace CharacterAnimationState
{
	enum State
	{
		Idle,
		Forward,
		Back,
		Stunned
	};
}

class Character : public ScriptableEntity
{
	void OnCreate() override;
	void OnBeginPlay() override;
	void OnUpdate(float deltaTime) override;

	void UpdateStunTimer(float deltatime);
	void UpdateAnimationState(float deltatime);
	void UpdateAudioState(float deltatime);
	void UpdateSpringArm(float deltaTime);

	void OnMousePressed(QMouseEvent* event);
	void OnMouseReleased(QMouseEvent* event);
	void OnMouseMove(QMouseEvent* event);
	void OnMouseHeld(std::map<int, bool>* mHeld, float deltaTime);
	void OnKeyHeld(std::map<int, bool>* mHeld, float deltaTime);

	/*z is inverted, z is forward.*/
	void GetCamera2DAxises(glm::vec3& forward, glm::vec3& right);

	void SyncCameraWithSpringArm();

	/*Rotate character by 2D velocity*/
	bool RotateTo2DVelocity();

	/*Last rotation done by keyboard presses*/
	glm::mat4 lastRotation{ 1.f };
	
	/*Character speed*/
    const float speed = 80.f;
	const float rotSpeed = 100.f;
	float currentRot = 0.f;
	int iSign = 1; //which way to rotate
	/*Last frame mouse position*/
	QPoint lastFrameMP;

	void HandleSpringArmRotation(const float mx, const float my);
	
	//Oppgave 7
	const float stunTimer = 2.f;
	float CurrentStunTimer = 0.f;

	float backwardSpeedRatio{ 0.5f };

	CharacterAnimationState::State animState = CharacterAnimationState::Idle;
	std::map<int, bool>* mouseHeld{nullptr};
	std::map<int, bool>* keyHeld{ nullptr };
public:
	Character();
	~Character();

	virtual void OnHit(entt::entity otherEnt) override;
};

