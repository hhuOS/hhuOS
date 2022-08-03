/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_PCSPEAKERNODE_H
#define HHUOS_PCSPEAKERNODE_H

#include "filesystem/memory/MemoryNode.h"

namespace Device::Sound {

class PcSpeakerNode : public Filesystem::Memory::MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit PcSpeakerNode(const Util::Memory::String &name);

    /**
     * Copy Constructor.
     */
    PcSpeakerNode(const PcSpeakerNode &other) = delete;

    /**
     * Assignment operator.
     */
    PcSpeakerNode &operator=(const PcSpeakerNode &other) = delete;

    /**
     * Destructor.
     */
    ~PcSpeakerNode() override = default;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    uint32_t currentFrequency = 0;
    Util::Memory::String buffer = "0";
};

}

#endif
