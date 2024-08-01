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

#include "FontDemo.h"

#include <stdint.h>

#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/base/System.h"
#include "lib/util/graphic/font/Mini4x6.h"
#include "lib/util/graphic/font/Acorn8x8.h"
#include "lib/util/graphic/font/Pearl8x8.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/font/Terminal8x16.h"
#include "lib/util/graphic/font/Sun8x16.h"
#include "lib/util/graphic/font/Sun12x22.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/stream/InputStream.h"

const char *string = "The quick brown fox jumps over the lazy dog 1234567890";

uint16_t testFont(const Util::Graphic::StringDrawer &stringDrawer, const Util::Graphic::Font &font, const char *fontName, uint16_t y) {
    stringDrawer.drawString(font, 0, y, static_cast<const char*>(Util::String::format("%s:", fontName)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
    stringDrawer.drawString(font, 0, y + font.getCharHeight(), string, Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);

    return y + 2 * font.getCharHeight();
}

void fontDemo(Util::Graphic::LinearFrameBuffer &lfb) {
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);
    uint16_t yPosition = 0;

    Util::Graphic::Ansi::prepareGraphicalApplication(false);
    lfb.clear();

    yPosition = testFont(stringDrawer, Util::Graphic::Fonts::MINI_4x6, "Mini 4x6", yPosition) + 16;
    yPosition = testFont(stringDrawer, Util::Graphic::Fonts::ACORN_8x8, "Acorn 8x8", yPosition) + 16;
    yPosition = testFont(stringDrawer, Util::Graphic::Fonts::PEARL_8x8, "Pearl 8x8", yPosition) + 16;
    yPosition = testFont(stringDrawer, Util::Graphic::Fonts::TERMINAL_8x8, "Terminal 8x8", yPosition) + 16;
    yPosition = testFont(stringDrawer, Util::Graphic::Fonts::TERMINAL_8x16, "Terminal 8x16", yPosition) + 16;
    yPosition = testFont(stringDrawer, Util::Graphic::Fonts::SUN_8x16, "Sun 8x16", yPosition) + 16;
    testFont(stringDrawer, Util::Graphic::Fonts::SUN_12x22, "Sun 12x22", yPosition);

    Util::System::in.read();
    Util::Graphic::Ansi::cleanupGraphicalApplication();
}