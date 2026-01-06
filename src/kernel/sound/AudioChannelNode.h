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

#ifndef AUDIOCHANNELNODE_H
#define AUDIOCHANNELNODE_H

#include "AudioMixer.h"
#include "filesystem/memory/MemoryNode.h"

namespace Kernel {

class AudioChannelNode : public Filesystem::Memory::MemoryNode {

public:
    /**
     * Default Constructor.
     */
    explicit AudioChannelNode(uint8_t id, AudioChannel &channel, AudioMixer &mixer);

    /**
     * Copy Constructor.
     */
    AudioChannelNode(const AudioChannelNode &other) = delete;

    /**
     * Assignment operator.
     */
    AudioChannelNode &operator=(const AudioChannelNode &other) = delete;

    /**
     * Destructor.
     */
    ~AudioChannelNode() override = default;

    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

    bool control(uint32_t request, const Util::Array<uint32_t> &parameters) override;

private:

    const uint8_t id;
    AudioChannel &channel;
    AudioMixer &mixer;
};

}

#endif
