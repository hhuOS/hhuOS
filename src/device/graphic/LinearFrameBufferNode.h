/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <cstdint>

#include "filesystem/memory/StringNode.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"

namespace Device {
namespace Graphic {
class VesaBiosExtensions;
}  // namespace Graphic
}  // namespace Device

namespace Util {
namespace Graphic {
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

namespace Device::Graphic {

class LinearFrameBufferNode : public Filesystem::Memory::StringNode {

public:
    /**
     * Constructor.
     */
    explicit LinearFrameBufferNode(const Util::String &name, const Util::Graphic::LinearFrameBuffer &lfb);

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
     * Overriding function from StringNode.
     */
    Util::String getString() override;

    /**
     * Overriding function from Node.
     */
    bool control(uint32_t request, const Util::Array<uint32_t> &parameters) override;

private:

    void *physicalAddress;
    uint16_t resolutionX;
    uint16_t resolutionY;
    uint8_t colorDepth;
    uint16_t pitch;
    Device::Graphic::VesaBiosExtensions *vbe = nullptr;
};

}

#endif
