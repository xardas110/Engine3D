#include "AudioData.h"
#include "Include/glm/gtc/type_ptr.inl"
#ifdef _WIN32
#include "Include/AL/al.h"
#include "Include/AL/alc.h"
#endif

#include "AudioManager.h"

AudioData::AudioData()
{
	//source generation happens inside the manager
	//This struct has ownership tho.
}

AudioData::AudioData(const std::string& path)
{
	LoadAudio(path);
}

AudioData::~AudioData()
{
	//ent deletes source
}

void AudioData::SetPosition(const glm::vec3& posWS)
{
	const glm::vec3 newPos = posWS + localPosition;
	alSourcefv(source, AL_POSITION, glm::value_ptr(newPos));
}

void AudioData::SetLocalPosition(const glm::vec3 localPos)
{
	localPosition = localPos;
}

void AudioData::SetVelocity(const glm::vec3& newVel)
{
	alSourcefv(source, AL_VELOCITY, glm::value_ptr(newVel));
}

void AudioData::SetPitch(const float newPitch)
{
	alSourcef(source, AL_PITCH, newPitch);
}

void AudioData::SetGain(const float newGain)
{
	alSourcef(source, AL_GAIN, newGain);
}

void AudioData::SetState(AudioState::AudioState newState)
{
	state = newState;

	switch (state)
	{
	case AudioState::Stop:
		alSourceStop(source);
		break;
	case AudioState::Play:
		alSourcePlay(source);
		break;
	case AudioState::Pause:
		alSourcePause(source);
		break;
	default:
		break;
	}
}

void AudioData::SetLooping(bool bLoop)
{
	alSourcei(source, AL_LOOPING, bLoop);
}

void AudioData::Play()
{
	SetState(AudioState::Play);
	alSourcePlay(source);
}

void AudioData::Pause()
{
	SetState(AudioState::Pause);
	alSourcePause(source);
}

void AudioData::Stop()
{
	SetState(AudioState::Stop);
	alSourceStop(source);
}

bool AudioData::IsValid() const
{
	return source != NULL;
}

void AudioData::CleanUp()
{
	printf("cleanup running \n");
	alSourcei(source, AL_BUFFER, 0);
	alDeleteSources(1, &source);
}

void AudioData::LoadAudio(const std::string& path)
{
	auto& audioManager = AudioManager::Get();
	audioManager.LoadAudio(path, *this);
}

AudioState::AudioState AudioData::GetState() const
{
	return state;
}

const glm::vec3& AudioData::GetLocalPosition() const
{
	return localPosition;
}

glm::vec3 AudioData::GetPosition() const
{
	glm::vec3 result;
	alGetSourcefv(source, AL_POSITION, glm::value_ptr(result));
	return result;
}
