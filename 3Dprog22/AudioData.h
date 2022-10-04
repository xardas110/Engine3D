#pragma once

#include "Include/glm/glm.hpp"
#include <string>

using AudioSource = unsigned;
using AudioBuffer = unsigned;

namespace AudioState
{ 
	enum AudioState
	{
		Stop,
		Play,
		Pause
	};
}

//Transformcomponent contains pos
//rigidbody contains velocity
// has unique ownership of source
struct AudioData
{	
	friend class AudioManager;

	AudioData();
	AudioData(const std::string& path);

	~AudioData();

	AudioData(const AudioData& cpy) = delete;
	AudioData(AudioData&& move) = default;

	AudioData& operator=(const AudioData& cpy) = delete;
	AudioData& operator=(AudioData&& move) = default;

	void SetPosition(const glm::vec3& newPos);
	void SetLocalPosition(const glm::vec3 localPos);

	void SetVelocity(const glm::vec3& newVel);
	void SetPitch(const float newPitch);
	void SetGain(const float newGain);
	void SetState(AudioState::AudioState newState);
	void SetLooping(bool bLoop);

	void Play();
	void Pause();
	void Stop();

	bool IsValid() const;

	/*Ent deletion will clean*/
	void CleanUp();

	void LoadAudio(const std::string& path);

	AudioState::AudioState GetState() const;
	const glm::vec3& GetLocalPosition() const;
	glm::vec3 GetPosition() const; 
private:
	AudioSource source;
	glm::vec3 localPosition{0.f, 0.f, 0.f};
	AudioState::AudioState state{ AudioState::Stop };
};

