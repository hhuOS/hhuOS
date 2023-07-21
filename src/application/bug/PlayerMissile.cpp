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

#include "PlayerMissile.h"

#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/entity/component/LinearMovementComponent.h"
#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/game/entity/event/CollisionEvent.h"
#include "application/bug/Ship.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/entity/collider/Collider.h"
#include "lib/util/game/entity/collider/RectangleCollider.h"
#include "lib/util/math/Vector2D.h"
#include "EnemyMissile.h"
#include "Bug.h"

namespace Util {
namespace Game {
class CollisionEvent;
}  // namespace Game
}  // namespace Util

PlayerMissile::PlayerMissile(const Util::Math::Vector2D &position, Ship &ship) : Util::Game::Entity(TAG, position, Util::Game::RectangleCollider(position, Util::Game::Collider::STATIC, SIZE_X, SIZE_Y)), ship(ship) {
    addComponent(new Util::Game::LinearMovementComponent(*this));
}

void PlayerMissile::initialize() {
    sprite = Util::Game::Sprite("/initrd/bug/player_missile.bmp", SIZE_X, SIZE_Y);
}

void PlayerMissile::onUpdate(double delta) {

}

void PlayerMissile::onTranslationEvent(Util::Game::TranslationEvent &event) {
    if (event.getTargetPosition().getY() > 1.0) {
        Util::Game::GameManager::getGame().getCurrentScene().removeObject(this);
        ship.allowFireMissile();
    }
}

void PlayerMissile::onCollisionEvent(Util::Game::CollisionEvent &event) {
    auto tag = event.getCollidedWidth().getTag();
    if (tag == EnemyMissile::TAG) {
        const auto &missile = event.getCollidedWidth<const EnemyMissile&>();
        if (!missile.isAlive()) {
            return;
        }
    } else if (tag == Bug::TAG) {
        const auto &bug = event.getCollidedWidth<const Bug&>();
        if (!bug.isAlive()) {
            return;
        }
    }


    Util::Game::GameManager::getGame().getCurrentScene().removeObject(this);
    ship.allowFireMissile();
}

void PlayerMissile::draw(Util::Game::Graphics2D &graphics) {
    graphics.drawImage(getPosition(), sprite.getImage());
}
