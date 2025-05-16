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

#include "lib/util/graphic/Color.h"

namespace Util::Graphic {

Image::Image(uint16_t width, uint16_t height, Color *pixelBuffer) : width(width), height(height), pixelBuffer(pixelBuffer) {

}

Image::~Image() {
    delete[] pixelBuffer;
}

Graphic::Color* Image::getPixelBuffer() const {
    return pixelBuffer;
}

uint16_t Image::getWidth() const {
    return width;
}

uint16_t Image::getHeight() const {
    return height;
}

Image* Image::scale(uint16_t newWidth, uint16_t newHeight) {
    auto *newPixelBuffer = new Graphic::Color[newWidth * newHeight];
    double factorX = static_cast<double>(newWidth) / width;
    double factorY = static_cast<double>(newHeight) / height;

    for (int32_t y = 0; y < newHeight; y++) {
        for (int32_t x = 0; x < newWidth; x++) {
            auto oldX = static_cast<uint16_t>(x / factorX);
            auto oldY = static_cast<uint16_t>(y / factorY);
            newPixelBuffer[newWidth * y + x] = pixelBuffer[width * oldY + oldX];
        }
    }

    return new Image(newWidth, newHeight, newPixelBuffer);
}

}