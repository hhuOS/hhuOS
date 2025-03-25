/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "IntroScreen.h"

#include <stdint.h>

#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"
#include "Level.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"

void IntroScreen::initialize() {
    setKeyListener(*this);
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
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::SPACE:
            auto &game = Util::Game::GameManager::getGame();
            game.pushScene(new Level(Util::Io::File("/user/dino/level/level1.txt"), 0));
            game.switchToNextScene();
    }
}

void IntroScreen::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}
