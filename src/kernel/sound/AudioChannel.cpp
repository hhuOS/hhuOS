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
 *
 * The audio mixer is based on a bachelor's thesis, written by Andreas Lüpertz.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-ANLU89
 */

#include "AudioChannel.h"

#include "async/Thread.h"

namespace Kernel {

AudioChannel::AudioChannel(int32_t bufferSize) :
    FilterInputStream(inputStream), FilterOutputStream(outputStream),
    inputStream(bufferSize), outputStream(inputStream) {}

bool AudioChannel::write(uint8_t c) {
    while (state != Util::Sound::AudioChannel::PLAYING) {
        Util::Async::Thread::yield();
    }

    return FilterOutputStream::write(c);
}

uint32_t AudioChannel::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    while (state != Util::Sound::AudioChannel::PLAYING) {
        Util::Async::Thread::yield();
    }

    return FilterOutputStream::write(sourceBuffer, offset, length);
}

void AudioChannel::setState(Util::Sound::AudioChannel::State state) {
    AudioChannel::state = state;
}

Util::Sound::AudioChannel::State AudioChannel::getState() const {
    return state;
}

uint32_t AudioChannel::getReadableBytes() {
    return inputStream.getReadableBytes();
}

uint32_t AudioChannel::getWritableBytes() {
    return outputStream.getWritableBytes();
}

}
