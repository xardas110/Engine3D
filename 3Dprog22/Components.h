#pragma once
#include "Mesh.h"
#include "shader.h"
#include "Material.h"
#include "Texture.h"
#include <string>

#include "DirectionalLight.h"
#include "HelperMath.h"
#include "Include/glm/gtx/quaternion.hpp"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Terrain.h"
#include "AudioData.h"
#include "StaticMeshInstanced.h"
#include "Vegetation.h"
#include "Particle.h"
#include "Text.h"
#include "Segment.h"
#include "Collision.h"
#include "RigidBody.h"
#include "qevent.h"
#include "HelperMath.h"
#include "Include/PhysX/PxPhysics.h"

/// <summary>
/// All the components below are wrappers for the ECS
/// </summary>

struct TransformComponent
{

	[[nodiscard]]
	glm::mat4 GetTransform() const
	{
		glm::mat4 posMat(1.f);
		posMat = glm::translate(posMat, position);

		glm::mat4 scaleMat(1.f);
		scaleMat = glm::scale(scaleMat, scale);

		const glm::mat4 rotMat(rotation);

		return posMat * rotMat * scaleMat;
	}

	void SetTransform(const glm::mat4& transform) //should be private
	{
		glm::vec3 newPos;
		glm::vec3 newScale;
		glm::quat newRot;

		DecomposeMatrix(transform, newPos, newRot, newScale);

		position = newPos;
		scale = newScale;
		rotation = newRot;
	}

	[[nodiscard]]
	glm::vec3 GetPosition() const
	{
		return position;
	}

	[[nodiscard]]
	glm::vec3 GetScale() const
	{
		return scale;
	}

	[[nodiscard]]
	glm::quat GetRotation() const
	{
		return rotation;
	}

	//Only Entity has access rights.
	void SetPosition(const glm::vec3& newPos)
	{
		position = newPos;
	}
	void SetScale(const glm::vec3& newScale)
	{
		scale = newScale;
	}
	void SetRotation(const glm::quat& newRot)
	{
		rotation = newRot;
	}

	glm::vec3 position{ 0.f };
	glm::vec3 scale{ 1.f };
	glm::quat rotation{ glm::identity<glm::quat>() };
};

struct TagComponent
{
	std::string tag;
	TagComponent(const std::string& tag) :tag(tag) {}
};

struct InputComponent
{
	std::function<void(QMouseEvent*)> mousePressedEvent;
	std::function<void(QMouseEvent*)> mouseReleasedEvent;
	std::function<void(QMouseEvent*)> mouseMoveEvent;
	std::function<void(std::map<int, bool>*, float)> mouseHeldEvent;

	std::function<void(QKeyEvent*)> keyPressedEvent;
	std::function<void(QKeyEvent*)> keyReleasedEvent;
	std::function<void(std::map<int, bool>*, float)> keyHeldEvent;

	void BindMousePressed(std::function<void(QMouseEvent*)> lambda)
	{
		mousePressedEvent = lambda;
	}

	void BindMouseReleased(std::function<void(QMouseEvent*)> lambda)
	{
		mouseReleasedEvent = lambda;
	}

	void BindMouseMove(std::function<void(QMouseEvent*)> lambda)
	{
		mouseMoveEvent = lambda;
	}

	void BindMouseHeld(std::function<void(std::map<int, bool>*, float)> lambda)
	{
		mouseHeldEvent = lambda;
	}

	void BindKeyPressed(std::function<void(QKeyEvent*)> lambda)
	{
		keyPressedEvent = lambda;
	}

	void BindKeyReleased(std::function<void(QKeyEvent*)> lambda)
	{
		keyReleasedEvent = lambda;
	}

	void BindKeyHeld(std::function<void(std::map<int, bool>*, float)> lambda)
	{
		keyHeldEvent = lambda;
	}
};

struct SpringArmComponent
{
	Segment segment;
	
	float yaw{ -90.f };
	float pitch{ 30.f };	
	float length{ 10.f };
	
	glm::vec3 rootOffset{ 0.f };

	void SetLength(const float length) { this->length = length; }
	void SetRootOffset(const glm::vec3& rootOffset) { this->rootOffset = rootOffset; segment.b = this->rootOffset; }

	void SetPitch(const float pitch)
	{
		this->pitch = pitch;
		
		if (pitch > 89.0f)
			this->pitch = 89.0f;
		if (pitch < -89.0f)
			this->pitch = -89.0f;
		
		ApplyRotation();
	}
	void SetYaw(const float yaw)
	{
		this->yaw = yaw;
		ApplyRotation();
	}
	void AddYaw(const float yaw)
	{
		this->yaw += yaw;
		ApplyRotation();
	}
	void AddPitch(const float pitch)
	{
		this->pitch+= pitch;

		if (this->pitch > 89.0f)
			this->pitch = 89.0f;
		if (this->pitch < -89.0f)
			this->pitch = -89.0f;
		
		ApplyRotation();
	}
	
	void AddRotation(const float yaw, const float pitch)
	{
		this->yaw += yaw;
		this->pitch += pitch;
		ApplyRotation();
	}
	void SetRotation(const float yaw, const float pitch)
	{
		this->yaw = yaw;
		this->pitch = pitch;
		ApplyRotation();
	}

[[nodiscard]] const glm::vec3&	GetAttachPosition() const { return segment.a; }
[[nodiscard]] const glm::vec3&	GetRootPosition() const { return segment.b; }
//[[nodiscard]] const glm::quat&	GetRotation() const { return rotation; }
private:
	void ApplyRotation()
	{
		segment.a.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		segment.a.y = sin(glm::radians(pitch));
		segment.a.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		segment.a = glm::normalize(segment.a) * length;
	}
};

struct SkeletalMeshComponent
{
	SkeletalMesh skeletalMesh;

	operator SkeletalMesh& () { return skeletalMesh; }
};

struct StaticMeshComponent
{
	StaticMesh staticMeshInstanced;

	operator StaticMesh& () { return staticMeshInstanced; }
};

struct CameraComponent
{
	Camera camera;

	operator Camera& () { return camera; }
};

struct MeshComponent
{
	Mesh mesh;
	Shader shader;
};

struct TerrainComponent
{
	Terrain terrain;
};

struct AudioComponent
{
	std::array<AudioData, 5> audios;

	AudioComponent() = default;
};

struct StaticMeshInstancedComponent
{
	StaticMeshInstanced staticMeshInstanced;
};

struct CollisionComponent
{
	CollisionComponent(CollideableType::Type collisionType)
	{
		col.type = collisionType;
	}
	Collideable col;
};

struct PhysicsComponent
{
	RigidBody body;
};

struct PhysXBody
{
	physx::PxRigidDynamic* body; 
};

struct PhysicsBall
{
	int ball;
};

struct PhysicsBox
{
	int box;
};
struct VegetationComponent
{
	Vegetation veg;
};

struct ParticleEmitterComponent
{
	ParticleEmitter emitter;
};

struct TextComponent
{
	Text text;
};
