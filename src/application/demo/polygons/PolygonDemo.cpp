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
 */

#include "PolygonDemo.h"
#include "DemoPolygon.h"
#include "DemoPolygonFactory.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game
}  // namespace Util

PolygonDemo::PolygonDemo(uint32_t initialCount) : initialCount(initialCount) {}

void PolygonDemo::initialize() {
    for (uint32_t i = 0; i < initialCount; i++) {
        auto *polygon = factory.createPolygon();
        polygons.offer(polygon);
        addObject(polygon);
    }

    setKeyListener(*this);
}

void PolygonDemo::update([[maybe_unused]] double delta) {}

void PolygonDemo::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::PLUS: {
            auto *polygon = factory.createPolygon();
            polygons.offer(polygon);
            addObject(polygon);
            break;
        }
        case Util::Io::Key::MINUS: {
            if (polygons.size() > 0) {
                removeObject(polygons.poll());
            }
            break;
        }
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        default:
            break;
    }
}

void PolygonDemo::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}

void PolygonDemo::initializeBackground([[maybe_unused]] Util::Game::Graphics &graphics) {}
