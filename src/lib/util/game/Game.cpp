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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Game.h"

#include "lib/util/game/Scene.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/game/Graphics.h"

namespace Util::Game {

Game::~Game() {
    for (const auto *scene : scenes) {
        delete scene;
    }

    scenes.clear();
}

bool Game::isRunning() const {
    return running;
}

void Game::stop() {
    running = false;
}

Scene &Game::getCurrentScene() {
    return *scenes.peek();
}

void Game::pushScene(Scene *scene) {
    scenes.add(scene);
}

void Game::switchToNextScene() {
    sceneSwitched = true;
}

void Game::initializeNextScene(Graphics &graphics) {
    if (!firstScene) {
        delete scenes.poll();
    }

    auto &scene = *scenes.peek();
    graphics.clearBackground();
    scene.initialize(graphics);

    firstScene = false;
    sceneSwitched = false;
}

}