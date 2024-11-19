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

#ifndef HHUOS_BUFFERSCROLLER_H
#define HHUOS_BUFFERSCROLLER_H

#include <stdint.h>

namespace Util {
namespace Graphic {
class LinearFrameBuffer;
}  // namespace Graphic

}  // namespace Util

namespace Util::Graphic {

class BufferScroller {

public:
    /**
     * Constructor.
     *
     * @param lfb The linear frame buffer on which to draw pixels.
     */
    explicit BufferScroller(const LinearFrameBuffer &lfb);

    /**
     * Copy Constructor.
     */
    BufferScroller(const BufferScroller &copy) = delete;

    /**
     * Assignment operator.
     */
    BufferScroller& operator=(const BufferScroller & other) = delete;

    /**
     * Destructor.
     */
    ~BufferScroller() = default;

    /**
     * Scroll the buffer upwards by a given amount of pixel lines.
     *
     * @param lineCount The amount of pixel lines to scroll up
     */
    void scrollUp(uint16_t lineCount, bool clearBelow) const;

private:

    bool useMmx = false;
    const LinearFrameBuffer &lfb;
};

}

#endif
