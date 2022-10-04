#pragma once
#include "Texture.h"
#include "Include/glm/glm.hpp"
#include "GlobalTypeDefs.h"
#include "CascadedShadowMaps.h"
#include "FrameBuffer.h"
#include "UBO.h"

#include "GlobalTypeDefs.h" //max cascades are defined inside globaltypedefs, in pre compilation it will sync the variable with the shader

#define		AMPLIFIER 5.f //This will move the cascade bounds for the z depth, a box trace will eventually be run instead.

/// <summary>
/// The textures are inside the FBO and will belong to the CascadedShadowMaps class so they can be instanced as a texture array
/// Layered rendering will be used to render to multiple shadow maps in one shadow pass
/// </summary>
struct Cascade
{
	//Cascades use UBO. So they need to match GPU strides.
	glm::mat4 cascadeProjection{}; //64 bytes
	// How much the main projection has to move to center around this cascade
	float cascadeOffsetX{}; //4B Translation x from shadow space to cascade space
	float cascadeOffsetY{}; //4B Translation y from shadow space to cascade space
	//	---------------------------
	float cascadeScaleX{}; //4B The x scale needed to project from shadow space to this cascade
	
	float cascadeScaleY{}; //4B The y scale needed to project from shadow space to this cascade
};

struct CascadedShadowMaps
{
	friend class SkySystem;
	
	using Cascades = std::array<Cascade, MAX_CASCADES>;

	FrameBuffer FBO; // contains the texture array for the cascades
	UBO			UBO; // cascade data for GPU buffer

	glm::mat4 toShadowProjection{}; // transforms to shadow space, each cascade needs to use this before they apply their projection.
	
	const std::uint32_t cascadeResolution{ 8196u }; //remember to add this to the operator if refactoring is needed
	
	const Cascades& GetCascades(const FrustumCorners::FrustumArray& frustumCorners, const glm::vec3& dirLight, float camNear, float camFar);

	CascadedShadowMaps& operator=(const CascadedShadowMaps& other)
	{
		this->cascades = other.cascades;
		this->FBO = other.FBO;
		this->toShadowProjection = other.toShadowProjection;

		return *this;
	};
private:
	float zOffset{ 0.f };//Offset to move the frustum bounding boxes behind the camera
	Cascades cascades{};
	/*frustum corners In world space. The function populates member variables*/
	void CalculateCascades(const FrustumCorners::FrustumArray& frustumCorners, const glm::vec3& dirLight, float camNear, float camFar);

	void CalculateCascade(const FrustumCorners::FrustumArray& corners, const glm::vec3& dirLight, int index, float camNear, float camFar);
};

