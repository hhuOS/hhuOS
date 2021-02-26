/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_PIXELDRAWER_H
#define HHUOS_PIXELDRAWER_H

#include "LinearFrameBuffer.h"

namespace Util {

/**
 * Draws pixels on a LinearFrameBuffer.
 */
class PixelDrawer {

public:

    /**
     * Constructor.
     *
     * @param lfb The linear frame buffer on which to draw pixels.
     */
    explicit PixelDrawer(LinearFrameBuffer &lfb);

    /**
     * Copy constructor.
     */
    PixelDrawer(const PixelDrawer &copy) = delete;

    /**
     * Assignment operator.
     */
    PixelDrawer& operator=(const PixelDrawer & other) = delete;

    /**
     * Destructor.
     */
    ~PixelDrawer() = default;

    /**
     * Draw a pixel at a given position.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param color The color
     */
    void drawPixel(uint16_t x, uint16_t y, Color &color);

private:

    LinearFrameBuffer &lfb;
};

}

#endif
