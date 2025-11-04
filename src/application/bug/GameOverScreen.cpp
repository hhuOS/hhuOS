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
 */

#include "GameOverScreen.h"

#include <stdint.h>

#include "BugDefender.h"
#include "lib/util/pulsar/Game.h"
#include "lib/util/base/Address.h"
#include "lib/util/pulsar/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"

GameOverScreen::GameOverScreen(bool won) : won(won) {}

void GameOverScreen::initialize() {

}

void GameOverScreen::update([[maybe_unused]] double delta) {}

void GameOverScreen::initializeBackground(Util::Pulsar::Graphics &graphics) {
    auto **text = won ? WIN_TEXT : LOOSE_TEXT;
    auto lines = sizeof(won ? WIN_TEXT : LOOSE_TEXT) / sizeof(char*);
    auto centerX = graphics.getAbsoluteResolutionX() / 2;
    auto centerY = graphics.getAbsoluteResolutionY() / 2;
    auto y = static_cast<uint16_t>(centerY - ((lines * Util::Pulsar::Graphics::FONT_SIZE) / 2.0));

    graphics.clear();
    graphics.setColor(Util::Graphic::Colors::GREEN);
    for (uint32_t i = 0; i < lines; i++) {
        auto x = static_cast<uint16_t>(centerX - (Util::Address(text[i]).stringLength() * Util::Pulsar::Graphics::FONT_SIZE) / 2.0);
        graphics.drawStringDirectAbsolute(x, y + i * Util::Pulsar::Graphics::FONT_SIZE, text[i]);
    }
}

void GameOverScreen::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Pulsar::Game::getInstance().stop();
            break;
        case Util::Io::Key::SPACE:
            auto &game = Util::Pulsar::Game::getInstance();
            game.pushScene(new BugDefender());
            game.switchToNextScene();
    }
}

void GameOverScreen::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}
