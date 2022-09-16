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

#ifndef HHUOS_LINEARFRAMEBUFFERNODE_H
#define HHUOS_LINEARFRAMEBUFFERNODE_H

#include "filesystem/memory/MemoryNode.h"
#include "lib/util/graphic/LinearFrameBuffer.h"

namespace Device::Graphic {

class LinearFrameBufferNode : public Filesystem::Memory::MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit LinearFrameBufferNode(const Util::Memory::String &name, Util::Graphic::LinearFrameBuffer *lfb);

    /**
     * Copy Constructor.
     */
    LinearFrameBufferNode(const LinearFrameBufferNode &copy) = delete;

    /**
     * Assignment operator.
     */
    LinearFrameBufferNode& operator=(const LinearFrameBufferNode &other) = delete;

    /**
     * Destructor.
     */
    ~LinearFrameBufferNode() override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    Util::Graphic::LinearFrameBuffer *lfb;

    const Util::Memory::String addressBuffer;
    const Util::Memory::String resolutionBuffer;
    const Util::Memory::String pitchBuffer;

};

}

#endif
