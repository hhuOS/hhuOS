/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "Color.h"

class Image {

protected:

    Color *pixelBuf = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t depth = 0;

protected:

    Image() = default;

public:

    Image(const Image &copy) = delete;

    virtual ~Image() = default;

    uint32_t getHeight() const;

    uint32_t getWidth() const;

    uint8_t getColorDepth() const;

    void scaleUp(uint8_t factor);

    void scaleDown(uint8_t factor);

    void draw(uint16_t x, uint16_t y) const;
};

#endif
