/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Image.h"

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

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            auto oldX = static_cast<uint16_t>(x / factorX);
            auto oldY = static_cast<uint16_t>(y / factorY);
            newPixelBuffer[newWidth * y + x] = pixelBuffer[width * oldY + oldX];
        }
    }

    return new Image(newWidth, newHeight, newPixelBuffer);
}

}