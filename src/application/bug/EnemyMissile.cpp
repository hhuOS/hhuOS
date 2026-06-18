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

#include "EnemyMissile.h"

#include "PlayerMissile.h"
#include "Ship.h"

#include <util/base/String.h>
#include <util/math/Vector2.h>
#include <pulsar/2d/Entity.h>
#include <pulsar/2d/component/LinearMovementComponent.h>
#include <pulsar/2d/collider/RectangleCollider.h>
#include <pulsar/2d/event/TranslationEvent.h>
#include <pulsar/2d/event/CollisionEvent.h>

class EnemyBug;

EnemyMissile::EnemyMissile(const Util::Math::Vector2<float> &position) : Explosive(TAG, position,
    Pulsar::D2::RectangleCollider(position, WIDTH, HEIGHT, Pulsar::D2::RectangleCollider::STATIC),
    "/user/bug/ship_hit.wav")
{
    addComponent(new Pulsar::D2::LinearMovementComponent());
    setVelocityY(-1);
}

void EnemyMissile::initialize() {
    Explosive::initialize();
    sprite = Pulsar::D2::Sprite("/user/bug/enemy_missile.bmp", WIDTH, HEIGHT);
}

void EnemyMissile::onUpdate(const float delta) {
    Explosive::onUpdate(delta);

    if (hasExploded()) {
        removeFromScene();
    }
}

void EnemyMissile::onTranslationEvent(Pulsar::D2::TranslationEvent &event) {
    if (event.getTargetPosition().getY() < -1.0) {
        removeFromScene();
    }

    if (isExploding()) {
        event.cancel();
    }
}

void EnemyMissile::onCollisionEvent(const Pulsar::D2::CollisionEvent &event) {
    if (isExploding()) {
        return;
    }

    const auto tag = event.getCollidedWidth().getTag();
    if (tag == PlayerMissile::TAG || tag == Ship::TAG) {
        explode();
    }
}

void EnemyMissile::draw(Pulsar::Graphics &graphics) const {
    if (isExploding()) {
        Explosive::draw(graphics);
    } else {
        sprite.draw(graphics, getPosition());
    }
}
