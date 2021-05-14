/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "PixelDrawer.h"

namespace Util::Graphic {

PixelDrawer::PixelDrawer(LinearFrameBuffer &lfb): lfb(lfb) {}

void PixelDrawer::drawPixel(uint16_t x, uint16_t y, Color &color) {
    // Pixels outside the visible area won't be drawn
    if(x > lfb.getResolutionX() - 1 || y > lfb.getResolutionY() - 1) {
        return;
    }

    // Invisible pixels won't be drawn
    if(color.getAlpha() == 0) {
        return;
    }

    auto bpp = static_cast<uint8_t>(lfb.getColorDepth() == 15 ? 16 : lfb.getColorDepth());

    if(color.getAlpha() < 255) {
        Color currentColor = lfb.readPixel(x, y);
        currentColor.blendWith(color);
        color = currentColor;
    }

    uint32_t rgbColor = color.getColorForDepth(lfb.getColorDepth());

    //Calculate pixel offset
    auto address = lfb.getBuffer().add((x * (bpp / 8)) + y * lfb.getPitch());

    //Write color to pixel offset
    for(uint32_t i = 0; i < (bpp / 8); i++) {
        address.setByte((rgbColor >> (i * 8)) & 0xff, i);
    }
}

}