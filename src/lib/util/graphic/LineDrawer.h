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

#ifndef HHUOS_LINEDRAWER_H
#define HHUOS_LINEDRAWER_H

#include <cstdint>

namespace Util {
namespace Graphic {
class Color;
class PixelDrawer;
}  // namespace Graphic
}  // namespace Util

namespace Util::Graphic {

/**
 * Draws lines on a linear frame buffer using a pixel drawer
 */
class LineDrawer {

public:
    /**
     * Constructor.
     *
     * @param lfb The linear frame buffer on which to draw pixels.
     */
    explicit LineDrawer(const PixelDrawer &pixelDrawer);

    /**
     * Copy Constructor.
     */
    LineDrawer(const LineDrawer &copy) = delete;

    /**
     * Assignment operator.
     */
    LineDrawer& operator=(const LineDrawer & other) = delete;

    /**
     * Destructor.
     */
    ~LineDrawer() = default;

    void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const Color &color) const;

private:

    void drawLineMajorAxis(int32_t x, int32_t y, int8_t xMovement, int8_t yMovement, int32_t dx, int32_t dy, bool majorAxisX, const Color &color) const;

    void drawLineSingleAxis(int32_t x, int32_t y, int8_t movement, int32_t dx, bool majorAxisX, const Color &color) const;

    static void swap(int32_t *a, int32_t *b);

    const PixelDrawer &pixelDrawer;
};

}

#endif
