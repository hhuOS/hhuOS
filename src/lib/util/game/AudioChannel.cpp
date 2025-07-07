/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "AudioChannel.h"

#include "sound/WaveFile.h"

namespace Util::Game {

void AudioChannel::play(const AudioBuffer &audioBuffer, bool loop) {
    buffer = &audioBuffer;
    position = 0;
    AudioChannel::loop = loop;

    if (getState() != PLAYING) {
        Sound::AudioChannel::play();
    }
}

bool AudioChannel::update() {
    const auto writableBytes = getWritableBytes();
    if (writableBytes == 0) {
        return false;
    }

    const auto bytesLeft = buffer->getSize() - position;
    const auto toWrite = static_cast<int32_t>(writableBytes < bytesLeft ? writableBytes : bytesLeft);

    write(buffer->getSamples() + position, 0, toWrite);
    position += toWrite;

    if (position >= buffer->getSize()) {
        // End of sound buffer reached
        if (loop) {
            // Loop the sound buffer
            position = 0;
        } else {
            // Stop playback if looping is not enabled
            stop();
        }
    }

    return true;
}

String AudioChannel::getWaveFilePath() const {
    return buffer->getWaveFilePath();
}

}
