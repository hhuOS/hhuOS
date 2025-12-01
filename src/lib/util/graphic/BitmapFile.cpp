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

#include "BitmapFile.h"

#include "util/io/file/File.h"
#include "util/io/stream/FileInputStream.h"
#include "util/base/Panic.h"

namespace Util::Graphic {

BitmapFile* BitmapFile::open(const String &path) {
    // Read bitmap file into memory
    const Io::File file(path);
    Io::FileInputStream stream(file);
    auto *buffer = new uint8_t[file.getLength()];
    stream.read(buffer, 0, file.getLength());

    // Get reference to header
    const auto &header = *reinterpret_cast<const Header*>(buffer);

    // Check signature
    if (header.signature[0] != 'B' || header.signature[1] != 'M') {
        Panic::fire(Panic::UNSUPPORTED_OPERATION, "BitmapFile: Invalid bitmap signature");
    }

    // Check if color depth is supported
    if (header.bitmapBitsPerPixel != 16 && header.bitmapBitsPerPixel != 24 && header.bitmapBitsPerPixel != 32) {
        Panic::fire(Panic::UNSUPPORTED_OPERATION, "BitmapFile: Unsupported color depth");
    }

    // Parse bitmap data
    const auto bytesPerPixel = (header.bitmapBitsPerPixel + 7) / 8;
    const auto padding = (4 - ((header.bitmapWidth * bytesPerPixel) % 4)) % 4;
    const auto bitmap = buffer + header.dataOffset;
    auto *pixelBuffer = new Color[header.bitmapWidth * header.bitmapHeight];

    for (int32_t y = 0; y < header.bitmapHeight; y++) {
        for (int32_t x = 0; x < header.bitmapWidth; x++) {
            const auto pos = y * header.bitmapWidth * bytesPerPixel + bytesPerPixel * x + y * padding;
            const auto colorValue = *reinterpret_cast<uint32_t*>(bitmap + pos);

            pixelBuffer[y * header.bitmapWidth + x] = Color::fromRGB(colorValue, header.bitmapBitsPerPixel);
        }
    }

    auto *bitmapFile = new BitmapFile(header.bitmapWidth, header.bitmapHeight, pixelBuffer);
    delete[] buffer;

    return bitmapFile;
}

}