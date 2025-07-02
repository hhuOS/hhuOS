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

#include "AudioMixerNode.h"

#include "AudioMixer.h"
#include "AudioMixerRunnable.h"
#include "lib/util/sound/AudioChannel.h"

namespace Kernel {

AudioMixerNode::AudioMixerNode(AudioMixer &audioMixer, AudioMixerRunnable &runnable, Thread &audioMixerThread) :
    MemoryNode("audiomixer"), audioMixer(audioMixer), runnable(runnable), audioMixerThread(audioMixerThread) {}

AudioMixerNode::~AudioMixerNode() {
    runnable.stop();
    audioMixerThread.join();

    delete &audioMixer;
}

bool AudioMixerNode::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    if (parameters.length() == 1) {
        switch (request) {
            case Util::Sound::AudioChannel::CREATE: {
                auto *id = reinterpret_cast<uint8_t*>(parameters[0]);
                return audioMixer.createChannel(*id);
            }
            case Util::Sound::AudioChannel::DELETE: {
                const auto id = parameters[0];
                return audioMixer.deleteChannel(id);
            }
            default: {
                const auto id = parameters[0];
                return audioMixer.controlPlayback(request, id);
            }
        }
    }

    return false;
}

}