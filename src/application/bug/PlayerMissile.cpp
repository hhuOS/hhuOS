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
 */

#include "PlayerMissile.h"

#include "lib/util/pulsar/Game.h"
#include "lib/util/pulsar/2d/component/LinearMovementComponent.h"
#include "lib/util/pulsar/2d/event/TranslationEvent.h"
#include "lib/util/pulsar/2d/event/CollisionEvent.h"
#include "application/bug/Ship.h"
#include "lib/util/pulsar/Scene.h"
#include "lib/util/pulsar/Collider.h"
#include "lib/util/pulsar/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2.h"
#include "EnemyMissile.h"
#include "EnemyBug.h"
#include "lib/util/base/String.h"

PlayerMissile::PlayerMissile(const Util::Math::Vector2<double> &position, Ship &ship) : Util::Pulsar::D2::Entity(TAG, position, Util::Pulsar::D2::RectangleCollider(position, Util::Math::Vector2<double>(SIZE_X, SIZE_Y), Util::Pulsar::Collider::STATIC)), ship(ship) {
    addComponent(new Util::Pulsar::D2::LinearMovementComponent(*this));
}

void PlayerMissile::initialize() {
    sprite = Util::Pulsar::D2::Sprite("/user/bug/player_missile.bmp", SIZE_X, SIZE_Y);
}

void PlayerMissile::onUpdate([[maybe_unused]] double delta) {}

void PlayerMissile::onTranslationEvent(Util::Pulsar::D2::TranslationEvent &event) {
    if (event.getTargetPosition().getY() > 1.0) {
        ship.allowFireMissile();
        removeFromScene();
    }
}

void PlayerMissile::onCollisionEvent(Util::Pulsar::D2::CollisionEvent &event) {
    auto tag = event.getCollidedWidth().getTag();
    if (tag == EnemyMissile::TAG) {
        const auto &missile = event.getCollidedWidth<const EnemyMissile&>();
        if (missile.isExploding()) {
            return;
        }
    } else if (tag == EnemyBug::TAG) {
        const auto &bug = event.getCollidedWidth<const EnemyBug&>();
        if (bug.isExploding()) {
            return;
        }
    }

    ship.allowFireMissile();
    removeFromScene();
}

void PlayerMissile::draw(Util::Pulsar::Graphics &graphics) const {
    sprite.draw(graphics, getPosition());
}
