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

#include <cstdint>
#include "lib/util/system/System.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/graphic/Fonts.h"
#include "lib/util/async/FunctionPointerRunnable.h"
#include "lib/util/async/Thread.h"

bool isRunning = true;

void textLoop() {
    auto inputStream = Util::Stream::FileInputStream("/device/mouse");

    Util::Async::Thread::createThread("Exit-Listener", new Util::Async::FunctionPointerRunnable([]{
        Util::System::in.read();
        isRunning = false;
    }));

    while (isRunning) {
        auto buttons = inputStream.read();
        auto xMovement = static_cast<int8_t>(inputStream.read());
        auto yMovement = static_cast<int8_t>(inputStream.read());

        Util::System::out << "Left: " << (buttons & 0x01) << ", "
                          << "Right: " << ((buttons & 0x02) >> 1) << ", "
                          << "Middle: " << ((buttons & 0x04) >> 2) << ", "
                          << "x: " << xMovement << ", "
                          << "y: " << yMovement << ", " << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    }
}

void graphicsLoop() {
    auto lfbFile = Util::File::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto stringDrawer = Util::Graphic::StringDrawer(Util::Graphic::PixelDrawer(lfb));
    auto inputStream = Util::Stream::FileInputStream("/device/mouse");
    int32_t x = lfb.getResolutionX() / 2;
    int32_t y = lfb.getResolutionY() / 2;

    lfb.clear();
    stringDrawer.drawString(Util::Graphic::Fonts::TERMINAL_FONT, x, y, "@", Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);

    Util::Async::Thread::createThread("Exit-Listener", new Util::Async::FunctionPointerRunnable([]{
        Util::System::in.read();
        isRunning = false;
    }));

    while (isRunning) {
        auto buttons = inputStream.read();
        auto xMovement = static_cast<int8_t>(inputStream.read());
        auto yMovement = static_cast<int8_t>(inputStream.read());

        stringDrawer.drawString(Util::Graphic::Fonts::TERMINAL_FONT, x, y, "   ", Util::Graphic::Colors::WHITE, Util::Graphic::Colors::BLACK);

        x += xMovement;
        y += yMovement;

        if (x >= lfb.getResolutionX()) x = lfb.getResolutionX() - 1;
        if (y >= lfb.getResolutionY()) y = lfb.getResolutionY() - 1;
        if (x <= 0) x = 0;
        if (y <= 0) y = 0;

        Util::Memory::String cursor;
        if (buttons & 0x01) {
            cursor += 'l';
        }
        if (buttons & 0x04) {
            cursor += 'm';
        }
        if (buttons & 0x02) {
            cursor += 'r';
        }

        stringDrawer.drawString(Util::Graphic::Fonts::TERMINAL_FONT, x, y, static_cast<const char*>(cursor.isEmpty() ? "@" : cursor), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
    }

    lfb.clear();
}

int32_t main(int32_t argc, char *argv[]) {
    auto mode = Util::Memory::String(argc > 1 ? argv[1] : "graphics");
    if (mode == "text") {
        textLoop();
    } else if (mode == "graphics") {
        graphicsLoop();
    } else {
        Util::System::error << "Invalid argument! Please specify the mode (text/graphics)" << Util::Stream::PrintWriter::endl;
        return -1;
    }

    return 0;
}