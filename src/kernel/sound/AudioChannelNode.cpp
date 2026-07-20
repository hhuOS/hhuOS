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

#include "AudioChannelNode.h"

#include "util/sound/AudioChannel.h"

namespace Kernel {

AudioChannelNode::AudioChannelNode(const uint8_t id, AudioChannel &channel, AudioMixer &mixer) :
    MemoryNode(Util::String::format("channel%u", id)), id(id), channel(channel), mixer(mixer) {}

uint64_t AudioChannelNode::writeData(const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    channel.write(sourceBuffer, 0, numBytes);
    return numBytes;
}

int64_t AudioChannelNode::control(uint32_t request, uint32_t, uint32_t, uint32_t) {
    switch (request) {
        case Util::Sound::AudioChannel::PLAY:
            mixer.controlPlayback(Util::Sound::AudioChannel::PLAY, id);
            return 0;
        case Util::Sound::AudioChannel::STOP:
            mixer.controlPlayback(Util::Sound::AudioChannel::STOP, id);
            return 0;
        case Util::Sound::AudioChannel::GET_PLAYBACK_STATE: {
            return channel.getState();
        }
        case Util::Sound::AudioChannel::GET_REMAINING_BYTES: {
            return channel.getReadableBytes();
        }
        case Util::Sound::AudioChannel::GET_WRITABLE_BYTES: {
            return channel.getWritableBytes();
        }
        default:
            return -1;
    }

}

}
