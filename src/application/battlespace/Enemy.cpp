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
 *
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Enemy.h"

#include "EnemyDebris.h"
#include "Missile.h"
#include "Player.h"

#include <util/math/Math.h>
#include <util/graphic/Colors.h>
#include <util/math/Vector3.h>
#include <util/base/String.h>
#include <util/collection/ArrayList.h>
#include <pulsar/Scene.h>
#include <pulsar/3d/Entity.h>
#include <pulsar/3d/event/CollisionEvent.h>

const Util::Math::Vector3<float> Enemy::MAX_ROTATION_DELTA = Util::Math::Vector3<float>(1, 1, 0);

Enemy::Enemy(Player &player, Util::ArrayList<Enemy*> &enemies, Strategy strategy,
    const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation, const float scale) :
    Model(TAG, "/user/battlespace/enemy.obj", position, rotation,
        Util::Math::Vector3<float>(scale, scale, scale), Util::Graphic::Colors::RED),
    player(player), enemies(enemies), targetScale(scale), strategy(strategy) {}

void Enemy::initialize() {
    Model::initialize();

    // After spawning, a short animation is played
    // which lets the enemy ship grow from 10% of its size to its final size.
    setScale(Util::Math::Vector3<float>(targetScale * 0.1f, targetScale * 0.1f, targetScale * 0.1f));
}

void Enemy::onUpdate(const float delta) {
    if (getScale().length() < targetScale) {
        // The enemy ship has just spawned and the scaling animation is still in progress.
        // Increase the scale until its final scale is reached.
        setScale(getScale() * (1 + (delta * 5)));
    }

    // After being hit, the enemy is invulnerable for a short amount of time.
    // In this case, this time span is decrease until it reaches zero and the enemy ship becomes vulnerable again.
    if (invulnerabilityTimer > 0) {
        invulnerabilityTimer -= delta;
    }

    // After the enemy has shot a missile, a cooldown time starts in which it cannot shoot further missiles.
    if (missileTimer > 0) {
        missileTimer -= delta;
    }

    const auto distance = getPosition().distance(player.getPosition());
    const auto goalRotation = findLookAt(getPosition(), player.getPosition()) % 360;
    const auto relativeRotation = goalRotation - getRotation();

    /*auto goalTranslation = player.getPosition();

    if (distance > 2) {
        float time = distance / Missile::START_SPEED;
        float missileSlowDistance = Missile::START_SPEED * Missile::START_SPEED_TIME;

        // Calculate missile travel distance and time
        if (distance > missileSlowDistance) {
            auto missileFastDistance = distance - missileSlowDistance;
            time = Missile::START_SPEED_TIME + (missileFastDistance / Missile::FULL_SPEED);
        }

        goalTranslation = player.getPosition() + player.getCurrentMovementDirection() * (time / delta);
    }*/

    if (relativeRotation.length() > 0.1) {
        // Make sure to rotate in the right direction (whichever one is closer)
        float relativeX = relativeRotation.getX();
        if (Util::Math::absolute(relativeX) > 180) {
            if (relativeX > 180) relativeX = relativeX - 360;
            else if (relativeX < -180) relativeX = relativeX + 360;
        }

        float relativeY = relativeRotation.getY();
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

        rotate(Util::Math::Vector3<float>(relativeX, relativeY, 0));
    }

    switch (strategy) {
        case ORBIT_PLAYER_CLOCKWISE:
            translateLocal(Util::Math::Vector3<float>(0.03, 0, 0));
            break;
        case ORBIT_PLAYER_COUNTER_CLOCKWISE:
            translateLocal(Util::Math::Vector3<float>(-0.03, 0, 0));
            break;
        case FLY_TOWARDS_PLAYER:
            translateLocal(Util::Math::Vector3<float>(0, 0, 0.015));
            break;
        case KEEP_DISTANCE:
            translateLocal(distance > 3 ? Util::Math::Vector3<float>(0, 0, -0.015) :
                Util::Math::Vector3<float>(0, 0, 0.015));
            break;
        case STATIONARY:
        default:
            break;
    }

    // Shoot a missile if the cooldown timer is zero and the enemy ship looks roughly in the player's direction.
    if (missileTimer <= 0 && relativeRotation.length() < 2) {
        // Offset in front of the enemy ship, where the missile should span
        const auto offset = Util::Math::Vector3<float>(0, 0, 1.5).rotate(getRotation());
        // Spawn the missile
        getScene().addEntity(new Missile(player, getPosition() + offset,
            (player.getPosition() - getPosition()).normalize()));

        // Set the missile cooldown timer
        missileTimer = MIN_MISSILE_COOLDOWN_TIME + random.getRandomNumber<float>() * 2.5f;
    }
}

void Enemy::onCollisionEvent(const Pulsar::D3::CollisionEvent &event) {
    switch (event.getCollidedWidth().getTag()) {
        case TAG:
        case Missile::TAG:
        case Player::TAG:
            // Loose all health on collision
            takeDamage(health);
            break;
        default:
            break;
    }
}

void Enemy::takeDamage(const uint8_t damage) {
    // Only take damage if the enemy ship is currently vulnerable
    if (invulnerabilityTimer <= 0) {
        invulnerabilityTimer = INVULNERABILITY_TIME;

        if (damage < health) {
            health -= damage;
        } else {
            // The enemy ship is destroyed.
            // This gains the player some points and remove the enemy from the game.
            player.addScore(POINTS);
            enemies.remove(this);

            // Spawn debris objects that fly away from the enemy ship's last position.
            // This simulates an explosion.
            const auto offset1 = Util::Math::Vector3<float>(-0.3, 0.03, 0.03).rotate(getRotation());
            const auto offset2 = Util::Math::Vector3<float>(0.3, -0.02, 0.04).rotate(getRotation());
            const auto offset3 = Util::Math::Vector3<float>(-0.01, 0.17, -0.4).rotate(getRotation());

            auto &scene = getScene();
            scene.addEntity(new EnemyDebris(1, getPosition() + offset1, getRotation(), 0.3));
            scene.addEntity(new EnemyDebris(2, getPosition() + offset2, getRotation(), 0.3));
            scene.addEntity(new EnemyDebris(3, getPosition() + offset3, getRotation(), 0.3));

            removeFromScene();
        }
    }
}

Util::Math::Vector3<float> Enemy::findLookAt(const Util::Math::Vector3<float> &from,
    const Util::Math::Vector3<float> &to)
{
    const Util::Math::Vector3<float> v = to - from;
    const Util::Math::Vector3<float> norm = v.normalize();

    const auto x = norm.getX();
    const auto y = norm.getY();
    const auto z = norm.getZ();

    auto pitch = Util::Math::arcsine(y);
    auto a = x / Util::Math::cosine(pitch);

    // Fix rounding errors (|a| shouldn't actually ever exceed 1)
    if (a > 1) a = 1;
    if (a < -1) a = -1;

    auto yaw = Util::Math::arcsine(a);
    constexpr auto c = 180 / Util::Math::PI_FLOAT;

    yaw *= c;
    pitch *= c;

    // Fix mirroring issue when getting vectors behind you
    if (z < 0) yaw = -180 - yaw;

    return { -pitch, yaw, 0 };
}
