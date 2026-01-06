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
 *
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Missile.h"

#include "lib/pulsar/Scene.h"
#include "lib/pulsar/3d/event/CollisionEvent.h"
#include "lib/util/math/Vector3.h"
#include "application/battlespace/Player.h"
#include "lib/pulsar/3d/Entity.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/base/String.h"

Missile::Missile(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &direction, Player &player) :
    Model(TAG, "/user/battlespace/missile.obj", position, Util::Math::Vector3<float>(0, 0, 0), Util::Math::Vector3<float>(0.2, 0.2, 0.2), Util::Graphic::Colors::RED), player(&player) {
    setFrontVector(direction);
}

Missile::Missile(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &direction) :
    Model(TAG, "/user/battlespace/missile.obj", position, Util::Math::Vector3<float>(0, 0, 0), Util::Math::Vector3<float>(0.2, 0.2, 0.2), Util::Graphic::Colors::GREEN) {
    setFrontVector(direction);
}

void Missile::onUpdate(float delta) {
    if (lifetime > 5) {
        removeFromScene();
    } else {
        lifetime += delta;

        auto speed = lifetime < START_SPEED_TIME ? START_SPEED : FULL_SPEED;
        translate(getFrontVector() * speed * delta * 60);
    }
}

void Missile::onCollisionEvent(const Pulsar::D3::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == TAG && player != nullptr) {
        player->addScore(250);
    }

    removeFromScene();
}
