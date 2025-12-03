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
#include "util/io/stream/FileInputStream.h"

namespace Util {
namespace Sound {

AudioChannel::~AudioChannel() {
    audioMixerFile.controlFile(DELETE, Util::Array<size_t>({id}));
}

uint8_t AudioChannel::createChannel() {
    uint8_t id = 0;
    if (!audioMixerFile.controlFile(CREATE, Util::Array<size_t>({reinterpret_cast<size_t>(&id)}))) {
        Panic::fire(Panic::ILLEGAL_STATE, "No audio channel available!");
    }

    return id;
}

bool AudioChannel::stop() {
    const auto success = audioChannelFile.controlFile(STOP, Util::Array<size_t>());
    if (success) {
        playing = false;
    }

    return success;
}

bool AudioChannel::play() {
    const auto success = audioChannelFile.controlFile(PLAY, Util::Array<size_t>());
    if (success) {
        playing = true;
    }

    return success;
}


AudioChannel::State AudioChannel::getState() const {
    State state;
    const auto success = audioChannelFile.controlFile(GET_PLAYBACK_STATE,
        Util::Array<size_t>({reinterpret_cast<size_t>(&state)}));

    if (!success) {
        Panic::fire(Panic::ILLEGAL_STATE, "Failed to get audio channel state!");
    }

    return state;
}

size_t AudioChannel::getRemainingBytes() const {
    size_t remainingBytes = 0;
    audioChannelFile.controlFile(GET_REMAINING_BYTES, {reinterpret_cast<size_t>(&remainingBytes)});

    return remainingBytes;
}

size_t AudioChannel::getWritableBytes() const {
    size_t writableBytes = 0;
    audioChannelFile.controlFile(GET_WRITABLE_BYTES, {reinterpret_cast<size_t>(&writableBytes)});

    return writableBytes;
}

}
}