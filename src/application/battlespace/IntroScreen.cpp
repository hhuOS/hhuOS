/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "IntroScreen.h"

#include <stdint.h>

#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "BattleSpaceGame.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/graphic/Font.h"

void IntroScreen::initialize() {
    setKeyListener(*this);
}

void IntroScreen::initializeBackground(Util::Game::Graphics &graphics) {
    auto &font = Util::Graphic::Font::getFontForResolution(static_cast<uint32_t>(Util::Game::GameManager::getAbsoluteResolution().getY()));
    auto &resolution = Util::Game::GameManager::getAbsoluteResolution();
    auto lines = sizeof(INTRO_TEXT) / sizeof(char*);
    auto centerX = resolution.getX() / 2;
    auto centerY = resolution.getY() / 2;
    auto y = static_cast<uint16_t>(centerY - ((lines * font.getCharHeight()) / 2.0));

    graphics.clear();
    graphics.setColor(Util::Graphic::Colors::GREEN);
    for (uint32_t i = 0; i < lines; i++) {
        auto x = static_cast<uint16_t>(centerX - (Util::Address<uint32_t>(INTRO_TEXT[i]).stringLength() * font.getCharWidth()) / 2.0);
        graphics.drawString(font, Util::Math::Vector2D(x, y + i * font.getCharHeight()), INTRO_TEXT[i]);
    }
}

void IntroScreen::update([[maybe_unused]] double delta) {}

void IntroScreen::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::SPACE:
            Util::Game::GameManager::getGame().pushScene(new BattleSpaceGame());
            Util::Game::GameManager::getGame().switchToNextScene();
            break;
    }
}

void IntroScreen::keyReleased([[maybe_unused]] const Util::Io::Key &key) {

}
