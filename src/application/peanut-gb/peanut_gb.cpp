/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Peanut-GB/peanut_gb.h"

#include "lib/util/base/Exception.h"
#include "stdio.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/base/System.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "stdlib.h"
#include "time.h"

const constexpr uint32_t TARGET_FRAME_RATE = 60;
const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));

const auto palettes = new Util::Graphic::Color[31][3][4] {
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF7300), Util::Graphic::Color::fromRGB32(0x944200), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xADAD84), Util::Graphic::Color::fromRGB32(0x42737B), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFC542), Util::Graphic::Color::fromRGB32(0xFFD600), Util::Graphic::Color::fromRGB32(0x943A00), Util::Graphic::Color::fromRGB32(0x4A0000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFF9C), Util::Graphic::Color::fromRGB32(0x94B5FF), Util::Graphic::Color::fromRGB32(0x639473), Util::Graphic::Color::fromRGB32(0x003A3A) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x6BFF00), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF524A), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x52DE00), Util::Graphic::Color::fromRGB32(0xFF8400), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFFFFFF) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF00), Util::Graphic::Color::fromRGB32(0xB57300), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x52FF00), Util::Graphic::Color::fromRGB32(0xFF4200), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF9C00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFF6352), Util::Graphic::Color::fromRGB32(0xD60000), Util::Graphic::Color::fromRGB32(0x630000), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF7B), Util::Graphic::Color::fromRGB32(0x0084FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xA59CFF), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0x006300), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF7300), Util::Graphic::Color::fromRGB32(0x944200), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFCE), Util::Graphic::Color::fromRGB32(0x63EFEF), Util::Graphic::Color::fromRGB32(0x9C8431), Util::Graphic::Color::fromRGB32(0x5A5A5A) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0x000000), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0x000000), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xB5B5FF), Util::Graphic::Color::fromRGB32(0xFFFF94), Util::Graphic::Color::fromRGB32(0xAD5A42), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF7B), Util::Graphic::Color::fromRGB32(0x0084FF), Util::Graphic::Color::fromRGB32(0xFF0000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFC542), Util::Graphic::Color::fromRGB32(0xFFD600), Util::Graphic::Color::fromRGB32(0x943A00), Util::Graphic::Color::fromRGB32(0x4A0000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x8C8CDE), Util::Graphic::Color::fromRGB32(0x52528C), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x8C8CDE), Util::Graphic::Color::fromRGB32(0x52528C), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x00FF00), Util::Graphic::Color::fromRGB32(0x318400), Util::Graphic::Color::fromRGB32(0x004A00) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x004A00) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x000000), Util::Graphic::Color::fromRGB32(0x008484), Util::Graphic::Color::fromRGB32(0xFFDE00), Util::Graphic::Color::fromRGB32(0xFFFFFF) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x630000), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x630000), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xA5A5A5), Util::Graphic::Color::fromRGB32(0x525252), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xA5A5A5), Util::Graphic::Color::fromRGB32(0x525252), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xA5A5A5), Util::Graphic::Color::fromRGB32(0x525252), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFA5), Util::Graphic::Color::fromRGB32(0xFF9494), Util::Graphic::Color::fromRGB32(0x9494FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFA5), Util::Graphic::Color::fromRGB32(0xFF9494), Util::Graphic::Color::fromRGB32(0x9494FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFA5), Util::Graphic::Color::fromRGB32(0xFF9494), Util::Graphic::Color::fromRGB32(0x9494FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFE6C5), Util::Graphic::Color::fromRGB32(0xCE9C84), Util::Graphic::Color::fromRGB32(0x846B29), Util::Graphic::Color::fromRGB32(0x5A3108) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0x7B4A00), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFCE00), Util::Graphic::Color::fromRGB32(0x9C6300), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFCE00), Util::Graphic::Color::fromRGB32(0x9C6300), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFCE00), Util::Graphic::Color::fromRGB32(0x9C6300), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x0063C5), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        { // Black and White (not included in CGB boot rom)
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xAAAAAA), Util::Graphic::Color::fromRGB32(0x555555), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xAAAAAA), Util::Graphic::Color::fromRGB32(0x555555), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xAAAAAA), Util::Graphic::Color::fromRGB32(0x555555), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        { // Shades of green (not included in CGB boot rom)
                { Util::Graphic::Color::fromRGB32(0x9bbc0f), Util::Graphic::Color::fromRGB32(0x8bac0f), Util::Graphic::Color::fromRGB32(0x306230), Util::Graphic::Color::fromRGB32(0x0f380f) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0x9bbc0f), Util::Graphic::Color::fromRGB32(0x8bac0f), Util::Graphic::Color::fromRGB32(0x306230), Util::Graphic::Color::fromRGB32(0x0f380f) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x9bbc0f), Util::Graphic::Color::fromRGB32(0x8bac0f), Util::Graphic::Color::fromRGB32(0x306230), Util::Graphic::Color::fromRGB32(0x0f380f) }  // BG
        }
};

const auto manualPalettes = new uint16_t[14] {
    0x1d << 8 | 0x05, // Black and White
    0x1e << 8 | 0x05, // Shades of green
    0x12 << 8 | 0x00, // Up
    0x10 << 8 | 0x05, // A + Up
    0x19 << 8 | 0x03, // B + Up
    0x18 << 8 | 0x05, // Left
    0x0d << 8 | 0x05, // A + Left
    0x16 << 8 | 0x00, // B + Left
    0x17 << 8 | 0x00, // Down
    0x07 << 8 | 0x00, // A + Down
    0x1a << 8 | 0x05, // B + Down
    0x05 << 8 | 0x00, // Right
    0x1c << 8 | 0x03, // A + Right
    0x13 << 8 | 0x00 // B + Right
};

uint32_t *palette = nullptr;
uint8_t *rom = nullptr;
uint8_t *ram = nullptr;
uint8_t scale = 1;
uint8_t maxScale = 1;
uint16_t offsetX = 0;
uint16_t offsetY = 0;
Util::Graphic::LinearFrameBuffer *lfb = nullptr;
Util::String saveFilePath;

Util::Time::Timestamp fpsTimer;
uint32_t fpsCounter = 0;
uint32_t fps = 0;

uint32_t* get_palette(uint8_t index, uint8_t flags) {
    auto *pal = palettes[index];
    auto ret = new uint32_t[3][4];

    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            ret[i][j] = pal[i][j].getColorForDepth(lfb->getColorDepth());
        }
    }

    switch (flags) {
        case 0x00: // OBJ0 = BG, OBJ1 = BG
            ret[0][0] = ret[2][0];
            ret[0][1] = ret[2][1];
            ret[0][2] = ret[2][2];

            ret[1][0] = ret[2][0];
            ret[1][1] = ret[2][1];
            ret[1][2] = ret[2][2];
            break;
        case 0x01: // OBJ1 = BG
            ret[1][0] = ret[2][0];
            ret[1][1] = ret[2][1];
            ret[1][2] = ret[2][2];
            break;
        case 0x02: // OBJ1 = OBJ0, OBJ0 = BG
            ret[1][0] = ret[0][0];
            ret[1][1] = ret[0][1];
            ret[1][2] = ret[0][2];

            ret[0][0] = ret[2][0];
            ret[0][1] = ret[2][1];
            ret[0][2] = ret[2][2];
        case 0x03: // OBJ1 = OBJ0
            ret[1][0] = ret[0][0];
            ret[1][1] = ret[0][1];
            ret[1][2] = ret[0][2];
            break;
        case 0x04: // OBJ0 = BG
            ret[0][0] = ret[2][0];
            ret[0][1] = ret[2][1];
            ret[0][2] = ret[2][2];
            break;
        default:
            break;
    }

    return reinterpret_cast<uint32_t*>(ret);
}

uint32_t* get_manual_palette(uint8_t manualIndex) {
    auto index = manualPalettes[manualIndex] >> 8;
    auto flags = manualPalettes[manualIndex] & 0xff;

    return get_palette(index, flags);
}

uint32_t* gb_get_palette(gb_s *gb) {
    char title[16]{};
    gb_get_rom_name(gb, title);
    auto hash = gb_colour_hash(gb);

    switch (hash) {
        case 0xb3:
        if (title[3] == 'U') {
            return get_palette(0x00, 0x03);
        } else if (title[3] == 'R') {
            return get_palette(0x05, 0x04);
        } else if (title[3] == 'B') {
            return get_palette(0x08, 0x05);
        }
        break;
        case 0x59:
            return get_palette(0x00, 0x05);
        case 0xc6:
        if (title[3] == 'A') {
            return get_palette(0x00, 0x05);
        } else
        if (title[3] == ' ') {
            return get_palette(0x1c, 0x03);
        }
        break;
        case 0x8c:
            return get_palette(0x01, 0x00);
        case 0x86:
        case 0xa8:
            return get_palette(0x01, 0x05);
        case 0xbf:
        if (title[3] == 'C') {
            return get_palette(0x02, 0x05);
        } else
        if (title[3] == ' ') {
            return get_palette(0x0d, 0x03);
        }
        break;
        case 0xce:
        case 0xd1:
        case 0xf0:
            return get_palette(0x02, 0x05);
        case 0x36:
            return get_palette(0x03, 0x05);
        case 0x34:
            return get_palette(0x04, 0x03);
        case 0x66:
        if (title[3] == 'E') {
            return get_palette(0x04, 0x03);
        } else if (title[3] == 'L') {
            return get_palette(0x1c, 0x03);
        }
        break;
        case 0xf4:
        if (title[3] == ' ') {
            return get_palette(0x04, 0x03);
        } else
        if (title[3] == '-') {
            return get_palette(0x1c, 0x05);
        }
        break;
        case 0x3d:
            return get_palette(0x05, 0x03);
        case 0x6a:
        if (title[3] == 'I') {
            return get_palette(0x05, 0x03);
        } else if (title[3] == 'K') {
            return get_palette(0x0c, 0x05);
        }
        break;
        case 0x95:
            return get_palette(0x05, 0x04);
        case 0x71:
        case 0xff:
            return get_palette(0x06, 0x00);
        case 0x19:
            return get_palette(0x06, 0x03);
        case 0x3e:
        case 0xe0:
            return get_palette(0x06, 0x04);
        case 0x15:
        case 0xdb:
            return get_palette(0x07, 0x00);
        case 0x0d:
            if (title[3] == 'R') {
                return get_palette(0x07, 0x04);
            } else if (title[3] == 'E') {
                return get_palette(0x0c, 0x03);
            }
        break;
        case 0x69:
        case 0xf2:
            return get_palette(0x07, 0x04);
        case 0x88:
            return get_palette(0x08, 0x00);
        case 0x1d:
            return get_palette(0x08, 0x03);
        case 0x27:
            if (title[3] == 'B') {
                return get_palette(0x08, 0x05);
            } else if (title[3] == 'N') {
                return get_palette(0x0e, 0x05);
            }
        break;
        case 0x49:
        case 0x5c:
            return get_palette(0x08, 0x05);
        case 0xc9:
            return get_palette(0x09, 0x05);
        case 0x46:
            if (title[3] == 'E') {
                return get_palette(0x0a, 0x03);
            } else if (title[3] == 'R') {
                return get_palette(0x14, 0x05);
            }
        break;
        case 0x61:
            if (title[3] == 'E') {
                return get_palette(0x0b, 0x01);
            } else if (title[3] == 'A') {
                return get_palette(0x0e, 0x05);
            }
            break;
        case 0x3c:
            return get_palette(0x0b, 0x02);
        case 0x4e:
            return get_palette(0x0b, 0x05);
        case 0x9c:
            return get_palette(0x0c, 0x02);
        case 0x18:
            if (title[3] == 'K') {
                return get_palette(0x0c, 0x05);
            } else
            if (title[3] == 'I') {
                return get_palette(0x1c, 0x03);
            }
            break;
        case 0x6b:
            return get_palette(0x0c, 0x05);
        case 0xd3:
            if (title[3] == 'R') {
                return get_palette(0x0d, 0x01);
            } else if (title[3] == 'I') {
                return get_palette(0x15, 0x05);
            }
            break;
        case 0x9d:
            return get_palette(0x0d, 0x05);
        case 0x28:
            if (title[3] == 'F') {
                return get_palette(0x0e, 0x03);
            } else if (title[3] == 'A') {
                return get_palette(0x13, 0x00);
            }
            break;
        case 0x4b:
        case 0x90:
        case 0x9a:
        case 0xbd:
            return get_palette(0x0e, 0x03);
        case 0x17:
        case 0x8b:
            return get_palette(0x0e, 0x05);
        case 0x39:
        case 0x43:
        case 0x97:
            return get_palette(0x0f, 0x03);
        case 0x01:
        case 0x10:
        case 0x29:
        case 0x52:
        case 0x5d:
        case 0x68:
        case 0x6d:
        case 0xf6:
            return get_palette(0x0f, 0x05);
        case 0x14:
            return get_palette(0x10, 0x01);
        case 0x70:
            return get_palette(0x11, 0x05);
        case 0x0c:
        case 0x16:
        case 0x35:
        case 0x67:
        case 0x75:
        case 0x92:
        case 0x99:
        case 0xb7:
            return get_palette(0x12, 0x00);
        case 0xa5:
            if (title[3] == 'R') {
                return get_palette(0x12, 0x03);
            } else if (title[3] == 'A') {
                return get_palette(0x13, 0x00);
            }
            break;
        case 0xa2:
        case 0xf7:
            return get_palette(0x12, 0x05);
        case 0xe8:
            return get_palette(0x13, 0x00);
        case 0x58:
            return get_palette(0x16, 0x00);
        case 0x6f:
            return get_palette(0x1b, 0x00);
        case 0xaa:
            return get_palette(0x1c, 0x01);
        case 0x00:
        case 0x3f:
            return get_palette(0x1c, 0x03);
        default:
            break;
    }

    return get_palette(0x1c, 0x03);
}

uint8_t gb_rom_read(gb_s* gb, const uint_fast32_t addr) {
    return rom[addr];
}

uint8_t gb_cart_ram_read(gb_s* gb, const uint_fast32_t addr) {
    return ram[addr];
}

void gb_cart_ram_write(gb_s* gb, const uint_fast32_t addr, const uint8_t val) {
    ram[addr] = val;
}

void gb_error(gb_s* gb, const enum gb_error_e error, const uint16_t addr) {
    switch (error) {
        case GB_UNKNOWN_ERROR:
            printf("Unknown error at address 0x%04X\n", addr);
            break;
        case GB_INVALID_OPCODE:
            printf("Invalid opcode at address 0x%04X\n", addr);
            break;
        case GB_INVALID_READ:
            printf("Invalid read at address 0x%04X\n", addr);
            break;
        case GB_INVALID_WRITE:
            printf("Invalid write at address 0x%04X\n", addr);
            break;
        case GB_HALT_FOREVER:
            printf("Halting forever at address 0x%04X\n", addr);
            break;
        case GB_INVALID_MAX:
            printf("Invalid max at address 0x%04X\n", addr);
            break;
    }

    exit(error);
}

void lcd_draw_line_32bit(gb_s *gb, const uint8_t *pixels, const uint_fast8_t line) {
    auto screenBuffer = reinterpret_cast<uint32_t*>(lfb->getBuffer().add(offsetX * 4 + (offsetY + line * scale) * lfb->getPitch()).get());
    uint16_t resX = LCD_WIDTH * scale;

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            uint8_t pixel = pixels[x / scale];
            auto paletteIndex = (pixel & 0x30) >> 4;
            auto colorIndex = pixel & 0x03;
            auto color = palette[4 * paletteIndex + colorIndex];

            screenBuffer[x] = color;
        }

        screenBuffer += (lfb->getPitch() / 4);
    }
}

void lcd_draw_line_24bit(gb_s *gb, const uint8_t *pixels, const uint_fast8_t line) {
    auto screenBuffer = reinterpret_cast<uint8_t*>(lfb->getBuffer().add(offsetX * 3 + (offsetY + line * scale) * lfb->getPitch()).get());
    uint16_t resX = LCD_WIDTH * scale;

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            uint8_t pixel = pixels[x / scale];
            auto paletteIndex = (pixel & 0x30) >> 4;
            auto colorIndex = pixel & 0x03;
            auto color = palette[4 * paletteIndex + colorIndex];

            screenBuffer[x * 3] = color & 0xff;
            screenBuffer[x * 3 + 1] = (color >> 8) & 0xff;
            screenBuffer[x * 3 + 2] = (color >> 16) & 0xff;
        }

        screenBuffer += (lfb->getPitch());
    }
}

void lcd_draw_line_16bit(gb_s *gb, const uint8_t *pixels, const uint_fast8_t line) {
    auto screenBuffer = reinterpret_cast<uint16_t*>(lfb->getBuffer().add(offsetX * 2 + (offsetY + line * scale) * lfb->getPitch()).get());
    uint16_t resX = LCD_WIDTH * scale;

    for (uint16_t y = 0; y < scale; y++) {
        for (uint16_t x = 0; x < resX; x++) {
            uint8_t pixel = pixels[x / scale];
            auto paletteIndex = (pixel & 0x30) >> 4;
            auto colorIndex = pixel & 0x03;
            auto color = palette[4 * paletteIndex + colorIndex];

            screenBuffer[x] = color;
        }

        screenBuffer += (lfb->getPitch() / 2);
    }
}

void write_ram_to_file(gb_s *gb) {
    auto saveSize = gb_get_save_size(gb);
    if (saveSize == 0) {
        return;
    }

    auto saveFile = Util::Io::File(saveFilePath);
    if (!saveFile.exists()) {
        saveFile.create(Util::Io::File::REGULAR);
    }

    auto saveStream = Util::Io::FileOutputStream(saveFile);
    saveStream.write(ram, 0, saveSize);
}

void read_ram_from_file(gb_s *gb) {
    auto saveSize = gb_get_save_size(gb);
    ram = new uint8_t[saveSize];

    auto saveFile = Util::Io::File(saveFilePath);
    if (saveFile.exists()) {
        auto saveStream = Util::Io::FileInputStream(saveFile);
        saveStream.read(ram, 0, saveSize);
    }
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("GameBoy emulator by 'deltabeard' (https://github.com/deltabeard/Peanut-GB).\n"
                               "Joypad is mapped to WASD; A and B are mapped to K and J; Start is mapped to Space and Select is mapped to Enter.\n"
                               "Use 'F1' and 'F2' to adjust screen scaling. Use 'F3' to cycle through color palettes and 'F4' to reset to default palette.\n"
                               "Usage: peanut-gb [FILE]\n"
                               "Options:\n"
                               "  -s, --save: Path to save file\n"
                               "  -r, --resolution: Set display resolution\n"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("save", false, "s");
    argumentParser.addArgument("resolution", false, "r");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "peanut-gb: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto romFile = Util::Io::File(arguments[0]);
    auto stream = Util::Io::FileInputStream(romFile);

    rom = new uint8_t[romFile.getLength()];
    stream.read(rom, 0, romFile.getLength());

    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        uint32_t resolutionX = Util::String::parseInt(split1[0]);
        uint32_t resolutionY = Util::String::parseInt(split2[0]);
        uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    lfb = new Util::Graphic::LinearFrameBuffer(lfbFile);
    maxScale = lfb->getResolutionX() / LCD_WIDTH > lfb->getResolutionY() / LCD_HEIGHT ? lfb->getResolutionY() / LCD_HEIGHT : lfb->getResolutionX() / LCD_WIDTH;
    scale = maxScale;
    offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ? (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
    offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ? (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;

    gb_s gb{};

    auto initResult = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, nullptr);
    if (initResult != GB_INIT_NO_ERROR) {
        Util::System::error << "peanut-gb: Failed to initialize emulator!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (argumentParser.hasArgument("save")) {
        saveFilePath = argumentParser.getArgument("save");
    } else {
        char title[16]{};
        gb_get_rom_name(&gb, title);

        saveFilePath = romFile.getParent() + "/" + Util::String(title).strip() + ".sav";
    }

    read_ram_from_file(&gb);

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    switch (lfb->getColorDepth()) {
        case 32:
            gb_init_lcd(&gb, &lcd_draw_line_32bit);
            break;
        case 24:
            gb_init_lcd(&gb, &lcd_draw_line_24bit);
            break;
        case 15:
        case 16:
            gb_init_lcd(&gb, &lcd_draw_line_16bit);
            break;
        default:
            Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Unsupported color depth!");
    }

    lfb->clear();

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    auto keyDecoder = Util::Io::KeyDecoder(new Util::Io::DeLayout());
    auto stringDrawer = Util::Graphic::StringDrawer(Util::Graphic::PixelDrawer(*lfb));

    uint8_t manualPaletteIndex = UINT8_MAX;
    palette = gb_get_palette(&gb);

    auto timer = time(nullptr);
    auto date = localtime(&timer);
    gb_set_rtc(&gb, date);

    while (true) {
        auto startTime = Util::Time::getSystemTime();

        auto c = Util::System::in.read();
        if (c != -1 && keyDecoder.parseScancode(c)) {
            auto key = keyDecoder.getCurrentKey();
            uint8_t joyKey = 0;

            switch (key.getScancode()) {
                case Util::Io::Key::UP:
                    joyKey = JOYPAD_UP;
                    break;
                case Util::Io::Key::LEFT:
                    joyKey = JOYPAD_LEFT;
                    break;
                case Util::Io::Key::DOWN:
                    joyKey = JOYPAD_DOWN;
                    break;
                case Util::Io::Key::RIGHT:
                    joyKey = JOYPAD_RIGHT;
                    break;
                case Util::Io::Key::K:
                    joyKey = JOYPAD_A;
                    break;
                case Util::Io::Key::J:
                    joyKey = JOYPAD_B;
                    break;
                case Util::Io::Key::SPACE:
                    joyKey = JOYPAD_START;
                    break;
                case Util::Io::Key::ENTER:
                    joyKey = JOYPAD_SELECT;
                    break;
                case Util::Io::Key::F1:
                    if (key.isPressed() && scale < maxScale) {
                        scale++;
                        offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ? (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
                        offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ? (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;
                        lfb->clear();
                    }
                    break;
                case Util::Io::Key::F2:
                    if (key.isPressed() && scale > 1) {
                        scale--;
                        offsetX = lfb->getResolutionX() - LCD_WIDTH * scale > 0 ? (lfb->getResolutionX() - LCD_WIDTH * scale) / 2 : 0;
                        offsetY = lfb->getResolutionY() - LCD_HEIGHT * scale > 0 ? (lfb->getResolutionY() - LCD_HEIGHT * scale) / 2 : 0;
                        lfb->clear();
                    }
                    break;
                case Util::Io::Key::F3:
                    if (key.isPressed()) {
                        manualPaletteIndex = manualPaletteIndex + 1 > 14 ? 0 : manualPaletteIndex + 1;

                        delete palette;
                        palette = get_manual_palette(manualPaletteIndex);
                    }
                    break;
                case Util::Io::Key::F4:
                    if (key.isPressed() && manualPaletteIndex != UINT8_MAX) {
                        manualPaletteIndex = UINT8_MAX;

                        delete palette;
                        palette = gb_get_palette(&gb);
                    }
                    break;
                case Util::Io::Key::ESC:
                    write_ram_to_file(&gb);
                    return 0;
                default:
                    break;
            }

            if (key.isPressed()) {
                gb.direct.joypad &= ~joyKey;
            } else {
                gb.direct.joypad |= joyKey;
            }
        }

        gb_run_frame(&gb);
        stringDrawer.drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

        fpsCounter++;
        auto frameTime = Util::Time::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer.reset();
        }
    }
}