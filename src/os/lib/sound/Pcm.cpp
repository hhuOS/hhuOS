#include "Pcm.h"

Pcm::AudioFormat Pcm::getAudioFormat() const {
    return audioFormat;
}

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

uint8_t *Pcm::getPcmData() const {
    return pcmData;
}

Pcm::Pcm(uint16_t numChannels, uint32_t samplesPerSecond, uint32_t bytesPerSecond, uint16_t bitsPerSample,
        uint16_t frameSize, uint32_t sampleCount, uint8_t *pcmData) :
            numChannels(numChannels), samplesPerSecond(samplesPerSecond), bytesPerSecond(bytesPerSecond),
            bitsPerSample(bitsPerSample), frameSize(frameSize), sampleCount(sampleCount), pcmData(pcmData) {

}
