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

void textLoop() {
    auto inputStream = Util::Stream::FileInputStream("/device/mouse");

    while (true) {
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
    auto lfb = Util::Graphic::LinearFrameBuffer(Util::File::File("/device/lfb"));
    auto graphics = Util::Game::Graphics2D(lfb);
    auto inputStream = Util::Stream::FileInputStream("/device/mouse");
    double x = 0;
    double y = 0;

    graphics.drawString(x, y, "@");
    graphics.show();

    while (true) {
        auto buttons = inputStream.read();
        auto xMovement = static_cast<int8_t>(inputStream.read());
        auto yMovement = static_cast<int8_t>(inputStream.read());

        x += static_cast<double>(xMovement) / INT8_MAX;
        y += static_cast<double>(yMovement) / INT8_MAX;

        if (x > 1) x = 1;
        if (y > 1) y = 1;
        if (x < -1) x = -1;
        if (y < -1) y = -1;

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

        graphics.drawString(x, y, cursor.isEmpty() ? "@" : cursor);
        graphics.show();
    }
}

int32_t main(int32_t argc, char *argv[]) {
    auto mode = Util::Memory::String(argc > 1 ? argv[1] : "text");
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