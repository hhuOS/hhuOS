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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "ParticleDemo.h"

#include <stdint.h>

#include "lib/pulsar/Game.h"
#include "RainEmitter.h"
#include "Ground.h"
#include "lib/pulsar/Graphics.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/math/Vector2.h"
#include "Dino.h"

void ParticleDemo::initialize() {
    cloud = new RainEmitter(Util::Math::Vector2<double>(0, 0.8));
    addEntity(cloud);

    for (uint32_t i = 0; i < 3; i++) {
        auto *newGround = new Ground(Util::Math::Vector2<double>(-1.5 + i, -1));
        addEntity(newGround);
    }

    addEntity(new Dino(Util::Math::Vector2<double>(-0.7, -1 + Ground::HEIGHT), false));
    addEntity(new Dino(Util::Math::Vector2<double>(0.5, -1 + Ground::HEIGHT), true));
}

void ParticleDemo::update([[maybe_unused]] double delta) {}

bool ParticleDemo::initializeBackground(Pulsar::Graphics &graphics) {
    graphics.clear(Util::Graphic::Color(153, 221, 255));
    return true;
}

void ParticleDemo::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Pulsar::Game::getInstance().stop();
            break;
        default:
            break;
    }
}

void ParticleDemo::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}
