#include "Pcm.h"

uint16_t Pcm::getNumChannels() const {
    return numChannels;
}

uint32_t Pcm::getSamplesPerSecond() const {
    return samplesPerSecond;
}

uint32_t Pcm::getBytesPerSecond() const {
    return bytesPerSecond;
}

uint16_t Pcm::getBitsPerSample() const {
    return bitsPerSample;
}

uint16_t Pcm::getFrameSize() const {
    return frameSize;
}

uint32_t Pcm::getSampleCount() const {
    return sampleCount;
}

char *Pcm::getPcmData() const {
    return pcmData;
}
