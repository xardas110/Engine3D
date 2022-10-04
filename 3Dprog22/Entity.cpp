#include "Entity.h"
#include "Components.h"

Entity::Entity()
{
}

Entity::Entity(entt::entity id, World* worldPtr)
	:id(id), world(worldPtr) {}

entt::entity Entity::GetEntityId() const
{
	return id;
}

World* Entity::GetWorld() const
{
	return world;
}

bool Entity::IsValid()
{
	if (!world) return false;

	if (!world->entRegistry.valid(id)) return false;

	return true;
}

void Entity::SetPosition(const glm::vec3& newPosition)
{
    if (!IsValid()) return;
	
	if (HasComponent<TransformComponent>())
	{
		auto& transform = GetComponent<TransformComponent>();
		transform.SetPosition(newPosition);
		
	}
	/*
	if (HasComponent<AABBCollisionComponent>())
	{
		auto& comp = GetComponent<AABBCollisionComponent>();
		comp.collisionVolume.SetCenter(newPosition);	
	}
	
	if (HasComponent<OBBCollisionComponent>())
	{
		auto& comp = GetComponent<OBBCollisionComponent>();
		comp.collisionVolume.SetPosition(newPosition);
	}
	
	if (HasComponent<TriangleCollisionComponent>())
	{
		//TODO: Make triangle abc maybe local cordinates and include a matrix to transform them to WS
		auto& comp = GetComponent<TriangleCollisionComponent>();

		const auto nTimesRemoved = world->collisionSystem.RemoveVolume(&comp.collisionVolume);
		
		glm::vec3 center{ 0.f };
		for (auto& pt : comp.collisionVolume.pts)
			center += pt;

		center /= 3.f;

		const auto aLocal = comp.collisionVolume.a - center;
		const auto bLocal = comp.collisionVolume.b - center;
		const auto cLocal = comp.collisionVolume.c - center;
		
		comp.collisionVolume.a = newPosition + aLocal;
		comp.collisionVolume.b = newPosition + bLocal;
		comp.collisionVolume.c = newPosition + cLocal;

		if (nTimesRemoved > 0)
			world->collisionSystem.AddVolume(&comp.collisionVolume);
	}
	if (HasComponent<SphereCollisionComponent>())
	{
		auto& comp = GetComponent<SphereCollisionComponent>();

		const auto nTimesRemoved = world->collisionSystem.RemoveVolume(&comp.collisionVolume);
		
		comp.collisionVolume.centerOfMass = glm::vec4(newPosition, 1.f);
		
		if (nTimesRemoved > 0)
			world->collisionSystem.AddVolume(&comp.collisionVolume);
	}
	if (HasComponent<PlaneCollisionComponent>())
	{
		auto& comp = GetComponent<PlaneCollisionComponent>();

		const auto nTimesRemoved = world->collisionSystem.RemoveVolume(&comp.collisionVolume);

		const float posLength = glm::length(newPosition);
		const glm::vec3 dir = newPosition / posLength;
		
		comp.collisionVolume.normal = dir;
		comp.collisionVolume.distance = posLength;

		if (nTimesRemoved > 0)
			world->collisionSystem.AddVolume(&comp.collisionVolume);
	}
	*/
	/*
	if (HasComponent<OBBCompoundCollisionComponent>())
	{
		auto& comp = GetComponent<OBBCompoundCollisionComponent>();

		for (auto& obb : comp.occ.constraints)
		{
			const auto nTimesRemoved = world->collisionSystem.RemoveVolume(&obb);
			obb.C += newPosition; // the compound is in local space
			if (nTimesRemoved > 0)
			{
				world->collisionSystem.AddVolume(&obb);
			}
		}
	}
	*/
}

glm::vec3 Entity::GetPosition()
{
	if (!world) glm::vec3(0.f);
	
	auto& transform = GetComponent<TransformComponent>();

	return transform.GetPosition();
}

void Entity::SetScale(const glm::vec3& newScale)
{
    if (!IsValid()) return;
	
	if (HasComponent<TransformComponent>())
	{
		auto& transform = GetComponent<TransformComponent>();
		transform.SetScale(newScale);
	}
}

glm::vec3 Entity::GetScale()
{
	assert(world);
	
	auto& trans = GetComponent<TransformComponent>();
	return trans.GetScale();
}

void Entity::SetRotation(const glm::quat& newRotation)
{
    if (!IsValid()) return;

	if (HasComponent<TransformComponent>())
	{
		auto& transform = GetComponent<TransformComponent>();
		transform.SetRotation(newRotation);
	}


	/*
	if (HasComponent<TriangleCollisionComponent>())
	{
		auto& triangle = GetComponent<TriangleCollisionComponent>().collisionVolume;

		triangle.a = newRotation * triangle.a;
		triangle.b = newRotation * triangle.b;
		triangle.c = newRotation * triangle.c;
	}
	if (HasComponent<SphereCollisionComponent>())
	{
		//no rotation
	}
	if (HasComponent<PlaneCollisionComponent>())
	{
	
	}
	if (HasComponent<OBBCollisionComponent>())
	{		
		auto& comp = GetComponent<OBBCollisionComponent>();
		comp.collisionVolume.SetRotation(newRotation);		
	}
	/*
	if (HasComponent<OBBCompoundCollisionComponent>())
	{
		auto& comp = GetComponent<OBBCompoundCollisionComponent>();

		for (auto& obb : comp.occ.constraints)
		{
			const auto nTimesRemoved = world->collisionSystem.RemoveVolume(&obb);
			obb.orient *= newRotation; // now can't remember the order of this multiplication, lets hope :)
			if (nTimesRemoved > 0)
			{
				world->collisionSystem.AddVolume(&obb);
			}
		}
	}
	*/
	
}

glm::quat Entity::GetRotation()
{
	assert(world);

	auto& trans = GetComponent<TransformComponent>();
	
	return trans.GetRotation();
}

void Entity::AddRotation(const glm::quat& addRot)
{
	SetRotation(addRot * GetRotation());
}

void Entity::SetTransform(const glm::mat4& transform)
{
	glm::vec3 newPos;
	glm::vec3 newScale;
	glm::quat newRot;

	DecomposeMatrix(transform, newPos, newRot, newScale);

	SetScale(newScale);
	SetRotation(newRot);
	SetPosition(newPos);	
}

void Entity::SetIgnorePathFinding(bool bIgnore)
{

}

glm::vec3 Entity::GetRightVector()
{
	if (!HasComponent<TransformComponent>()) return { 0.f, 0.f, 1.f };

	const auto transform = GetComponent<TransformComponent>().GetTransform();

	glm::mat4 transformInverse = glm::inverse(transform);
	return  glm::normalize(transformInverse[0]);
}

glm::vec3 Entity::GetUpVector()
{
	if (!HasComponent<TransformComponent>()) return { 0.f, 0.f, 1.f };

	const auto transform = GetComponent<TransformComponent>().GetTransform();

	glm::mat4 transformInverse = glm::inverse(transform);
	return  glm::normalize(transformInverse[1]);
}

void Entity::SetCollisionExtent(const glm::vec3& newExtent)
{
	if (HasComponent<CollisionComponent>())
	{
		auto& collision = GetComponent<CollisionComponent>().col;
		collision.SetExtents(newExtent);
	}
}

void Entity::SetCollisionPosition(const glm::vec3& newPos)
{
	auto& collision = GetComponent<CollisionComponent>().col;
	collision.SetPosition(newPos);
}

const glm::vec3& Entity::GetCollisionPosition()
{
	if (HasComponent<CollisionComponent>())
	{
		auto& collision = GetComponent<CollisionComponent>().col;
		return collision.GetPosition();
	}
	std::cout << "GetCollisionPosition failed " << (unsigned)id << std::endl;
	return glm::vec3(0.f);
}

const glm::vec3& Entity::GetCollisionExtent()
{
	if (HasComponent<CollisionComponent>())
	{
		auto& collision = GetComponent<CollisionComponent>().col;
		return collision.GetExtents();
	}
	std::cout << "Physics GetPhysicsExtent failed " << (unsigned)id << std::endl;
	return glm::vec3(0.f);
}

void Entity::SetPhysicsPosition(const glm::vec3& newPos)
{
	if (HasComponent<PhysicsComponent>())
	{
		auto& body = GetComponent<PhysicsComponent>().body;
		body.SetPos(newPos);
	}
}

const glm::vec3& Entity::GetPhysicsPosition()
{
	if (HasComponent<PhysicsComponent>())
	{
		auto& body = GetComponent<PhysicsComponent>().body;
		return body.GetPos();
	}
	
	std::cout << "Entity GetPhysicsPosition failed " << (unsigned)id << std::endl;
	return glm::vec3(0.f);
}

void Entity::SetPhysicsRotation(const glm::quat& newRotation)
{
	if (HasComponent<PhysicsComponent>())
	{
		auto& body = GetComponent<PhysicsComponent>().body;
		body.SetRotation(newRotation);
	}
	if (HasComponent<CollisionComponent>())
	{
		auto& collision = GetComponent<CollisionComponent>().col;
		collision.SetRotation(newRotation);
	}
}

const glm::quat& Entity::GetPhysicsRotation()
{
	if (HasComponent<PhysicsComponent>())
	{
		auto& body = GetComponent<PhysicsComponent>().body;
		return body.GetRotation();
	}
	if (HasComponent<CollisionComponent>())
	{
		auto& collision = GetComponent<CollisionComponent>().col;
		return collision.GetRotation();
	}

	std::cout << "Entity GetPhysicsRotation  failed " << (unsigned)id << std::endl;
	return glm::identity<glm::quat>();
}

CollideableType::Type Entity::GetColType()
{
	return GetComponent<CollisionComponent>().col.type;
}

std::uint32_t Entity::GetColIndex()
{
	return GetComponent<CollisionComponent>().col.shapeIndex;
}

void Entity::Delete()
{
	if (!world) return;
	
	if (HasComponent<AudioComponent>())
	{
		auto& audios = GetComponent<AudioComponent>().audios;

		for (auto& audio : audios)
		{
			audio.CleanUp();
		}
	}
}

glm::vec3 Entity::GetForwardVector()
{
	if (!HasComponent<TransformComponent>()) return { 0.f, 0.f, 1.f };

	const auto transform = GetComponent<TransformComponent>().GetTransform();

	glm::mat4 transformInverse = glm::inverse(transform);
	return  glm::normalize(transformInverse[2]);
}

