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

#include "WaveFile.h"

#include "base/Panic.h"

namespace Util::Sound {

WaveFile::WaveFile(const Io::File &file) : FilterInputStream(stream), stream(file) {
    // Read the RIFF and format chunks from the beginning of the file.
    FilterInputStream::read(reinterpret_cast<uint8_t*>(&riffChunk), 0, sizeof(RiffChunk));
    FilterInputStream::read(reinterpret_cast<uint8_t*>(&formatChunk), 0, sizeof(FormatChunk));

    // Search for the 'data' chunk in the file, by reading chunks until we find one with the 'data' signature.
    // Afterward, this stream will be positioned at the start of the 'data' chunk and is ready for reading audio data.
    auto read = FilterInputStream::read(reinterpret_cast<uint8_t*>(&dataChunk), 0, sizeof(dataChunk));
    while (dataChunk.dataSignature[0] != 'd' &&dataChunk.dataSignature[1] != 'a' &&
        dataChunk.dataSignature[2] != 't' && dataChunk.dataSignature[3] != 'a')
    {
        if (read <= 0) {
            Util::Panic::fire(Panic::INVALID_ARGUMENT, "WaveFile: No 'data' chunk found!");
        }

        skip(dataChunk.chunkSize);
        read = FilterInputStream::read(reinterpret_cast<uint8_t*>(&dataChunk), 0, sizeof(dataChunk));
    }
}

WaveFile::AudioFormat WaveFile::getAudioFormat() const {
    return formatChunk.audioFormat;
}

uint16_t WaveFile::getNumChannels() const {
    return formatChunk.numChannels;
}

uint32_t WaveFile::getSamplesPerSecond() const {
    return formatChunk.samplesPerSecond;
}

uint32_t WaveFile::getBytesPerSecond() const {
    return formatChunk.bytesPerSecond;
}

uint16_t WaveFile::getBitsPerSample() const {
    return formatChunk.bitsPerSample;
}

uint32_t WaveFile::getSampleCount() const {
    return dataChunk.chunkSize / formatChunk.frameSize;
}

uint32_t WaveFile::getDataSize() const {
    return dataChunk.chunkSize;
}

}