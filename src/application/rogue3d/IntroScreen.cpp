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
 *
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include <stdint.h>

#include "IntroScreen.h"
#include "Rogue3D.h"
#include "lib/util/game/Game.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"

void IntroScreen::initialize() {

}

void IntroScreen::update([[maybe_unused]] double delta) {}

void IntroScreen::initializeBackground(Util::Game::Graphics &graphics) {
    auto lines = sizeof(INTRO_TEXT) / sizeof(char*);
    auto centerX = graphics.getAbsoluteResolutionX() / 2;
    auto centerY = graphics.getAbsoluteResolutionY() / 2;
    auto y = static_cast<uint16_t>(centerY - ((lines * Util::Game::Graphics::FONT_SIZE) / 2.0));

    graphics.clear();
    graphics.setColor(Util::Graphic::Colors::GREEN);
    for (uint32_t i = 0; i < lines; i++) {
        auto x = static_cast<uint16_t>(centerX - (Util::Address(INTRO_TEXT[i]).stringLength() * Util::Game::Graphics::FONT_SIZE) / 2.0);
        graphics.drawStringDirectAbsolute(x, y + i * Util::Game::Graphics::FONT_SIZE, INTRO_TEXT[i]);
    }
}

void IntroScreen::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::Game::getInstance().stop();
            break;
        case Util::Io::Key::SPACE:
            auto &game = Util::Game::Game::getInstance();
            game.pushScene(new Rogue3D());
            game.switchToNextScene();
    }
}

void IntroScreen::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}
