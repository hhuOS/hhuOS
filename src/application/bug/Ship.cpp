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

#include "Ship.h"

#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "PlayerMissile.h"
#include "lib/util/game/entity/component/LinearMovementComponent.h"
#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/game/entity/event/CollisionEvent.h"
#include "GameOverScreen.h"
#include "EnemyMissile.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/entity/collider/Collider.h"
#include "lib/util/game/entity/collider/RectangleCollider.h"
#include "lib/util/math/Vector2D.h"
#include "application/bug/Explosive.h"
#include "lib/util/game/entity/Entity.h"

Ship::Ship(const Util::Math::Vector2D &position) : Explosive(TAG, position, Util::Game::RectangleCollider(position, Util::Game::Collider::STATIC, SIZE_X, SIZE_Y)) {
    addComponent(new Util::Game::LinearMovementComponent(*this));
}

void Ship::initialize() {
    Explosive::initialize();

    sprite = Util::Game::Sprite("/initrd/bug/ship.bmp", SIZE_X, SIZE_Y);
    heart = Util::Game::Sprite("/initrd/bug/heart.bmp", 0.05, 0.05);
}

void Ship::onUpdate(double delta) {
    Explosive::onUpdate(delta);

    if (lives == 0) {
        explode();
    }

    if (hasExploded()) {
        auto &game = Util::Game::GameManager::getGame();
        game.pushScene(new GameOverScreen(false));
        game.switchToNextScene();
    }
}

void Ship::onTranslationEvent(Util::Game::TranslationEvent &event) {
    if (isExploding()) {
        event.cancel();
        return;
    }

    const auto targetX = event.getTargetPosition().getX();
    const auto maxX = Util::Game::GameManager::getRelativeResolution().getX();

    if (targetX > maxX - SIZE_X || targetX < -maxX) {
        event.cancel();
    }
}

void Ship::onCollisionEvent(Util::Game::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() != EnemyMissile::TAG) {
        return;
    }

    const auto &missile = event.getCollidedWidth<const EnemyMissile>();
    if (missile.isExploding()) {
        return;
    }

    if (lives > 0) {
        lives--;
    }
}

void Ship::draw(Util::Game::Graphics2D &graphics) {
    if (isExploding()) {
        Explosive::draw(graphics);
        return;
    }

    graphics.drawImage(getPosition(), sprite.getImage());

    for (uint32_t i = 0; i < lives; i++) {
        graphics.drawImage(Util::Math::Vector2D(-0.9 + i * 1.5 * heart.getWidth(), -0.9), heart.getImage());
    }
}

void Ship::fireMissile() {
    if (!mayFireMissile) {
        return;
    }

    mayFireMissile = false;
    auto *missile = new PlayerMissile(getPosition() + Util::Math::Vector2D((SIZE_X / 2) - (PlayerMissile::SIZE_X / 2), SIZE_Y), *this);
    Util::Game::GameManager::getCurrentScene().addObject(missile);
    missile->setVelocityY(2);
}

void Ship::allowFireMissile() {
    mayFireMissile = true;
}
