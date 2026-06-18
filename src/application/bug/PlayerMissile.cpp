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

#include "PlayerMissile.h"

#include "EnemyMissile.h"
#include "EnemyBug.h"
#include "Ship.h"

#include <util/base/String.h>
#include <util/math/Vector2.h>
#include <pulsar/2d/component/LinearMovementComponent.h>
#include <pulsar/2d/event/TranslationEvent.h>
#include <pulsar/2d/event/CollisionEvent.h>
#include <pulsar/2d/collider/RectangleCollider.h>

PlayerMissile::PlayerMissile(const Util::Math::Vector2<float> &position, Ship &ship) : Entity(TAG, position,
    Pulsar::D2::RectangleCollider(position, WIDTH, HEIGHT, Pulsar::D2::RectangleCollider::STATIC)), ship(ship)
{
    addComponent(new Pulsar::D2::LinearMovementComponent());
    setVelocityY(2);
}

void PlayerMissile::initialize() {
    sprite = Pulsar::D2::Sprite("/user/bug/player_missile.bmp", WIDTH, HEIGHT);
}

void PlayerMissile::onTranslationEvent(Pulsar::D2::TranslationEvent &event) {
    if (event.getTargetPosition().getY() > 1.0) {
        ship.allowFireMissile();
        removeFromScene();
    }
}

void PlayerMissile::onCollisionEvent(const Pulsar::D2::CollisionEvent &event) {
    const auto &otherEntity = event.getCollidedWidth();
    switch (otherEntity.getTag()) {
        case EnemyMissile::TAG: {
            const auto &missile = reinterpret_cast<const EnemyMissile&>(otherEntity);
            if (missile.isExploding()) {
                return;
            }
            break;
        }
        case EnemyBug::TAG: {
            const auto &bug = reinterpret_cast<const EnemyBug&>(otherEntity);
            if (bug.isExploding()) {
                return;
            }
            break;
        }
        default:
            break;
    }

    ship.allowFireMissile();
    removeFromScene();
}

void PlayerMissile::draw(Pulsar::Graphics &graphics) const {
    sprite.draw(graphics, getPosition());
}
