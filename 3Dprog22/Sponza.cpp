#include "Sponza.h"
#include "World.h"
#include "Entity.h"

void Sponza::Create(World* world, entt::registry& registry)
{
	CreateScene(world);
    world->LoadRenderConfig("../3dprog22/Config/Renderer/ConfigSponza.json");
}

void Sponza::BeginPlay(World* world, entt::registry& registry)
{
}

void Sponza::EndPlay(World* world, entt::registry& registry)
{
}

void Sponza::Update(World* world, entt::registry& registry, float deltatime)
{
}

void Sponza::UpdateEditor(World* world, entt::registry& registry, float deltatime)
{
}

void Sponza::Reset(World* world, entt::registry& registry)
{
}

void Sponza::CreateScene(World* world)
{
	auto* smm = world->GetStaticMeshManager();

	Entity sponza = world->CreateEntity("Crytek Sponza");
	auto& transform = sponza.GetComponent<TransformComponent>();
	auto& sm = sponza.AddComponent<StaticMeshComponent>();
    smm->LoadStaticMesh("../3dprog22/Assets/crytek-sponza-noflag/sponza.obj", sm);
	transform.SetScale({0.10f, 0.10f, 0.10f});
	transform.SetPosition({ 0.f, 0.f, 0.f });
	
	
	for (auto& mesh : sm.staticMeshInstanced.GetMeshes())
	{
		if (mesh.material.HasAOmap())
		{
			mesh.material.specularmap = mesh.material.ambientmap;
			mesh.material.ambientmap = Texture();
		}
	}
	
}
