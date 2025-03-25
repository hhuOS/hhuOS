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
 */

#include "GameOverScreen.h"

#include <stdint.h>

#include "BugDefender.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"

GameOverScreen::GameOverScreen(bool won) : won(won) {}

void GameOverScreen::initialize() {
    setKeyListener(*this);
}

void GameOverScreen::update([[maybe_unused]] double delta) {}

void GameOverScreen::initializeBackground(Util::Game::Graphics &graphics) {
    auto **text = won ? WIN_TEXT : LOOSE_TEXT;
    auto lines = sizeof(won ? WIN_TEXT : LOOSE_TEXT) / sizeof(char*);
    auto centerX = graphics.getAbsoluteResolutionX() / 2;
    auto centerY = graphics.getAbsoluteResolutionY() / 2;
    auto y = static_cast<uint16_t>(centerY - ((lines * Util::Game::Graphics::FONT_SIZE) / 2.0));

    graphics.clear();
    graphics.setColor(Util::Graphic::Colors::GREEN);
    for (uint32_t i = 0; i < lines; i++) {
        auto x = static_cast<uint16_t>(centerX - (Util::Address(text[i]).stringLength() * Util::Game::Graphics::FONT_SIZE) / 2.0);
        graphics.drawStringDirectAbsolute(x, y + i * Util::Game::Graphics::FONT_SIZE, text[i]);
    }
}

void GameOverScreen::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::SPACE:
            auto &game = Util::Game::GameManager::getGame();
            game.pushScene(new BugDefender());
            game.switchToNextScene();
    }
}

void GameOverScreen::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}
