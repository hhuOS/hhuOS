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

#include "Missile.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"

Missile::Missile(const Util::Math::Vector3D &translation, const Util::Math::Vector3D &rotation, const Util::Math::Vector3D &scale, const Util::Graphic::Color &color) : Util::Game::D3::ModelEntity(TAG, "/initrd/battlespace/missile.obj", translation, rotation, scale, color) {}

void Missile::onUpdate(double delta) {
    if (lifetime > 5) {
        Util::Game::GameManager::getCurrentScene().removeObject(this);
    } else {
        lifetime += delta;

        auto translation = lifetime < 0.5 ? Util::Math::Vector3D(0, 0, 0.04) : Util::Math::Vector3D(0, 0, 0.2);
        translateLocal(translation * delta * 60);
    }
}

void Missile::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
    Util::Game::GameManager::getCurrentScene().removeObject(this);
}
