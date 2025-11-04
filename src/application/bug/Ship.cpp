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

#include "Ship.h"

#include "EnemyBug.h"
#include "lib/util/pulsar/Game.h"
#include "PlayerMissile.h"
#include "lib/util/pulsar/2d/component/LinearMovementComponent.h"
#include "lib/util/pulsar/2d/event/TranslationEvent.h"
#include "lib/util/pulsar/2d/event/CollisionEvent.h"
#include "GameOverScreen.h"
#include "EnemyMissile.h"
#include "lib/util/pulsar/Scene.h"
#include "lib/util/pulsar/Collider.h"
#include "lib/util/pulsar/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2.h"
#include "application/bug/Explosive.h"
#include "lib/util/pulsar/2d/Entity.h"
#include "lib/util/base/String.h"

Ship::Ship(const Util::Math::Vector2<double> &position) : Explosive(TAG, position, Util::Pulsar::D2::RectangleCollider(position, Util::Math::Vector2<double>(SIZE_X, SIZE_Y), Util::Pulsar::Collider::STATIC), "/user/bug/ship_explosion.wav", 2.0) {
    addComponent(new Util::Pulsar::D2::LinearMovementComponent(*this));
}

void Ship::initialize() {
    Explosive::initialize();

    sprite = Util::Pulsar::D2::Sprite("/user/bug/ship.bmp", SIZE_X, SIZE_Y);
    heart = Util::Pulsar::D2::Sprite("/user/bug/heart.bmp", 0.05, 0.05);
}

void Ship::onUpdate(double delta) {
    Explosive::onUpdate(delta);

    if (lives == 0) {
        explode();
    }

    if (hasExploded()) {
        auto &game = Util::Pulsar::Game::getInstance();
        game.pushScene(new GameOverScreen(false));
        game.switchToNextScene();
    }
}

void Ship::onTranslationEvent(Util::Pulsar::D2::TranslationEvent &event) {
    if (isExploding()) {
        event.cancel();
        return;
    }

    const auto targetX = event.getTargetPosition().getX();
    const auto maxX = Util::Pulsar::Game::getInstance().getScreenDimensions().getX();

    if (targetX > maxX - SIZE_X || targetX < -maxX) {
        event.cancel();
    }
}

void Ship::onCollisionEvent(Util::Pulsar::D2::CollisionEvent &event) {
    if (isExploding()) {
        return;
    }

    if (event.getCollidedWidth().getTag() == EnemyMissile::TAG) {
        const auto &missile = event.getCollidedWidth<const EnemyMissile>();
        if (missile.isExploding()) {
            return;
        }

        if (lives > 0) {
            lives--;
        }
    } else if (event.getCollidedWidth().getTag() == EnemyBug::TAG) {
        lives = 0;
    }
}

void Ship::draw(Util::Pulsar::Graphics &graphics) const {
    if (isExploding()) {
        Explosive::draw(graphics);
        return;
    }

    sprite.draw(graphics, getPosition());

    for (uint32_t i = 0; i < lives; i++) {
        heart.draw(graphics, Util::Math::Vector2<double>(-0.9 + i * 1.5 * heart.getSize().getX(), -0.9));
    }
}

void Ship::fireMissile() {
    if (!mayFireMissile) {
        return;
    }

    mayFireMissile = false;
    auto *missile = new PlayerMissile(getPosition() + Util::Math::Vector2<double>((SIZE_X / 2) - (PlayerMissile::SIZE_X / 2), SIZE_Y), *this);
    getScene().addEntity(missile);
    missile->setVelocityY(2);
}

void Ship::allowFireMissile() {
    mayFireMissile = true;
}
