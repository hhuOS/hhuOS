/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "Image.h"

namespace Util::Graphic {

Image* Image::scale(const uint16_t newWidth, const uint16_t newHeight) const {
    auto *newPixelBuffer = new Color[newWidth * newHeight];
    const auto factorX = static_cast<float>(newWidth) / width;
    const auto factorY = static_cast<float>(newHeight) / height;

    for (int32_t y = 0; y < newHeight; y++) {
        for (int32_t x = 0; x < newWidth; x++) {
            const auto oldX = static_cast<uint16_t>(x / factorX);
            const auto oldY = static_cast<uint16_t>(y / factorY);
            newPixelBuffer[newWidth * y + x] = pixelBuffer[width * oldY + oldX];
        }
    }

    return new Image(newWidth, newHeight, newPixelBuffer);
}

}