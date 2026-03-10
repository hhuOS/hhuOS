/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "PolygonDemo.h"

#include "DemoPolygon.h"
#include "DemoPolygonFactory.h"
#include "lib/util/io/key/KeyEvent.h"
#include "lib/pulsar/Game.h"

PolygonDemo::PolygonDemo(uint32_t initialCount) : initialCount(initialCount) {}

void PolygonDemo::initialize() {
    for (uint32_t i = 0; i < initialCount; i++) {
        auto *polygon = factory.createPolygon();
        polygons.offer(polygon);
        addEntity(polygon);
    }


}

void PolygonDemo::update([[maybe_unused]] float delta) {}

void PolygonDemo::keyPressed(const Util::Io::KeyEvent &key) {
    switch (key.getScancode()) {
        case Util::Io::KeyEvent::PLUS: {
            auto *polygon = factory.createPolygon();
            polygons.offer(polygon);
            addEntity(polygon);
            break;
        }
        case Util::Io::KeyEvent::MINUS: {
            if (polygons.size() > 0) {
                removeEntity(polygons.poll());
            }
            break;
        }
        case Util::Io::KeyEvent::ESC:
            Pulsar::Game::getInstance().stop();
            break;
        default:
            break;
    }
}

void PolygonDemo::keyReleased([[maybe_unused]] const Util::Io::KeyEvent &key) {}
