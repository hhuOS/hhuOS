/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include <util/async/Thread.h>
#include <util/io/stream/FileInputStream.h>

namespace Util {
namespace Sound {

AudioChannel::~AudioChannel() {
    audioMixerFile.control(DELETE, id);
}

uint8_t AudioChannel::createChannel() const {
    const auto id = audioMixerFile.control(CREATE_CHANNEL);
    if (id < 0) {
        Panic::fire(Panic::ILLEGAL_STATE, "No audio channel available!");
    }

    return id;
}

bool AudioChannel::stop(const bool waitFlush) {
    const auto success = audioChannelFile.control(STOP);
    if (success) {
        playing = false;
    }

    if (waitFlush) {
        while (getState() != STOPPED) {
            Async::Thread::yield();
        }
    }

    return success;
}

bool AudioChannel::play() {
    const auto success = audioChannelFile.control(PLAY);
    if (success) {
        playing = true;
    }

    return success;
}


AudioChannel::State AudioChannel::getState() const {
    return static_cast<State>(audioChannelFile.control(GET_PLAYBACK_STATE));
}

size_t AudioChannel::getRemainingBytes() const {
    return audioChannelFile.control(GET_REMAINING_BYTES);
}

size_t AudioChannel::getWritableBytes() const {
    return audioChannelFile.control(GET_WRITABLE_BYTES);
}

}
}