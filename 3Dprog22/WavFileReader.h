#pragma once

#include <cstdint>
#include <string>

/// <summary>
/// Code made by Ole Flaten
/// https://github.com/Hedmark-University-College-SPIM/3Dprog22/tree/Sound-OpenAL
/// </summary>

struct Wave_t
{
    uint32_t size;              ///< Size of the WAVE file.
    uint32_t chunkSize;         ///< Size of the fmt chunk.
    uint32_t sampleRate;        ///< Sample rate of the audios.
    uint32_t avgBytesPerSec;    ///< Average byte rate.
    uint32_t dataSize;          ///< Size of the data chunk.
    short formatType;           ///< Audio format.
    short channels;             ///< Number of audios channels.
    short bytesPerSample;       ///< Number of bytes per sample.
    short bitsPerSample;        ///< Number of bits per sample.

    unsigned char* buffer;      ///< Buffer to hold audios data.
    Wave_t() : buffer(NULL) {}  ///< Structure Constructor. Initialises data buffer to NULL.

};

class WavFileReader
{
    friend class AudioManager;
public:
    static bool LoadWav(const std::string& path, Wave_t* wavePtr);

    static bool EndOnError(const std::string& errorMsg);
private:
    static unsigned LoadAudio(const char* filename);
};

