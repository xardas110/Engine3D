#pragma once
#include "Include/glm/fwd.hpp"
#include "Include/glm/gtx/transform.hpp"
#include "Include/glm/gtc/quaternion.hpp"
#include "HelperMath.h"

/// <summary>
/// Internal transform struct
/// Should differ from transform component due to access rights.
/// </summary>
struct Transform
{
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
	
	const glm::vec3& GetPosition() const
	{
		return position;
	}

	void AddPosition(const glm::vec3& addPos)
	{
		position += addPos;
	}

	const glm::vec3& GetScale() const
	{
		return scale;
	}

	const glm::quat& GetRotation() const
	{
		return rotation;
	}

private:		
	glm::vec3 position{ 0.f };
	glm::vec3 scale{ 1.f };
	glm::quat rotation{ glm::identity<glm::quat>() };
};
