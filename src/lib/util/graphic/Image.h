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

#ifndef HHUOS_IMAGE_H
#define HHUOS_IMAGE_H

#include <cstdint>

namespace Util {
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

namespace Util::Graphic {

class Image {

public:
    /**
     * Constructor.
     */
    Image(uint16_t width, uint16_t height, Color *pixelBuffer);

    /**
     * Copy Constructor.
     */
    Image(const Image &other) = delete;

    /**
     * Assignment operator.
     */
    Image &operator=(const Image &other) = delete;

    /**
     * Destructor.
     */
    ~Image();

    [[nodiscard]] Graphic::Color *getPixelBuffer() const;

    [[nodiscard]] uint16_t getWidth() const;

    [[nodiscard]] uint16_t getHeight() const;

    [[nodiscard]] Image* scale(uint16_t newWidth, uint16_t newHeight);

private:

    const uint16_t width;
    const uint16_t height;
    Graphic::Color *pixelBuffer;
};

}

#endif
