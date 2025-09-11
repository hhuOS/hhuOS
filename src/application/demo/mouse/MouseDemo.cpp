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

#include "MouseDemo.h"

#include "MouseCursor.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "application/demo/mouse/Logo.h"

void MouseDemo::update([[maybe_unused]] double delta) {}

void MouseDemo::initialize() {
    addObject(logo);
    addObject(cursor);
    setKeyListener(*this);
    setMouseListener(*cursor);
}

void MouseDemo::keyPressed(const Util::Io::Key &key) {
    if (key.getScancode() == Util::Io::Key::ESC) {
        Util::Game::GameManager::getGame().stop();
    }
}

void MouseDemo::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}

void MouseDemo::initializeBackground(Util::Game::Graphics &graphics) {
    graphics.clear(Util::Graphic::Colors::HHU_BLUE.withSaturation(35));
}
