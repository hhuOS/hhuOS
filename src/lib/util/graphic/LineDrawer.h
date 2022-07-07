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

#include "PixelDrawer.h"

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
    explicit LineDrawer(PixelDrawer &pixelDrawer);

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

    void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const Color &color);

private:

    void drawLineMajorAxis(uint32_t pixel, int32_t majorAxisPixelMovement, int32_t minorAxisPixelMovement, int32_t dx, int32_t dy, const Color &color);

    void drawLineSingleAxis(uint32_t pixel, int32_t majorAxisPixelMovement, int32_t dx, const Color &color);

    static void swap(uint16_t *a, uint16_t *b);

    static int32_t abs(int32_t a);

    PixelDrawer &pixelDrawer;
};

}

#endif
