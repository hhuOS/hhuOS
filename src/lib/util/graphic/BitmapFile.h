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

#ifndef HHUOS_BITMAPFILE_H
#define HHUOS_BITMAPFILE_H

#include <stdint.h>

#include "lib/util/base/String.h"
#include "lib/util/graphic/Image.h"

namespace Util {
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

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

    struct Header {
        char signature[2];
        uint32_t size;
        uint32_t reserved;
        uint32_t dataOffset;
        uint32_t dibHeaderSize;
        uint32_t bitmapWidth;
        uint32_t bitmapHeight;
        uint16_t bitmapCountColorPlanes;
        uint16_t bitmapBitsPerPixel;
    } __attribute__((__packed__));

private:

    BitmapFile(uint16_t width, uint16_t height, Color *pixelBuffer);
};

}

#endif
