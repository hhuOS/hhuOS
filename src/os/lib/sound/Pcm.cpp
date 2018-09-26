/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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
