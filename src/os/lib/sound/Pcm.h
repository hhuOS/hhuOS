#ifndef HHUOS_PCMDATA_H
#define HHUOS_PCMDATA_H

#include <cstdint>

class Pcm {

protected:

    uint16_t numChannels = 0;
    uint32_t samplesPerSecond = 0;
    uint32_t bytesPerSecond = 0;
    uint16_t bitsPerSample = 0;
    uint16_t frameSize = 0;
    uint32_t sampleCount = 0;

    char *pcmData = nullptr;

protected:

    Pcm() = default;

public:

    Pcm(const Pcm &copy) = delete;

    ~Pcm() = default;

    uint16_t getNumChannels() const;

    uint32_t getSamplesPerSecond() const;

    uint32_t getBytesPerSecond() const;

    uint16_t getBitsPerSample() const;

    uint16_t getFrameSize() const;

    uint32_t getSampleCount() const;

    char *getPcmData() const;
};

#endif
