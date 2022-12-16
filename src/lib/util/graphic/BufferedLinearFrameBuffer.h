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

#ifndef HHUOS_BUFFEREDLINEARFRAMEBUFFER_H
#define HHUOS_BUFFEREDLINEARFRAMEBUFFER_H

#include <cstdint>

#include "LinearFrameBuffer.h"

namespace Util {
namespace Memory {
template <typename T> class Address;
}  // namespace Memory
}  // namespace Util

namespace Util::Graphic {

class BufferedLinearFrameBuffer : public LinearFrameBuffer {

public:
    /**
     * Constructor.
     *
     * @param lfb The linear frame buffer, that shall be double buffered.
     */
    explicit BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, bool enableAcceleration = true);

    /**
     * Assignment operator.
     */
    BufferedLinearFrameBuffer& operator=(const BufferedLinearFrameBuffer &other) = delete;

    /**
     * Copy Constructor.
     */
    BufferedLinearFrameBuffer(const BufferedLinearFrameBuffer &copy) = delete;

    /**
     * Destructor.
     */
    ~BufferedLinearFrameBuffer() override;

    void flush() const;

private:

    bool useMmx = false;
    const Memory::Address<uint32_t> &targetBuffer;
};

}

#endif
