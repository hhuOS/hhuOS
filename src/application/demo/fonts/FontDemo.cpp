/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include <stdint.h>

#include "FontDemo.h"

#include <util/base/String.h>
#include <util/base/System.h>
#include <util/graphic/Colors.h>
#include <util/graphic/font/Mini4x6.h>
#include <util/graphic/font/Acorn8x8.h>
#include <util/graphic/font/Pearl8x8.h>
#include <util/graphic/font/Terminal8x8.h>
#include <util/graphic/font/Terminal8x16.h>
#include <util/graphic/font/Sun8x16.h>
#include <util/graphic/font/Sun12x22.h>
#include <util/graphic/Ansi.h>
#include <util/graphic/Font.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/io/stream/InputStream.h>

const char *string = "The quick brown fox jumps over the lazy dog 1234567890";

/// Print a test string using the given font.
/// The parameter `y` indicates the position to which the string is printed on the screen (x is always 0).
uint16_t testFont(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Font &font, const char *fontName,
    const uint16_t y)
{
    lfb.drawString(font, 0, y, static_cast<const char*>(Util::String::format("%s:", fontName)),
        Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
    lfb.drawString(font, 0, y + font.getCharHeight(), string,
        Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);

    return 2 * font.getCharHeight();
}

void fontDemo(const Util::Graphic::LinearFrameBuffer &lfb) {
    uint16_t yPosition = 0;

    yPosition += testFont(lfb, Util::Graphic::Fonts::MINI_4x6, "Mini 4x6", yPosition) + 16;
    yPosition += testFont(lfb, Util::Graphic::Fonts::ACORN_8x8, "Acorn 8x8", yPosition) + 16;
    yPosition += testFont(lfb, Util::Graphic::Fonts::PEARL_8x8, "Pearl 8x8", yPosition) + 16;
    yPosition += testFont(lfb, Util::Graphic::Fonts::TERMINAL_8x8, "Terminal 8x8", yPosition) + 16;
    yPosition += testFont(lfb, Util::Graphic::Fonts::TERMINAL_8x16, "Terminal 8x16", yPosition) + 16;
    yPosition += testFont(lfb, Util::Graphic::Fonts::SUN_8x16, "Sun 8x16", yPosition) + 16;
    testFont(lfb, Util::Graphic::Fonts::SUN_12x22, "Sun 12x22", yPosition);

    const Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);

    // Wait for Escape key to be pressed
    while (true) {
        if (keyDecoder.parseScancode(Util::System::in.read())) {
            if (keyDecoder.getKeyEvent().getScancode() == Util::Io::KeyEvent::ESC) {
                break;
            }
        }
    }
}