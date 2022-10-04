#include "AudioManager.h"
#include "WavFileReader.h"
#include "AudioData.h"
#include "Include/glm/gtc/type_ptr.inl"
#include <string>
#include <sstream>

AudioManager& AudioManager::Get()
{
	static AudioManager audioManager;
	return audioManager;
}

AudioManager::AudioManager()
{
	Initialize();
}

AudioManager::~AudioManager()
{
	CleanUp();
}

void AudioManager::Initialize()
{
	printf("Initalizing openAL\n");
	device = alcOpenDevice(NULL);

	if (!device)
	{
		CheckError("OpenAL device not found");
		return;
	}
	
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
}

void AudioManager::CleanUp()
{
	for (auto& [string, buffer] : audioMap)
	{
		alDeleteBuffers(1, &buffer);
	}

	audioMap.clear();

	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

bool AudioManager::InsertBuffer(const std::string& path, AudioBuffer buffer)
{
	if (BufferExists(path))
	{
		printf("Audio exists at: %s", path);
		return false;
	}

	audioMap[path] = buffer;
	return true;
}

bool AudioManager::BufferExists(const std::string& path)
{
	return audioMap.find(path) != audioMap.end();
}

void AudioManager::PopulateSource(AudioSource& source, AudioBuffer buffer)
{
	GenerateSource(source);

	alSourcei(source, AL_BUFFER, buffer);
	CheckError("PopulateSource");
}

bool AudioManager::CheckError(const std::string& name)
{
	switch (alGetError())
	{
	case AL_NO_ERROR:break;
	case AL_INVALID_NAME:
	{
		printf("Open AL invalid name error: %s\n", name.c_str());
		return false;
	}
	case AL_INVALID_ENUM:
	{
		printf("Open AL invalid enum error: %s\n", name.c_str());
		return false;
	}
	case AL_INVALID_VALUE:
	{
		printf("Open AL invalid value error: %s\n", name.c_str());
		return false;
	}
	case AL_INVALID_OPERATION:
	{
		printf("Open AL invalid operation error: %s\n", name.c_str());
		return false;
	}
	case AL_OUT_OF_MEMORY:
	{
		printf("Open AL out of memory error: %s\n", name.c_str());
		return false;
	}
	default:break;	
	}
	return true;
}

bool AudioManager::GenerateBuffer(AudioBuffer& outBuffer)
{
	alGetError();
	alGenBuffers(1, &outBuffer);

	if (!CheckError("alGenBuffer")) return false;
	return true;
}

bool AudioManager::GenerateSource(AudioSource& outSource)
{
	alGetError();
	alGenSources(1, &outSource);

	if (!CheckError("alGenSource")) return false;
	return true;
}

bool AudioManager::LoadWav(const std::string& path, AudioData& outData)
{
	if (BufferExists(path))
	{
		printf("Audio instance found, returning an instanace");

		PopulateSource(outData.source, audioMap[path]);

		return true;
	}

	printf("Loading wav file\n");

	ALuint frequency{};
	ALenum format{};

	unsigned buffer;

	Wave_t* wavData = new Wave_t;

	if (!WavFileReader::LoadWav(path, wavData))
	{
		printf("Error loading wav file\n"); return false;
	}

	frequency = wavData->sampleRate;

	if (wavData->bitsPerSample == 8)
	{
		if (wavData->channels == 1)
		{
			format = AL_FORMAT_MONO8;
		}
		if (wavData->channels == 2)
		{
			format = AL_FORMAT_STEREO8;
		}
	}
	else if (wavData->bitsPerSample == 16)
	{
		if (wavData->channels == 1)
		{
			format = AL_FORMAT_MONO16;
		}
		if (wavData->channels == 2)
		{
			format = AL_FORMAT_STEREO16;
		}
	}

	if (wavData->buffer == NULL)
	{
		printf("No buffer data\n");
	}

	std::ostringstream stringData;
	stringData << wavData->dataSize;

	printf("DataSize in bytes: %s\n", stringData.str().c_str());

	alGetError();
	GenerateBuffer(buffer);
	alBufferData(buffer, format, wavData->buffer, wavData->dataSize, frequency);
	CheckError("alBufferData");

	PopulateSource(outData.source, buffer);

	printf("Loading Complete\n");

	if (wavData->buffer) delete wavData->buffer;
	if (wavData) delete wavData;

	InsertBuffer(path, buffer);
	return true;
}

bool AudioManager::LoadAudio(const std::string& path, AudioData& outData)
{
	if (BufferExists(path))
	{
		printf("Audio instance found, returning an instanace");

		PopulateSource(outData.source, audioMap[path]);

		return true;
	}

	unsigned buffer = WavFileReader::LoadAudio(path.c_str());

	if (buffer == NULL)
	{
		printf("Failed to load audios file: %s\n", path.c_str());
		return false;
	}

	PopulateSource(outData.source, buffer);
	InsertBuffer(path, buffer);

	return true;
}

void AudioManager::UpdateListener(const glm::vec3& pos, const glm::vec3& vel, const glm::vec3& dir, const glm::vec3& up)
{
	float orient[] = { dir.x, dir.y, dir.z, up.x, up.y, up.z };

	alListenerfv(AL_POSITION, glm::value_ptr(pos));
	alListenerfv(AL_VELOCITY, glm::value_ptr(vel));
	alListenerfv(AL_ORIENTATION, orient);
}
