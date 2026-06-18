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

#include "EnemyBug.h"

#include "Fleet.h"
#include "Explosive.h"
#include "EnemyMissile.h"
#include "PlayerMissile.h"

#include <util/base/String.h>
#include <util/collection/Array.h>
#include <util/math/Vector2.h>
#include <pulsar/Game.h>
#include <pulsar/Scene.h>
#include <pulsar/2d/Entity.h>
#include <pulsar/2d/Sprite.h>
#include <pulsar/2d/component/LinearMovementComponent.h>
#include <pulsar/2d/collider/RectangleCollider.h>
#include <pulsar/2d/event/CollisionEvent.h>
#include <pulsar/2d/event/TranslationEvent.h>

EnemyBug::EnemyBug(const Util::Math::Vector2<float> &position, Fleet &fleet) : Explosive(TAG, position,
    Pulsar::D2::RectangleCollider(position, WIDTH, HEIGHT, Pulsar::D2::RectangleCollider::STATIC),
    "/user/bug/bug_explosion.wav"), fleet(fleet)
{
    addComponent(new Pulsar::D2::LinearMovementComponent());
}

void EnemyBug::initialize() {
    Explosive::initialize();

    animation = Pulsar::D2::SpriteAnimation(Util::Array<Pulsar::D2::Sprite>({
        Pulsar::D2::Sprite("/user/bug/bug1.bmp", WIDTH, HEIGHT),
        Pulsar::D2::Sprite("/user/bug/bug2.bmp", WIDTH, HEIGHT)
    }), 0.5);
}

void EnemyBug::onUpdate(const float delta) {
    Explosive::onUpdate(delta);

    if (hasExploded()) {
        // The bug is dead, and its explosion animation has finished playing.
        // We can now remove it from the scene.
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

    // Workaround for weird bug, where enemies suddenly get an x-position outside the screen range
    const auto dimX = Pulsar::Game::getInstance().getScreenDimensions().getX();
    if (event.getTargetPosition().getX() < -dimX || event.getTargetPosition().getX() > dimX) {
        event.cancel();
        return;
    }

    const auto targetX = event.getTargetPosition().getX();
    const auto maxX = dimX - WIDTH;

    // Check if the bug hits the left or right boundary
    if ((fleet.getVelocity() > 0 && targetX > maxX - WIDTH) || (fleet.getVelocity() < 0 && targetX < -maxX)) {
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

void EnemyBug::fireMissile() const {
    const auto offset = Util::Math::Vector2<float>(WIDTH / 2 - EnemyMissile::WIDTH / 2, -HEIGHT);
    auto *missile = new EnemyMissile(getPosition() + offset);
    getScene().addEntity(missile);
}
