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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Missile.h"

#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/math/Vector3.h"
#include "application/battlespace/Player.h"
#include "lib/util/game/3d/Entity.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/base/String.h"

Missile::Missile(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &direction, Player &player) :
    Util::Game::D3::Model(TAG, "/user/battlespace/missile.obj", position, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0.2, 0.2, 0.2), Util::Graphic::Colors::RED), player(&player), direction(direction) {}

Missile::Missile(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &direction) :
    Util::Game::D3::Model(TAG, "/user/battlespace/missile.obj", position, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0.2, 0.2, 0.2), Util::Graphic::Colors::GREEN), direction(direction) {}

void Missile::onUpdate(double delta) {
    if (lifetime > 5) {
        Util::Game::GameManager::getCurrentScene().removeObject(this);
    } else {
        lifetime += delta;

        auto speed = lifetime < START_SPEED_TIME ? START_SPEED : FULL_SPEED;
        translate(direction * speed * delta * 60);
    }
}

void Missile::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Missile::TAG && player != nullptr) {
        player->addScore(250);
    }

    Util::Game::GameManager::getCurrentScene().removeObject(this);
}
