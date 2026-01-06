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

#include "lib/pulsar/Game.h"
#include "lib/pulsar/2d/component/LinearMovementComponent.h"
#include "lib/pulsar/2d/event/TranslationEvent.h"
#include "lib/pulsar/2d/event/CollisionEvent.h"
#include "application/bug/Ship.h"
#include "lib/pulsar/Scene.h"
#include "lib/pulsar/Collider.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2.h"
#include "EnemyMissile.h"
#include "EnemyBug.h"
#include "lib/util/base/String.h"

PlayerMissile::PlayerMissile(const Util::Math::Vector2<float> &position, Ship &ship) : Pulsar::D2::Entity(TAG, position, Pulsar::D2::RectangleCollider(position, SIZE_X, SIZE_Y, Pulsar::D2::RectangleCollider::STATIC)), ship(ship) {
    addComponent(new Pulsar::D2::LinearMovementComponent());
}

void PlayerMissile::initialize() {
    sprite = Pulsar::D2::Sprite("/user/bug/player_missile.bmp", SIZE_X, SIZE_Y);
}

void PlayerMissile::onUpdate([[maybe_unused]] float delta) {}

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
