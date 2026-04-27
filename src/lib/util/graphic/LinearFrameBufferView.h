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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_LINEARFRAMEBUFFERVIEW_H
#define HHUOS_LIB_UTIL_GRAPHIC_LINEARFRAMEBUFFERVIEW_H

#include <stdint.h>

#include "LinearFrameBuffer.h"

namespace Util {
namespace Graphic {

/// Provides a transparent view into a rectangular cutout of a larger linear frame buffer.
/// All draw commands will directly draw into the parent buffer, but with respect to the position and size of the view.
///
/// ### Example
/// ```c++
/// // Open the LFB and create a 200x200 pixel view at position (100, 100)
/// auto lfb = Util::Graphic::LinearFrameBuffer(Util::Io::File("/device/lfb"));
/// auto lfbView = Util::Graphic::LinearFrameBufferView(lfb, 100, 100, 200, 200);
///
/// // Draw a filled square at position (25, 25) of the view.
/// // The square will actually be drawn at position (125, 125) of the parent linear frame buffer.
/// lfbView.fillSquare(25, 25, 50, Util::Graphic::Colors::BLUE);
/// ```
class LinearFrameBufferView : public LinearFrameBuffer {

public:

    /// Create a new cutout view into a given linear frame buffer.
    LinearFrameBufferView(const LinearFrameBuffer &lfb, const int32_t posX, const int32_t posY,
        const uint16_t width, const uint16_t height) : LinearFrameBuffer(lfb.getBuffer().getAsPointer(),
            lfb.getResolutionX(), lfb.getResolutionY(), lfb.getColorDepth(), lfb.getPitch(), false),
        posX(posX), posY(posY), width(width), height(height) {}

    /// Set the pixel at the given coordinates to the specified color.
    /// The given coordinates are converted to coordinates in the parent buffer,
    /// and the pixel is drawn directly into the parent buffer.
    void drawPixel(const int32_t x, const int32_t y, const Color &color) const override {
        if (x > width || x < 0 || y > height || y < 0) {
            return;
        }

        LinearFrameBuffer::drawPixel(posX + x, posY + y, color);
    }

private:

    int32_t posX;
    int32_t posY;
    uint16_t width;
    uint16_t height;

};

}
}

#endif
