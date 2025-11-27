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

#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/base/Panic.h"
#include "lib/util/graphic/Color.h"

namespace Util::Graphic {

BitmapFile::BitmapFile(uint16_t width, uint16_t height, Color *pixelBuffer) : Graphic::Image(width, height, pixelBuffer) {}

BitmapFile* BitmapFile::open(const String &path) {
    const Io::File file(path);
    Io::FileInputStream stream(file);
    auto *buffer = new uint8_t[file.getLength()];
    stream.read(buffer, 0, file.getLength());

    auto &header = *reinterpret_cast<const Header*>(buffer);

    auto pixelLength = header.bitmapBitsPerPixel / 8;
    if (pixelLength != 3 && pixelLength != 4) {
        Panic::fire(Panic::UNSUPPORTED_OPERATION, "BitmapFile: Unsupported color depth");
    }

    auto padding = (4 - ((header.bitmapWidth * pixelLength) % 4)) % 4;
    auto bitmap = buffer + header.dataOffset;
    auto *pixelBuffer = new Graphic::Color[header.bitmapWidth * header.bitmapHeight];

    for (uint32_t y = 0; y < header.bitmapHeight; y++) {
        for (uint32_t x = 0; x < header.bitmapWidth; x++) {
            uint32_t pos = (y * header.bitmapWidth * pixelLength) + (pixelLength * x) + y * padding;

            auto blue = *(bitmap + pos);
            auto green = *(bitmap + pos + 1);
            auto red = *(bitmap + pos + 2);
            auto alpha = 0xff;
            if (pixelLength == 4) {
                alpha = *(bitmap + pos + 3);
            }

            auto color = Graphic::Color(red, green, blue, alpha);
            pixelBuffer[(y * header.bitmapWidth) + x] = color;
        }
    }

    delete[] bitmap;
    return new BitmapFile(header.bitmapWidth, header.bitmapHeight, pixelBuffer);
}

}