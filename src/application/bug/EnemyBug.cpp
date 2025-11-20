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

#include "EnemyBug.h"

#include "lib/pulsar/2d/component/LinearMovementComponent.h"
#include "lib/pulsar/2d/event/CollisionEvent.h"
#include "lib/pulsar/2d/event/TranslationEvent.h"
#include "lib/pulsar/Game.h"
#include "EnemyMissile.h"
#include "PlayerMissile.h"
#include "application/bug/Fleet.h"
#include "application/bug/Ship.h"
#include "lib/util/collection/Array.h"
#include "lib/pulsar/Scene.h"
#include "lib/pulsar/2d/Sprite.h"
#include "lib/pulsar/Collider.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2.h"
#include "application/bug/Explosive.h"
#include "lib/pulsar/2d/Entity.h"
#include "lib/util/base/String.h"

EnemyBug::EnemyBug(const Util::Math::Vector2<double> &position, Fleet &fleet) : Explosive(TAG, position, Pulsar::D2::RectangleCollider(position, SIZE_X, SIZE_Y, Pulsar::D2::RectangleCollider::STATIC), "/user/bug/bug_explosion.wav"), fleet(fleet) {
    addComponent(new Pulsar::D2::LinearMovementComponent());
}

void EnemyBug::initialize() {
    Explosive::initialize();

    animation = Pulsar::D2::SpriteAnimation(Util::Array<Pulsar::D2::Sprite>({
        Pulsar::D2::Sprite("/user/bug/bug1.bmp", SIZE_X, SIZE_Y),
        Pulsar::D2::Sprite("/user/bug/bug2.bmp", SIZE_X, SIZE_Y)}), 0.5);
}

void EnemyBug::onUpdate(double delta) {
    Explosive::onUpdate(delta);

    if (hasExploded()) {
        fleet.decreaseSize();
        removeFromScene();
        return;
    }

    animation.update(delta);

    if (fleet.isMovingDown()) {
        translateY(-0.1);
    }

    setVelocityX(fleet.getVelocity());

    lastMissileRollTime += delta;
    if (lastMissileRollTime > 0.2) {
        if (fleet.getRandomNumber() < 0.01) {
            fireMissile();
        }

        lastMissileRollTime = 0;
    }
}

void EnemyBug::onTranslationEvent(Pulsar::D2::TranslationEvent &event) {
    if (isExploding()) {
        event.cancel();
        return;
    }

    // Workaround for weird bug, where enemy suddenly gets an x-position outside the screen range
    const auto dimX = Pulsar::Game::getInstance().getScreenDimensions().getX();
    if (event.getTargetPosition().getX() < -dimX || event.getTargetPosition().getX() > dimX) {
        event.cancel();
        return;
    }

    const auto targetX = event.getTargetPosition().getX();
    const auto maxX = dimX - SIZE_X;

    if ((fleet.getVelocity() > 0 && targetX > maxX - SIZE_X) || (fleet.getVelocity() < 0 && targetX < -maxX)) {
        fleet.moveDown();
        fleet.increaseVelocity();
        fleet.changeDirection();
    }
}

void EnemyBug::onCollisionEvent(const Pulsar::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == PlayerMissile::TAG) {
        explode();
    }
}

void EnemyBug::draw(Pulsar::Graphics &graphics) const {
    if (isExploding()) {
        Explosive::draw(graphics);
    } else {
        animation.draw(graphics, getPosition());
    }
}

void EnemyBug::fireMissile() {
    auto *missile = new EnemyMissile(getPosition() + Util::Math::Vector2<double>((SIZE_X / 2) - (EnemyMissile::SIZE_X / 2), -SIZE_Y), *this);
    getScene().addEntity(missile);
    missile->setVelocityY(-1);
}
