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

#include "Enemy.h"
#include "Missile.h"
#include "Player.h"
#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/3d/Util.h"

const Util::Math::Vector3D Enemy::MAX_ROTATION_DELTA = Util::Math::Vector3D(1, 1, 0);

Enemy::Enemy(Player &player, Util::ArrayList<Enemy*> &enemies, const Util::Math::Vector3D &position, const Util::Math::Vector3D &rotation, const Util::Math::Vector3D &scale, Enemy::Type type) : Util::Game::D3::Model(2, "initrd/battlespace/enemy.obj", position, rotation, scale, Util::Graphic::Colors::RED), player(player), enemies(enemies), type(type) {}

void Enemy::onUpdate(double delta) {
    if (invulnerabilityTimer > 0) invulnerabilityTimer -= delta;
    if (missileTimer > 0) missileTimer -= delta;

    auto distance = getPosition().distance(player.getPosition());
    /*auto goalTranslation = player.getPosition();

    if (distance > 2) {
        double time = distance / Missile::START_SPEED;
        double missileSlowDistance = Missile::START_SPEED * Missile::START_SPEED_TIME;

        // Calculate missile travel distance and time
        if (distance > missileSlowDistance) {
            auto missileFastDistance = distance - missileSlowDistance;
            time = Missile::START_SPEED_TIME + (missileFastDistance / Missile::FULL_SPEED);
        }

        goalTranslation = player.getPosition() + player.getCurrentMovementDirection() * (time / delta);
    }*/

    auto goalRotation = Util::Game::D3::Util::findLookAt(getPosition(), player.getPosition()) % 360;
    auto relativeRotation = goalRotation - getRotation();

    if (relativeRotation.length() > 0.1) {
        // Make sure to rotate in the right direction (whichever one is closer)
        double relativeX = relativeRotation.getX();
        if (Util::Math::absolute(relativeX) > 180) {
            if (relativeX > 180) relativeX = relativeX - 360;
            else if (relativeX < -180) relativeX = relativeX + 360;
        }

        double relativeY = relativeRotation.getY();
        if (Util::Math::absolute(relativeY) > 180) {
            if (relativeY > 180) relativeY = relativeY - 360;
            else if (relativeY < -180) relativeY = relativeY + 360;
        }

        if (relativeX > MAX_ROTATION_DELTA.getX()) {
            relativeX = MAX_ROTATION_DELTA.getX();
        } else if (relativeX < -MAX_ROTATION_DELTA.getX()) {
            relativeX = -MAX_ROTATION_DELTA.getX();
        }

        if (relativeY > MAX_ROTATION_DELTA.getY()) {
            relativeY = MAX_ROTATION_DELTA.getY();
        } else if (relativeY < -MAX_ROTATION_DELTA.getY()) {
            relativeY = -MAX_ROTATION_DELTA.getY();
        }

        rotate(Util::Math::Vector3D(relativeX, relativeY, 0));
    }

    switch (type) {
        case ORBIT_PLAYER_CLOCKWISE:
            translateLocal(Util::Math::Vector3D(0.03, 0, 0));
        case ORBIT_PLAYER_COUNTER_CLOCKWISE:
            translateLocal(Util::Math::Vector3D(-0.03, 0, 0));
        case FLY_TOWARDS_PLAYER:
            translateLocal(Util::Math::Vector3D(0, 0, 0.015));
        case KEEP_DISTANCE:
            translateLocal(distance > 3 ? Util::Math::Vector3D(0, 0, -0.015) : Util::Math::Vector3D(0, 0, 0.015));
        case STATIONARY:
        default:
            break;
    }

    if (missileTimer <= 0 && relativeRotation.length() < 2) {
        missileTimer = 2 + random.nextRandomNumber() * 2.5;
        auto offset = Util::Math::Vector3D(0, 0, 1.5).rotate(getRotation());
        Util::Game::GameManager::getCurrentScene().addObject(new Missile(getPosition() + offset, getRotation(), Util::Math::Vector3D(0.2, 0.2, 0.2), Util::Graphic::Colors::RED));
    }
}

void Enemy::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
    switch (event.getCollidedWidth().getTag()) {
        case Missile::TAG:
        case Player::TAG:
            takeDamage(50);
            break;
        default:
            break;
    }
}

int16_t Enemy::getHealth() const {
    return health;
}

void Enemy::takeDamage(uint8_t damage) {
    if (invulnerabilityTimer <= 0) {
        invulnerabilityTimer = 0.5;
        health -= damage;

        if (health <= 0) {
            player.addScore(1000);
            enemies.remove(this);
            Util::Game::GameManager::getCurrentScene().removeObject(this);
        }
    }
}
