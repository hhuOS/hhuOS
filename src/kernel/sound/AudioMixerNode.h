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

#ifndef HHUOS_AUDIOMIXERNODE_H
#define HHUOS_AUDIOMIXERNODE_H

#include "filesystem/memory/MemoryNode.h"
#include "kernel/sound/AudioMixer.h"

namespace Kernel {

/**
 * Represents the node in the filesystem where AudioChannel objects send their audio data packets and requests to.
 * Transmits data to @param &audioMixer and evokes it's methods corresponding to the type of request made.
 */
class AudioMixerNode : public Filesystem::Memory::MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit AudioMixerNode(AudioMixer &audioMixer, AudioMixerRunnable &runnable, Kernel::Thread &audioMixerThread);

    /**
    * Copy Constructor.
    */
    AudioMixerNode(const AudioMixerNode &other) = delete;

    /**
     * Assignment operator.
     */
    AudioMixerNode &operator=(const AudioMixerNode &other) = delete;

    /**
     * Destructor.
     */
    ~AudioMixerNode() override;

    bool control(uint32_t request, const Util::Array<uint32_t> &parameters) override;

private:

    AudioMixer &audioMixer;
    AudioMixerRunnable &runnable;
    Thread &audioMixerThread;
};

}

#endif
