/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Fonts.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/async/Thread.h"
#include "SchedulerSign.h"


void SchedulerSign::run() {
    auto lfbFile = Util::File::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);

    auto &font = Util::Graphic::Fonts::TERMINAL_FONT;
    const char *characters = "|/-\\";

    uint32_t characterIndex = 0;
    while (true) {
        stringDrawer.drawChar(font, lfb.getResolutionX() - font.getCharWidth(), 0, characters[characterIndex++ % 4], Util::Graphic::Colors::RED, Util::Graphic::Colors::BLACK);
        Util::Async::Thread::sleep({0, 250000000});
    }
}
