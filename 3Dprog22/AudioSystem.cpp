#include "AudioSystem.h"
#include "World.h"
#include "Components.h"
#include "AudioManager.h"

void AudioSystem::Init(World* world, entt::registry& registry)
{
	UpdateListener(world);
	UpdateTransforms(registry);
}

void AudioSystem::Update(World* world, entt::registry& registry)
{
	UpdateListener(world);
	UpdateTransforms(registry);
}

void AudioSystem::UpdateListener(World* world)
{
	AudioManager& audioManager = AudioManager::Get();
	auto* camera = world->GetRenderCamera();
	auto camPos = camera->GetPosition();
	const glm::vec3 camVel = { 0.f, 0.f, 0.f };
	auto camDir = camera->GetDirection();
	
	audioManager.UpdateListener(camPos, camVel, camDir, World::GetWorldUp());
}

void AudioSystem::UpdateTransforms(entt::registry& registry)
{
	auto view = registry.view<TransformComponent, AudioComponent>();

	for (auto entity : view)
	{
		auto [transformComponent, audioComponent] = view.get<TransformComponent, AudioComponent>(entity);

		for (auto& audio : audioComponent.audios)
		{
			if (audio.IsValid())
				audio.SetPosition(transformComponent.GetPosition());
		}		
	}
}

void AudioSystem::DeleteAll(entt::registry& registry)
{
	auto view = registry.view<AudioComponent>();
	for (auto entity : view)
	{
		auto &audioComponent = view.get<AudioComponent>(entity);

		for (auto& audio : audioComponent.audios)
		{
			audio.Stop();
			audio.CleanUp();			
		}
	}
}
