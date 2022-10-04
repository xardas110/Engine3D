#pragma once
#include "Include/glm/glm.hpp"
#include "Mesh.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Transform.h"

/// <summary>
/// Datastruct for a sun quad, does not contain the directional light
/// </summary>
struct Sun
{
	Mesh mesh;
	Transform transform;//Only scale and Rotation
	Texture texture;
	Texture innerTexture;
};

