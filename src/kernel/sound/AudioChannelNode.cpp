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
 *
 * The audio mixer is based on a bachelor's thesis, written by Andreas Lüpertz.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-ANLU89
 */

#include "AudioChannelNode.h"

#include "sound/AudioChannel.h"

namespace Kernel {

AudioChannelNode::AudioChannelNode(const uint8_t id, Util::Io::Pipe &audioPipe, AudioMixer &mixer) :
    MemoryNode(Util::String::format("channel%u", id)), audioPipe(audioPipe), mixer(mixer) {}

uint64_t AudioChannelNode::writeData(const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    audioPipe.write(sourceBuffer, 0, numBytes);
    return numBytes;
}

bool AudioChannelNode::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Sound::AudioChannel::GET_REMAINING_BYTES: {
            auto *remainingBytes = reinterpret_cast<uint32_t*>(parameters[0]);
            *remainingBytes = audioPipe.getReadableBytes();
            return true;
        }
        default:
            return mixer.controlPlayback(request, parameters[0]);
    }

}

}
