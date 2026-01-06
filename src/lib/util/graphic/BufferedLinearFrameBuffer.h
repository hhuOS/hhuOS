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
 */

#ifndef HHUOS_LIB_UTIL_GRAPHIC_BUFFEREDLINEARFRAMEBUFFER_H
#define HHUOS_LIB_UTIL_GRAPHIC_BUFFEREDLINEARFRAMEBUFFER_H

#include <stdint.h>

#include "util/graphic/LinearFrameBuffer.h"

namespace Util {
namespace Graphic {

/// A double buffered linear frame buffer implementation.
/// All drawing operations are performed on a software buffer which can be flushed to the target frame buffer on demand.
/// It can be initialized with a lower resolution than the target frame buffer and will scale the content accordingly
/// on flush. However, it uses integer scaling factors only and if the scaled resolution is smaller than the target
/// resolution, the content will be centered.
/// For example, a 400x300 buffered frame buffer on a 800x600 target frame buffer will use a scaling factor of 2
/// and fit perfectly, while a 300x200 buffered frame buffer on the same target will use a scaling factor of 2 as well,
/// resulting in a 600x400 output that is centered within the 800x600 target.
class BufferedLinearFrameBuffer final : public LinearFrameBuffer {

public:
    /// Create a new buffered linear frame buffer instance that matches the resolution of the target frame buffer.
    explicit BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb) :
        BufferedLinearFrameBuffer(lfb, lfb.getResolutionX(), lfb.getResolutionY(), lfb.getPitch()) {}

    /// Create a new buffered linear frame buffer instance with the given resolution.
    /// The resolution must not be larger than the target frame buffer's resolution.
    /// The content is scaled on flush to fit into the target frame buffer.
    /// If the given resolution is larger than the target frame buffer's resolution, a panic is fired.
    BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, const uint16_t resolutionX, const uint16_t resolutionY) :
        BufferedLinearFrameBuffer(lfb, resolutionX, resolutionY, lfb.getColorDepth() < 8 ?
            resolutionX / (8 / lfb.getColorDepth()) : resolutionX * ((lfb.getColorDepth() + 7) / 8)) {}

    /// Create a new buffered linear frame buffer instance with the given scale (> 0 and <= 1).
    /// The scale is applied to the target frame buffer's resolution to determine the buffered resolution.
    /// The content is scaled on flush to fit into the target frame buffer.
    BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, const float scaleFactor) :
        BufferedLinearFrameBuffer(lfb,static_cast<uint16_t>(lfb.getResolutionX() * scaleFactor),
            static_cast<uint16_t>(lfb.getResolutionY() * scaleFactor)) {}

    /// Flush the content of the buffered frame buffer to the target frame buffer.
    void flush() const;

private:

    BufferedLinearFrameBuffer(const LinearFrameBuffer &lfb, uint16_t resolutionX, uint16_t resolutionY, uint16_t pitch);

    uint8_t scale = 0;
    uint16_t offsetX = 0;
    uint16_t offsetY = 0;

    const LinearFrameBuffer &lfb;
};

}
}

#endif