#pragma once
#include <string>
#include <map>
#include "AudioData.h"

#ifdef _WIN32
#include "Include/AL/al.h"
#include "Include/AL/alc.h"
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

/// <summary>
/// Based on https://github.com/Hedmark-University-College-SPIM/3Dprog22/tree/Sound-OpenAL by Ole Flaten
/// </summary>

class AudioManager
{
	using AudioMap = std::map<std::string, AudioBuffer>;

	AudioManager();
	~AudioManager();

	void Initialize();
	void CleanUp();

	bool InsertBuffer(const std::string& path, AudioBuffer buffer);

	bool BufferExists(const std::string& path);

	void PopulateSource(AudioSource& source, AudioBuffer buffer);

	bool CheckError(const std::string& name);

	bool GenerateBuffer(AudioBuffer& outBuffer);
	bool GenerateSource(AudioSource& outSource);
public:
	static AudioManager& Get();

	/*Populates only the audios source of outData*/
	bool LoadWav(const std::string& path, AudioData& outData);

	/*Populates only the audios source of outData*/
	bool LoadAudio(const std::string& path, AudioData& outData);

	void UpdateListener(const glm::vec3& pos, const glm::vec3& vel, const glm::vec3& dir, const glm::vec3& up);
private:
	AudioMap audioMap;
	ALCdevice* device{ nullptr };
	ALCcontext* context{ nullptr };
};

