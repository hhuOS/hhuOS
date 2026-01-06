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

#include "Ant.h"

#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/async/Thread.h"
#include "lib/util/base/System.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/time/Timestamp.h"

Ant::Ant(int16_t limitX, int16_t limitY) : limitX(limitX), limitY(limitY), x(static_cast<int16_t>(Util::Math::Random().getRandomNumber() * limitX)), y(static_cast<int16_t>(Util::Math::Random().getRandomNumber() * limitY)) {}

void Ant::move() {
    switch (direction) {
        case UP:
            y--;
            break;
        case RIGHT:
            x++;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
    }

    bool crossedBorder = false;

    if (x < 0) {
        x = static_cast<int16_t>(limitX - 1);
        crossedBorder = true;
    }
    if (y < 0) {
        y = static_cast<int16_t>(limitY - 1);
        crossedBorder = true;
    }
    if (x >= limitX) {
        x = 0;
        crossedBorder = true;
    }
    if (y >= limitY) {
        y = 0;
        crossedBorder = true;
    }

    if (crossedBorder) {
        color = Util::Graphic::Color(static_cast<uint8_t>(random.getRandomNumber() * 256), static_cast<uint8_t>(random.getRandomNumber() * 256), static_cast<uint8_t>(random.getRandomNumber() * 256));
    }
}

void Ant::turnClockWise() {
    direction = static_cast<Direction>((direction + 90) % 360);
}

void Ant::turnCounterClockWise() {
    direction = direction == 0 ? LEFT : static_cast<Direction>(direction - 90);
}

int16_t Ant::getX() const {
    return x;
}

int16_t Ant::getY() const {
    return y;
}

const Util::Graphic::Color &Ant::getColor() const {
    return color;
}

void antDemo(const Util::Graphic::LinearFrameBuffer &lfb, uint32_t sleepInterval) {
    Util::Graphic::Ansi::prepareGraphicalApplication(false);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    Ant ant(static_cast<int16_t>(lfb.getResolutionX()), static_cast<int16_t>(lfb.getResolutionY()));
    lfb.clear();

    for (int i = 0;; i++) {
        if (Util::System::in.read() > 0) {
            break;
        }

        auto pixel = lfb.readPixel(ant.getX(), ant.getY());
        if (pixel == Util::Graphic::Colors::BLACK) {
            lfb.drawPixel(ant.getX(), ant.getY(), ant.getColor());
            ant.turnClockWise();
        } else {
            lfb.drawPixel(ant.getX(), ant.getY(), Util::Graphic::Colors::BLACK);
            ant.turnCounterClockWise();
        }

        ant.move();

        if (sleepInterval != 0 && i % sleepInterval == 0) {
            Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        }
    }

    Util::Graphic::Ansi::cleanupGraphicalApplication();
}