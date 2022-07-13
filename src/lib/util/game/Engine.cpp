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

#include "Engine.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/system/System.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Fonts.h"
#include "lib/util/graphic/Colors.h"

namespace Util::Game {

Engine::Engine(Game &game, const Util::Graphic::LinearFrameBuffer &lfb, const uint8_t targetFrameRate) :
        game(game), graphics(lfb), targetFrameRate(targetFrameRate) {}

void Engine::run() {
    const auto delta = 1.0 / targetFrameRate;
    const auto deltaMilliseconds = static_cast<uint32_t>(delta * 1000);
    uint32_t currentTime = Time::getSystemTime().toMilliseconds();
    uint32_t accumulated = 0;

    while (game.isRunning()) {
        uint32_t newTime = Time::getSystemTime().toMilliseconds();
        frameTime = newTime - currentTime;
        if (frameTime == 0) {
            frameTime = 1;
        } else if (frameTime > 250) {
            frameTime = 250;
        }

        currentTime = newTime;
        accumulated += frameTime;

        while (accumulated >= deltaMilliseconds) {
            game.update(delta);
            game.applyChanges();
            accumulated -= deltaMilliseconds;
        }

        game.draw(graphics);
        drawStatus();
        graphics.show();
    }
}

void Engine::drawStatus() {
    auto status = Memory::String::format("Objects: %u, Frame Time: %ums", game.getObjectCount(), frameTime);
    auto color = graphics.getColor();
    graphics.setColor(Util::Graphic::Colors::WHITE);
    graphics.drawStringSmall(-1, 1, status);
    graphics.setColor(color);
}

}