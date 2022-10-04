#include "Trophy.h"

#include "Trophy.h"
#include "World.h"
#include "Include/glm/gtx/quaternion.hpp"
#include "StaticMeshManager.h"
#include "Include/entt/entt.hpp"
#include "Debug.h"

void Trophy::OnCreate()
{
	auto* world = GetWorld();
	auto* smm = world->GetStaticMeshManager();

	//auto& collision = AddComponent<OBBCollisionComponent>((std::uint32_t)GetEntityId()).collisionVolume;
	auto& mesh = AddComponent<StaticMeshComponent>().staticMeshInstanced;
	auto& tag = GetComponent<TagComponent>().tag;

	if (tag.find("EnemyTrophy") != std::string::npos)
	{
		SetIgnorePathFinding(true);
	}

	//collision.BindOnHit([this](std::uint32_t otherEntity) {this->OnHit(otherEntity); });
	//collision.SetExtents({4.f, 6.f, 4.f});
	//collision.SetLocalPos({ 0.f, -2.f, 0.f });

	if (!smm->LoadStaticMesh("../3Dprog22/Assets/Models/statue/statue.obj", mesh))
	{
		std::cout << "Failed to load static mesh for trophy" << std::endl;
	}

	if ((rand() % 2) == 0) sign = 1.f; // Just to make them move different, can be better.
	else sign = -1.f;
}

void Trophy::OnBeginPlay()
{
}

void Trophy::OnUpdate(float deltaTime)
{
	RotateTrophy(deltaTime);
	OffsetTrophy(deltaTime);
}

void Trophy::RotateTrophy(float deltaTime)
{
	glm::quat addRot(glm::identity<glm::quat>());
	addRot = glm::rotate(addRot, { 0.f, spinSpeed * deltaTime, 0.f });
	AddRotation(addRot);
}

void Trophy::OffsetTrophy(float deltaTime)
{
	const auto currentPos = GetPosition();

	if (currentYOffset > maxYOffset) sign = -1.f;
	if (currentYOffset < -maxYOffset) sign = 1.f;

	const glm::vec3 addSpeed{ 0.f, moveSpeed * deltaTime * sign, 0.f };
	currentYOffset += addSpeed.y;
	SetPosition(currentPos + addSpeed);
}

void Trophy::SetTrophyColor(const glm::vec3& newColor)
{
	auto& mesh = GetComponent<StaticMeshComponent>().staticMeshInstanced;
	mesh.SetColor(newColor);
}

void Trophy::OnHit(std::uint32_t otherEntity)
{
	/*
	Entity otherEnt((entt::entity)otherEntity, GetWorld());
	auto& oEntTag = otherEnt.GetComponent<TagComponent>().tag;

	if (oEntTag == "Player")
		GetWorld()->DeleteEntity(GetEntityId());
	*/
}