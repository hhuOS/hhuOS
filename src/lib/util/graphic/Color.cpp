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

#include "Color.h"

Util::Graphic::Color Util::Graphic::Color::fromRGB(const uint32_t rgb, const uint8_t depth) {
    static constexpr void *LABELS[] = {
        &&INVALID_DEPTH,
        &&FROM_RGB1,
        &&FROM_RGB2,
        &&INVALID_DEPTH,
        &&FROM_RGB4,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&FROM_RGB8,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&FROM_RGB15,
        &&FROM_RGB16,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&FROM_RGB24,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&FROM_RGB32
    };

    if (depth > 32) {
        goto INVALID_DEPTH;
    }
    goto *LABELS[depth];

    FROM_RGB32:
    return fromRGB32(rgb);

    FROM_RGB24:
    return fromRGB24(rgb);

    FROM_RGB16:
    return fromRGB16(rgb);

    FROM_RGB15:
    return fromRGB15(rgb);

    FROM_RGB8:
    return fromRGB8(rgb);

    FROM_RGB4:
    return fromRGB4(rgb);

    FROM_RGB2:
    return fromRGB2(rgb);

    FROM_RGB1:
    return fromRGB1(rgb);

    INVALID_DEPTH:
    Util::Panic::fire(Panic::INVALID_ARGUMENT, "Unsupported color depth!");
}

uint32_t Util::Graphic::Color::getColorForDepth(const uint8_t depth) const {
    static constexpr void *LABELS[] = {
        &&INVALID_DEPTH,
        &&GET_RGB1,
        &&GET_RGB2,
        &&INVALID_DEPTH,
        &&GET_RGB4,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&GET_RGB8,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&GET_RGB15,
        &&GET_RGB16,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&GET_RGB24,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&INVALID_DEPTH,
        &&GET_RGB32
    };

    if (depth > 32) {
        goto INVALID_DEPTH;
    }
    goto *LABELS[depth];

    GET_RGB32:
    return getRGB32();

    GET_RGB24:
    return getRGB24();

    GET_RGB16:
    return getRGB16();

    GET_RGB15:
    return getRGB15();

    GET_RGB8:
    return getRGB8();

    GET_RGB4:
    return getRGB4();

    GET_RGB2:
    return getRGB2();

    GET_RGB1:
    return getRGB1();

    INVALID_DEPTH:
    Util::Panic::fire(Panic::INVALID_ARGUMENT, "Unsupported color depth!");
}
