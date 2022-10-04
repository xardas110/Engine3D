#include "WavFileReader.h"
#include <iostream>

#ifdef _WIN32
#include "Include/AL/al.h"
#include "Include/AL/alc.h"
#include "Include/AL/alext.h"
#include "Include/snd/sndfile.h"

#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

bool WavFileReader::LoadWav(const std::string& path, Wave_t* wavePtr)
{
    std::cout << "Loading " + path + " from disk\n";
    FILE* fp = NULL;
    fp = std::fopen(path.c_str(), "rb");
    if (fp == NULL) {
        return EndOnError("FileHandler error: File not found.\n");
    }

    char type[4];

    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
    {
        return EndOnError("FileHandler error: RIFF header missing or invalid.\n");
    }

    std::fread(&wavePtr->size, sizeof(uint32_t), 1, fp);
    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
    {
        return EndOnError("FileHandler error: WAVE header missing or invalid.\n");
    }

    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
    {
        return EndOnError("FileHandler error: fmt header missing or invalid.\n");
    }

    std::fread(&wavePtr->chunkSize, sizeof(uint32_t), 1, fp);
    std::fread(&wavePtr->formatType, sizeof(short), 1, fp);
    std::fread(&wavePtr->channels, sizeof(short), 1, fp);
    std::fread(&wavePtr->sampleRate, sizeof(uint32_t), 1, fp);
    std::fread(&wavePtr->avgBytesPerSec, sizeof(uint32_t), 1, fp);
    std::fread(&wavePtr->bytesPerSample, sizeof(short), 1, fp);
    std::fread(&wavePtr->bitsPerSample, sizeof(short), 1, fp);

    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a')
    {
        return EndOnError("FileHandler error: data header missing or invalid.\n");
    }

    std::fread(&wavePtr->dataSize, sizeof(uint32_t), 1, fp);

    wavePtr->buffer = new unsigned char[wavePtr->dataSize];
    //static_cast just to get rid of compiler warning
    uint32_t result = static_cast<uint32_t>(std::fread(wavePtr->buffer, sizeof(char), wavePtr->dataSize, fp));
    if (result != wavePtr->dataSize)
    {
        return EndOnError("Wav-file read error: fread result mismatch.\n");
    }

    if (ferror(fp))
    {
        return EndOnError("Wav-file read error: fstream error.");
    }

    if (wavePtr->buffer == NULL)
    {
        return EndOnError("Wav-file read error: Wave Data pointer is NULL.\n");
    }

    std::fclose(fp);
    std::cout << "Loading complete!\n";
    return true;
}

unsigned WavFileReader::LoadAudio(const char* filename)
{
	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	/* Open the audios file and check that it's usable. */
	sndfile = sf_open(filename, SFM_READ, &sfinfo);
	if (!sndfile)
	{
		fprintf(stderr, "Could not open audios in %s: %s\n", filename, sf_strerror(sndfile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels)
	{
		fprintf(stderr, "Bad sample count in %s (%" ")\n", filename, sfinfo.frames);
		sf_close(sndfile);
		return 0;
	}

	/* Get the sound format, and figure out the OpenAL format */
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4)
	{
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format)
	{
		fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
		sf_close(sndfile);
		return 0;
	}

	/* Decode the whole audios file to a buffer. */
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1)
	{
		free(membuf);
		sf_close(sndfile);
		fprintf(stderr, "Failed to read samples in %s (%" ")\n", filename, num_frames);
		return 0;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	/* Buffer the audios data into a new buffer object, then free the data and
	 * close the file.
	 */
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}

	return buffer;
}

bool WavFileReader::EndOnError(const std::string& errorMsg)
{
    std::cout << errorMsg; return false;
}
