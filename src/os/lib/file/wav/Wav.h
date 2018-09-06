#ifndef HHUOS_WAV_H
#define HHUOS_WAV_H

#include <lib/file/File.h>
#include <lib/sound/Pcm.h>

class Wav : public Pcm {

private:

    struct FormatChunk {
        char formatSignature[4];
        uint32_t chunkSize;
        Pcm::AudioFormat audioFormat;
        uint16_t numChannels;
        uint32_t samplesPerSecond;
        uint32_t bytesPerSecond;
        uint16_t frameSize;
        uint16_t bitsPerSample;
    } __attribute__ ((packed));

    struct DataChunk {
        char dataSignature[4];
        uint32_t chunkSize;
    } __attribute__ ((packed));

    struct RiffChunk {
        char riffSignature[4];
        uint32_t chunkSize;
        char waveSignature[4];
        FormatChunk formatChunk;
        DataChunk dataChunk;
    } __attribute__ ((packed));

private:

    char *rawData = nullptr;

private:

    void processData();

public:

    explicit Wav(File *file);

    Wav(const Wav &copy) = delete;

    ~Wav();

};

#endif
