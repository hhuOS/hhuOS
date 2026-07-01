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

#include "AntDemo.h"

#include "lib/util/base/System.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/InputStream.h"

void antDemo(const Util::Graphic::LinearFrameBuffer &lfb) {
    Ant ant(lfb.getResolutionX(), lfb.getResolutionY());

    // Each pixel is drawn to the lfb, as well as to this double buffer.
    // This is done because reading from the hardware framebuffer is expensive,
    // but reading from the double buffer is fast.
    const Util::Graphic::BufferedLinearFrameBuffer bufferedLfb(lfb);

    const Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    bufferedLfb.clear();
    bufferedLfb.flush();
    
    while (true) {
        // Exit application if ESC is pressed
        if (Util::System::in.isReadyToRead()) {
            if (keyDecoder.parseScancode(Util::System::in.read())) {
                if (keyDecoder.getKeyEvent().getScancode() == Util::Io::KeyEvent::ESC) {
                    break;
                }
            }
        }

        // Read the color of the pixel at the ant's current position.
        const auto pixel = bufferedLfb.readPixel(ant.getX(), ant.getY());

        if (pixel == Util::Graphic::Colors::BLACK) {
            // The pixel is black -> Color it in and turn the ant clockwise
            lfb.drawPixel(ant.getX(), ant.getY(), ant.getColor());
            bufferedLfb.drawPixel(ant.getX(), ant.getY(), ant.getColor());
            ant.turnClockwise();
        } else {
            // The pixel is colored -> Set it to black and turn the ant counter-clockwise
            lfb.drawPixel(ant.getX(), ant.getY(), Util::Graphic::Colors::BLACK);
            bufferedLfb.drawPixel(ant.getX(), ant.getY(), Util::Graphic::Colors::BLACK);
            ant.turnCounterClockwise();
        }

        ant.move();
    }
}

Ant::Ant(const uint16_t limitX, const uint16_t limitY) : limitX(limitX), limitY(limitY),
    x(random.getRandomNumber(0, limitX)), y(random.getRandomNumber(0, limitY)) {}

void Ant::move() {
    bool crossedBorder = false;

    switch (direction) {
        case UP:
            if (y == 0) {
                y = limitY - 1;
                crossedBorder = true;
            } else {
                y -= 1;
            }
            break;
        case RIGHT:
            if (x >= limitX - 1) {
                x = 0;
                crossedBorder = true;
            } else {
                x += 1;
            }
            break;
        case DOWN:
            if (y >= limitY - 1) {
                y = 0;
                crossedBorder = true;
            } else {
                y += 1;
            }
            break;
        case LEFT:
            if (x == 0) {
                x = limitX - 1;
                crossedBorder = true;
            } else {
                x -= 1;
            }
            break;
    }

    if (crossedBorder) {
        color = Util::Graphic::Color(random.getRandomNumber(1, 255),
            random.getRandomNumber(1, 255), random.getRandomNumber(1, 255));
    }
}

void Ant::turnClockwise() {
    direction = static_cast<Direction>((direction + 90) % 360);
}

void Ant::turnCounterClockwise() {
    direction = direction == 0 ? LEFT : static_cast<Direction>(direction - 90);
}