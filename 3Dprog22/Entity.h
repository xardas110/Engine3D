#pragma once
#include "World.h"
#include "Include/entt/entt.hpp"
#include "Components.h"

class Entity
{
	friend class World;
	friend class WorldSerializer;
	
	entt::entity id{entt::null};
	World* world; //should be a weak ref

protected:
	Entity();
public:
	Entity(entt::entity id, World *worldPtr);

	template<typename T>
	T& GetComponent()
	{
		if (world)
		{
			return world->entRegistry.get<T>(id);
			//worldPtr->entRegistry.
		}
	}

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		if (world)
		{
			auto& type = world->entRegistry.emplace<T>(id, std::forward<Args>(args)...);
			/*
			if (std::is_same<T, OBBCollisionComponent>::value)
			{
				auto& obb = world->entRegistry.get<OBBCollisionComponent>(id);
				auto& transform = world->entRegistry.get<TransformComponent>(id);
				obb.collisionVolume.centerOfMass = transform.GetPosition();
				world->collisionSystem.AddVolume(&obb.collisionVolume);
			}
			if (std::is_same<T, SphereCollisionComponent>::value)
			{
				auto& obb = world->entRegistry.get<SphereCollisionComponent>(id);
				auto& transform = world->entRegistry.get<TransformComponent>(id);
				obb.collisionVolume.centerOfMass = transform.GetPosition();				
				world->collisionSystem.AddVolume(&obb.collisionVolume);
			}
			*/
			return type;
		}
	}

	template<typename T>
	bool HasComponent()
	{
		if (world)
		{
			return world->entRegistry.any_of<T>(id);
		}
		return false;
	}
	
	template<typename T>
	size_t RemoveComponent()
	{
		if (world)
		{
			return world->entRegistry.remove<T>(id);
		}
	}

	entt::entity GetEntityId() const;
	World* GetWorld() const;
	
	bool IsValid();
	
	void SetPosition(const glm::vec3& newPosition);
	glm::vec3 GetPosition();

	void SetScale(const glm::vec3& newScale);
	glm::vec3 GetScale();

	void SetRotation(const glm::quat& newRotation);
	glm::quat GetRotation();

	void AddRotation(const glm::quat& addRot);
	
	void SetTransform(const glm::mat4& transform);
	void SetIgnorePathFinding(bool bIgnore);

	glm::vec3 GetForwardVector();
	glm::vec3 GetRightVector();
	glm::vec3 GetUpVector();

	void SetCollisionExtent(const glm::vec3& newExtent);
	const glm::vec3& GetCollisionExtent();

	void SetCollisionPosition(const glm::vec3& newPos);
	const glm::vec3& GetCollisionPosition();

	void SetPhysicsPosition(const glm::vec3& newPos);
	const glm::vec3& GetPhysicsPosition();
	
	void SetPhysicsRotation(const glm::quat& newRotation);
	const glm::quat& GetPhysicsRotation();

	CollideableType::Type GetColType();
	std::uint32_t GetColIndex();

private:
	void Delete();
};

/// <summary>
/// Trying Cherno's approach.
/// </summary>
class ScriptableEntity : public Entity
{
	friend class World;
public:
	//ScriptableEntity(entt::entity id, World* world) :Entity(id, world) {}	
protected:
	virtual void OnCreate(){};
	virtual void OnDestroy(){};
	virtual void OnBeginPlay() {};
	virtual void OnUpdate(float deltaTime){};
	
	virtual void OnHit(entt::entity otherEnt) {};
};

/// <summary>
/// Trying Cherno's approach
/// https://www.youtube.com/watch?v=1cHEcrIn8IQ&ab_channel=TheCherno
/// </summary>
struct ScriptComponent
{
	friend class World;
	
	ScriptableEntity* instance{ nullptr };

	ScriptableEntity* (*InstantiateScript)();
	void (*DestroyScript)(ScriptComponent*);
	
	template<typename T>
	void Bind()
	{
		InstantiateScript = []() {return static_cast<ScriptableEntity*>(new T()); };
		DestroyScript = [](ScriptComponent* self) {delete (T*)self->instance; self->instance = nullptr; };
	}	
};
