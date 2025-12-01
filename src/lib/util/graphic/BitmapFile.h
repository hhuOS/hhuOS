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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_BITMAPFILE_H
#define HHUOS_LIB_UTIL_GRAPHIC_BITMAPFILE_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/graphic/Image.h"

namespace Util::Graphic {

/// Can be used to load bitmap files (BMP) from disk.
/// Only uncompressed bitmaps with 16, 24, or 32 bits per pixel are supported.
/// Files are expected to have the "Windows BITMAPINFOHEADER" DIB header format.
/// This class extends the `Image` class, so it can be used wherever an `Image` is expected.
/// Use the static `open()` method to load a bitmap file from disk.
class BitmapFile : public Image {

public:
    /// Open a bitmap file from the specified path and return a pointer to a `BitmapFile` instance.
    /// The returned instance must be deleted after use to free the allocated memory.
    /// An invalid path or bitmap file (e.g., unsupported format) will fire a panic.
    static BitmapFile* open(const String &path);

    /// Bitmap file header structure as defined by the "Windows BITMAPINFOHEADER" format.
    struct Header {
        /// Bitmap file signature (must be 'B' and 'M').
        char signature[2];
        /// Size of the bitmap file in bytes.
        uint32_t size;
        /// Reserved field (ignored for parsing).
        uint32_t reserved;
        /// Offset to the start of the bitmap data in bytes.
        uint32_t dataOffset;
        /// DIB header size in bytes (must be 40 for "Windows BITMAPINFOHEADER").
        uint32_t dibHeaderSize;
        /// Width of the bitmap in pixels.
        int32_t bitmapWidth;
        /// Height of the bitmap in pixels.
        int32_t bitmapHeight;
        /// Number of color planes (must be 1).
        uint16_t bitmapCountColorPlanes;
        /// Bits per pixel (color depth).
        uint16_t bitmapBitsPerPixel;
    } __attribute__((packed));

private:

    BitmapFile(const uint16_t width, const uint16_t height, const Color *pixelBuffer) :
        Image(width, height, pixelBuffer) {}
};

}

#endif
