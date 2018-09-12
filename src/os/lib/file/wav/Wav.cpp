#include "Wav.h"

Wav::Wav(File *file) {
    *file >> rawData;

    processData();

    delete rawData;
}

Wav::~Wav() {
    delete pcmData;
}

void Wav::processData() {
    RiffChunk riffChunk = *((RiffChunk*) rawData);

    audioFormat = riffChunk.formatChunk.audioFormat;
    bitsPerSample = riffChunk.formatChunk.bitsPerSample;
    numChannels = riffChunk.formatChunk.numChannels;
    samplesPerSecond = riffChunk.formatChunk.samplesPerSecond;
    bytesPerSecond = riffChunk.formatChunk.bytesPerSecond;
    bitsPerSample = riffChunk.formatChunk.bitsPerSample;
    frameSize = static_cast<uint16_t>(riffChunk.formatChunk.numChannels * ((bitsPerSample + 7) / 8));
    sampleCount = riffChunk.dataChunk.chunkSize / frameSize;

    pcmData = new uint8_t[riffChunk.dataChunk.chunkSize];
    memcpy(pcmData, &rawData[sizeof(RiffChunk)], riffChunk.dataChunk.chunkSize);
}