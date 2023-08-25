/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "IntroScreen.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"

IntroScreen::IntroScreen() {
    setKeyListener(*this);
}

void IntroScreen::initializeBackground(Util::Game::Graphics &graphics) {
    auto &resolution = Util::Game::GameManager::getAbsoluteResolution();
    auto lines = sizeof(INTRO_TEXT) / sizeof(char*);
    auto centerX = resolution.getX() / 2;
    auto centerY = resolution.getY() / 2;
    auto y = static_cast<uint16_t>(centerY - ((lines * graphics.getCharHeight()) / 2.0));

    graphics.setColor(Util::Graphic::Colors::GREEN);
    for (uint32_t i = 0; i < lines; i++) {
        auto x = static_cast<uint16_t>(centerX - (Util::Address<uint32_t>(INTRO_TEXT[i]).stringLength() * graphics.getCharWidth()) / 2.0);
        graphics.drawString(Util::Math::Vector2D(x, y + i * graphics.getCharHeight()), INTRO_TEXT[i]);
    }
}

void IntroScreen::update(double delta) {}

void IntroScreen::keyPressed(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        default:
            Util::Game::GameManager::getGame().switchToNextScene();
            break;
    }
}

void IntroScreen::keyReleased(Util::Io::Key key) {

}
