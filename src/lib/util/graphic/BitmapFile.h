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

#ifndef HHUOS_BITMAPFILE_H
#define HHUOS_BITMAPFILE_H

#include <cstdint>
#include "lib/util/graphic/Color.h"
#include "lib/util/base/String.h"
#include "lib/util/graphic/Image.h"

namespace Util::Graphic {

class BitmapFile : public Image {

public:
    /**
     * Copy Constructor.
     */
    BitmapFile(const BitmapFile &other) = delete;

    /**
     * Assignment operator.
     */
    BitmapFile &operator=(const BitmapFile &other) = delete;

    /**
     * Destructor.
     */
    ~BitmapFile() = default;

    static BitmapFile* open(const String &path);

private:

    struct Header {
        char signature[2];
        uint8_t size[4];
        uint8_t reserved[4];
        uint8_t dataOffset[4];
        uint8_t dibHeaderSize[4];
        uint8_t bitmapWidth[4];
        uint8_t bitmapHeight[4];
        uint8_t bitmapCountColorPlanes[2];
        uint8_t bitmapBitsPerPixel[2];
    };

    BitmapFile(uint16_t width, uint16_t height, Color *pixelBuffer);
};

}

#endif
