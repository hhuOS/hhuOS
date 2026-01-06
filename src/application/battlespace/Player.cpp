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

#include "Player.h"

#include "lib/pulsar/3d/Scene.h"
#include "lib/pulsar/3d/event/CollisionEvent.h"
#include "lib/util/math/Math.h"
#include "Missile.h"
#include "Astronomical.h"
#include "application/battlespace/Enemy.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/pulsar/3d/collider/SphereCollider.h"
#include "lib/pulsar/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/math/Vector2.h"
#include "lib/pulsar/Scene.h"
#include "lib/pulsar/Camera.h"

Player::Player(const Util::ArrayList<Enemy *> &enemies) : Pulsar::D3::Entity(TAG, Util::Math::Vector3<float>(0, 0, 0), Util::Math::Vector3<float>(0, 0, 0), Util::Math::Vector3<float>(0, 0, 0), Pulsar::D3::SphereCollider(Util::Math::Vector3<float>(0, 0, 0), 0.8)), enemies(enemies) {}

void Player::initialize() {}

void Player::onUpdate(float delta) {
    if (invulnerabilityTimer > 0) invulnerabilityTimer -= delta;
    if (missileTimer > 0) missileTimer -= delta;
}

void Player::draw(Pulsar::Graphics &graphics) const {
    const auto dimensions = graphics.getDimensions();
    const auto &scene = reinterpret_cast<Pulsar::D3::Scene&>(getScene());
    graphics.setColor(Util::Graphic::Colors::GREEN);

    // Draw reticle
    auto raytraceDirection = getScene().getCamera().getFrontVector();
    auto *aimTarget = scene.findEntityUsingRaytrace(getPosition() + raytraceDirection, raytraceDirection, 20, 0.1);

    if (aimTarget != nullptr) {
        graphics.setColor(Util::Graphic::Colors::RED);
    }

    graphics.drawLineDirect(Util::Math::Vector2<float>(-0.025, 0), Util::Math::Vector2<float>(-0.1, 0));
    graphics.drawLineDirect(Util::Math::Vector2<float>(0.025, 0), Util::Math::Vector2<float>(0.1, 0));
    graphics.drawLineDirect(Util::Math::Vector2<float>(0, 0.025), Util::Math::Vector2<float>(0, 0.1));
    graphics.drawLineDirect(Util::Math::Vector2<float>(0, -0.025), Util::Math::Vector2<float>(0, -0.1));

    graphics.setColor(invulnerabilityTimer > 0 ? Util::Graphic::Colors::RED : Util::Graphic::Colors::GREEN);

    // Draw player stats
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05, 0.95), Util::String("Health  : "));
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05, 0.9), Util::String::format("Score   : %d", score));
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05, 0.85), Util::String::format("Enemies : %d", enemies.size()));

    const auto relativeCharWidth = static_cast<float>(Pulsar::Graphics::FONT_SIZE) / graphics.getTransformation();
    const auto healthBarStart = -dimensions.getX() + 0.05 + 10 * relativeCharWidth;
    graphics.drawRectangleDirect(Util::Math::Vector2<float>(healthBarStart, 0.95), Util::Math::Vector2<float>(0.3, -0.025));
    graphics.fillRectangleDirect(Util::Math::Vector2<float>(healthBarStart, 0.95), Util::Math::Vector2<float>(0.3 * (getHealth() / 100.0), -0.025));

    // Draw speedometer
    graphics.fillRectangleDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.0625, 0), Util::Math::Vector2<float>(0.025, speed * 0.95));

    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075, 0.95), Util::Math::Vector2<float>(dimensions.getX() - 0.025, 0.95));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075, 0.5), Util::Math::Vector2<float>(dimensions.getX() - 0.025, 0.5));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075, 0), Util::Math::Vector2<float>(dimensions.getX() - 0.025, 0));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075, -0.5), Util::Math::Vector2<float>(dimensions.getX() - 0.025, -0.5));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075, -0.95), Util::Math::Vector2<float>(dimensions.getX() - 0.025, -0.95));

    // Draw radar
    auto headerSting = Util::String::format("P: %d  Y: %d", static_cast<int32_t>(getRotation().getY()), static_cast<int32_t>(getRotation().getZ()));
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05, -0.96), headerSting);

    graphics.drawSquareDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05, -0.95), 0.3);

    graphics.fillSquareDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2, -0.8), 0.003);
    graphics.setColor(Util::Graphic::Colors::RED);

    for (uint32_t i = 0; i < enemies.size(); i++) {
        auto enemyTargetVector = enemies.get(i)->getPosition() - getPosition();
        auto relativeOnPlane = Util::Math::Vector3<float>(enemyTargetVector.getX(), 0, -enemyTargetVector.getZ()).rotate(Util::Math::Vector3<float>(0, -getRotation().getZ(), 0));
        auto drawX = relativeOnPlane.getX() / 20 * 0.15;
        auto drawY = relativeOnPlane.getZ() / 20 * 0.15;

        if (Util::Math::absolute(drawX) < 0.13 && Util::Math::absolute(drawY) < 0.13) {
            auto cutoffWhenOnSameHeight = 3;
            if (enemyTargetVector.getY() > cutoffWhenOnSameHeight) {
                graphics.drawLineDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX + 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX, -0.8 + drawY));
                graphics.drawLineDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX - 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX, -0.8 + drawY));
            } else if (enemyTargetVector.getY() < -cutoffWhenOnSameHeight) {
                graphics.drawLineDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX - 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX, -0.8 + drawY));
                graphics.drawLineDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX + 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX, -0.8 + drawY));
            } else {
                graphics.drawLineDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX - 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX + 0.02, -0.8 + drawY + 0.02));
                graphics.drawLineDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX + 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<float>(-dimensions.getX() + 0.2 + drawX - 0.02, -0.8 + drawY + 0.02));
            }
        }
    }
}

void Player::onCollisionEvent(const Pulsar::D3::CollisionEvent &event) {
    switch (event.getCollidedWidth().getTag()) {
        case Missile::TAG:
            takeDamage(10);
            break;
        case Enemy::TAG:
            takeDamage(30);
            break;
        case Astronomical::TAG:
            takeDamage(100);
        default:
            break;
    }}

bool Player::mayFireMissile() {
    if (missileTimer <= 0) {
        missileTimer = 1;
        return true;
    }

    return false;
}

int16_t Player::getHealth() const {
    return health;
}

void Player::takeDamage(uint8_t damage) {
    if (invulnerabilityTimer <= 0) {
        invulnerabilityTimer = 0.5;
        health -= damage;
    }
}

void Player::addScore(uint32_t points) {
    score += points;
}

uint32_t Player::getScore() const {
    return score;
}

Util::Math::Vector3<float> Player::getCurrentMovementDirection() {
    return currentMovementDirection;
}

void Player::setMovementDirection(Util::Math::Vector3<float> direction) {
    currentMovementDirection = direction;
}

void Player::setSpeed(float speed) {
    Player::speed = speed;
}

float Player::getSpeed() const {
    return speed;
}
