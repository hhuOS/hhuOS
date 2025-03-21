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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "lib/util/math/Math.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/3d/event/CollisionEvent.h"
#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"

Projectile::Projectile(const Util::Math::Vector3<double> &position,const Util::Math::Vector3<double> &direction, uint32_t tag) : Entity(tag, position, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1), Util::Game::D3::SphereCollider(position, 0.25)), direction(direction), origin(position) {}

Projectile::Projectile(const Util::Math::Vector3<double> &position,const Util::Math::Vector3<double> &direction, uint32_t tag, double range) : Util::Game::D3::Entity(tag, position, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1), Util::Game::D3::SphereCollider(position, 0.25)), range(range), direction(direction), origin(position) {}

void Projectile::initialize() {}

void Projectile::onUpdate(double delta) {
    if (range < Util::Math::absolute(origin.getX() - getPosition().getX()) || range < Util::Math::absolute(origin.getZ() - getPosition().getZ())) {
        Util::Game::GameManager::getCurrentScene().removeObject(this);
    }

    setPosition(getPosition() + (direction * delta * 12));
}

void Projectile::draw(Util::Game::Graphics &graphics) {
    graphics.setColor(getTag() == TAG_PLAYER ? Util::Graphic::Color(68, 195, 212) : Util::Graphic::Color(255, 0, 0));
    graphics.drawCuboid3D(getPosition(), Util::Math::Vector3<double>(0.5, 0.5, 0.5), getRotation());
}

void Projectile::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
    const auto otherTag = event.getCollidedWidth().getTag();
    if ((getTag() == TAG_PLAYER && otherTag == Enemy::TAG) || (getTag() == TAG_ENEMY && otherTag == Player::TAG)) {
        Util::Game::GameManager::getCurrentScene().removeObject(this);
    }
}
