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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_IMAGE_H
#define HHUOS_LIB_UTIL_GRAPHIC_IMAGE_H

#include <stdint.h>

#include "util/graphic/Color.h"

namespace Util {
namespace Graphic {

/// Represents a 2D image with a specified width, height, and pixel buffer.
/// The pixel buffer is an array of Color objects representing the colors of each pixel in the image.
/// Thus, this class is essentially just a simple wrapper around a pixel buffer with width and height information.
/// It is used as a base class for more specific image formats, such as `BitmapFile`.
/// Furthermore, it allows scaling the image to a new width and height.
class Image {

public:
    /// Create a new image instance, wrapping a pixel buffer with the specified width, and height.
    /// The pixel buffer must be heap allocated and the image instance will take ownership of it.
    /// This means that the pixel buffer will be deleted when the image instance is destroyed.
    Image(const uint16_t width, const uint16_t height, const Color *pixelBuffer) :
        width(width), height(height), pixelBuffer(pixelBuffer) {}

    /// Image is not copyable, since it manages the heap-allocated pixel buffer, so the copy constructor is deleted.
    Image(const Image &other) = delete;

    /// Image is not copyable, since it manages the heap-allocated pixel buffer, so the assignment operator is deleted.
    Image &operator=(const Image &other) = delete;

    /// Destroy the image instance and free the pixel buffer.
    ~Image() {
        delete[] pixelBuffer;
    }

    /// Get the pixel buffer of the image.
    /// The returned pointer is const, so the pixel buffer cannot be modified directly.
    /// The image instance keeps ownership of the pixel buffer, so it must not be deleted manually.
    const Color* getPixelBuffer() const {
        return pixelBuffer;
    }

    /// Get the width of the image in pixels.
    uint16_t getWidth() const {
        return width;
    }

    /// Get the height of the image in pixels.
    uint16_t getHeight() const {
        return height;
    }

    /// Scale the image to the specified new width and height.
    /// This method returns a new image instance with the scaled pixel buffer.
    /// The scaling is done using nearest-neighbor interpolation.
    /// The returned image instance is allocated on the heap and must be deleted manually.
    Image* scale(uint16_t newWidth, uint16_t newHeight) const;

private:

    const uint16_t width;
    const uint16_t height;
    const Color *pixelBuffer;
};

}
}

#endif
