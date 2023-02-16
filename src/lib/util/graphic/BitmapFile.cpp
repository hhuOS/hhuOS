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

#include "BitmapFile.h"

#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/base/Exception.h"
#include "lib/util/graphic/Color.h"

namespace Util::Graphic {

BitmapFile::BitmapFile(uint16_t width, uint16_t height, Color *pixelBuffer) : Graphic::Image(width, height, pixelBuffer) {}

BitmapFile* BitmapFile::open(const String &path) {
    auto file = Io::File(path);
    auto stream = Io::FileInputStream(file);
    auto *buffer = new uint8_t[file.getLength()];
    stream.read(buffer, 0, file.getLength());

    auto &header = *reinterpret_cast<const Header*>(buffer);

    auto dataOffset = (*(header.dataOffset + 3) << 24) | (*(header.dataOffset + 2) << 16) | (*(header.dataOffset + 1) << 8) | *header.dataOffset;
    auto bitmapWidth = (*(header.bitmapWidth + 3) << 24) | (*(header.bitmapWidth + 2) << 16) | (*(header.bitmapWidth + 1) << 8) | *header.bitmapWidth;
    auto bitmapHeight = (*(header.bitmapHeight + 3) << 24) | (*(header.bitmapHeight + 2) << 16) | (*(header.bitmapHeight + 1) << 8) | *header.bitmapHeight;
    auto bitmapBitsPerPixel = (*(header.bitmapBitsPerPixel + 1) << 8) | *header.bitmapBitsPerPixel;

    auto pixelLength = bitmapBitsPerPixel / 8;
    if (pixelLength != 3 && pixelLength != 4) {
        Exception::throwException(Exception::UNSUPPORTED_OPERATION, "BitmapFile: Unsupported color depth");
    }

    auto bitmap = buffer + dataOffset;
    auto *pixelBuffer = new Graphic::Color[bitmapWidth * bitmapHeight];
    for (auto y = bitmapHeight - 1; y >= 0; y--) {
        for (auto x = 0; x < bitmapWidth; x++) {
            uint32_t pos = (y * bitmapWidth * pixelLength) + (pixelLength * x);

            auto blue = *(bitmap + pos);
            auto green = *(bitmap + pos + 1);
            auto red = *(bitmap + pos + 2);
            auto alpha = 0;
            if (pixelLength == 4) {
                alpha = *(bitmap + pos + 3);
            }

            auto color = Graphic::Color(red, green, blue, alpha);
            pixelBuffer[(y * bitmapWidth) + x] = color;
        }
    }

    delete[] bitmap;
    return new BitmapFile(bitmapWidth, bitmapHeight, pixelBuffer);
}

}