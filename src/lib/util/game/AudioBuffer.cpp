/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "AudioBuffer.h"

#include "io/file/File.h"
#include "sound/WaveFile.h"

namespace Util::Game {

AudioBuffer::AudioBuffer(const String &waveFilePath) : waveFilePath(waveFilePath) {
    auto waveFile = Sound::WaveFile(Io::File(waveFilePath));

    samples = new uint8_t[waveFile.getDataSize()];
    waveFile.read(samples, 0, waveFile.getDataSize());

    samplesPerSecond = waveFile.getSamplesPerSecond();
    bitsPerSample = waveFile.getBitsPerSample();
    numChannels = waveFile.getNumChannels();
    size = waveFile.getDataSize();
}

String AudioBuffer::getWaveFilePath() const {
    return waveFilePath;
}

uint32_t AudioBuffer::getSamplesPerSecond() const {
    return samplesPerSecond;
}

uint16_t AudioBuffer::getBitsPerSample() const {
    return bitsPerSample;
}

uint16_t AudioBuffer::getNumChannels() const {
    return numChannels;
}

uint32_t AudioBuffer::getSize() const {
    return size;
}

const uint8_t* AudioBuffer::getSamples() const {
    return samples;
}

}
