#pragma once
#include "Include/entt/entt.hpp"
#include "shader.h"
#include "Text.h"
#include "SSBO.h"
#include "Font.h"
#include "Mesh.h"
#include "ABuffer.h"

#define MAX_CHARACTERS 50000

class BillboardSystem
{
	friend class World;
	friend class DeferredRenderer;

	void Init(class World* world, entt::registry& registry);
	void InitShader();
	void InitShaderBindings();
	void InitSSBO();
	void InitFonts();

	void Update(class World* world, entt::registry& registry, float deltatime);

	void Render(class World* world, entt::registry& registry, const ABuffer* aBuffer, const Mesh& quad);

	Shader textShader;
	SSBO ssbo;
	TextBindings tb;
	Mesh quad;

	Font fonts[FontType::Size];
};

