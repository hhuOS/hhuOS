/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "WaveFile.h"
#include "lib/util/base/Address.h"

namespace Util::Sound {

WaveFile::WaveFile(uint8_t *buffer) : buffer(buffer) {
    auto &riffChunk = *((RiffChunk*) buffer);

    audioFormat = riffChunk.formatChunk.audioFormat;
    bitsPerSample = riffChunk.formatChunk.bitsPerSample;
    numChannels = riffChunk.formatChunk.numChannels;
    samplesPerSecond = riffChunk.formatChunk.samplesPerSecond;
    bytesPerSecond = riffChunk.formatChunk.bytesPerSecond;
    bitsPerSample = riffChunk.formatChunk.bitsPerSample;
    frameSize = static_cast<uint16_t>(riffChunk.formatChunk.numChannels * ((bitsPerSample + 7) / 8));
    dataSize = riffChunk.dataChunk.chunkSize;
    sampleCount = dataSize / frameSize;
}

const uint8_t* WaveFile::getData() const {
    return &buffer[sizeof(RiffChunk)];
}

uint32_t WaveFile::getDataSize() const {
    return dataSize;
}

WaveFile::~WaveFile() {
    delete buffer;
}

WaveFile::AudioFormat WaveFile::getAudioFormat() const {
    return audioFormat;
}

uint16_t WaveFile::getNumChannels() const {
    return numChannels;
}

uint32_t WaveFile::getSamplesPerSecond() const {
    return samplesPerSecond;
}

uint32_t WaveFile::getBytesPerSecond() const {
    return bytesPerSecond;
}

uint16_t WaveFile::getBitsPerSample() const {
    return bitsPerSample;
}

uint16_t WaveFile::getFrameSize() const {
    return frameSize;
}

uint32_t WaveFile::getSampleCount() const {
    return sampleCount;
}

}