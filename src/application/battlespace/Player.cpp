/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Player.h"

#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/3d/Util.h"
#include "Missile.h"
#include "Astronomical.h"
#include "application/battlespace/Enemy.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/math/Vector2.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/Camera.h"

Player::Player(const Util::ArrayList<Enemy *> &enemies) : Util::Game::D3::Entity(TAG, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Game::D3::SphereCollider(Util::Math::Vector3<double>(0, 0, 0), 0.8)), enemies(enemies) {}

void Player::initialize() {}

void Player::onUpdate(double delta) {
    if (invulnerabilityTimer > 0) invulnerabilityTimer -= delta;
    if (missileTimer > 0) missileTimer -= delta;
}

void Player::draw(Util::Game::Graphics &graphics) {
    graphics.setColor(Util::Graphic::Colors::GREEN);

    // Draw reticle
    auto raytraceDirection = Util::Game::GameManager::getCurrentScene().getCamera().getFrontVector();
    auto *aimTarget = Util::Game::D3::Util::findEntityUsingRaytrace(reinterpret_cast<const Util::ArrayList<Util::Game::D3::Entity*>&>(enemies), getPosition() + raytraceDirection, raytraceDirection, 20, 0.1);

    if (aimTarget != nullptr) {
        graphics.setColor(Util::Graphic::Colors::RED);
    }

    graphics.drawLineDirect(Util::Math::Vector2<double>(-0.025, 0), Util::Math::Vector2<double>(-0.1, 0));
    graphics.drawLineDirect(Util::Math::Vector2<double>(0.025, 0), Util::Math::Vector2<double>(0.1, 0));
    graphics.drawLineDirect(Util::Math::Vector2<double>(0, 0.025), Util::Math::Vector2<double>(0, 0.1));
    graphics.drawLineDirect(Util::Math::Vector2<double>(0, -0.025), Util::Math::Vector2<double>(0, -0.1));

    graphics.setColor(invulnerabilityTimer > 0 ? Util::Graphic::Colors::RED : Util::Graphic::Colors::GREEN);

    // Draw player stats
    graphics.drawStringDirect(Util::Math::Vector2<double>(-0.95, 0.95), Util::String("Health  : "));
    graphics.drawStringDirect(Util::Math::Vector2<double>(-0.95, 0.9), Util::String::format("Score   : %d", score));
    graphics.drawStringDirect(Util::Math::Vector2<double>(-0.95, 0.85), Util::String::format("Enemies : %d", enemies.size()));

    const auto relativeCharWidth = static_cast<double>(Util::Game::Graphics::FONT_SIZE) / Util::Game::GameManager::getTransformation();
    const auto healthBarStart = -0.95 + 10 * relativeCharWidth;
    graphics.drawRectangleDirect(Util::Math::Vector2<double>(healthBarStart, 0.95), Util::Math::Vector2<double>(0.3, -0.025));
    graphics.fillRectangleDirect(Util::Math::Vector2<double>(healthBarStart, 0.95), Util::Math::Vector2<double>(0.3 * (getHealth() / 100.0), -0.025));

    // Draw speedometer
    graphics.fillRectangleDirect(Util::Math::Vector2<double>(0.9375, 0), Util::Math::Vector2<double>(0.025, speed * 0.95));

    graphics.drawLineDirect(Util::Math::Vector2<double>(0.925, 0.95), Util::Math::Vector2<double>(0.975, 0.95));
    graphics.drawLineDirect(Util::Math::Vector2<double>(0.925, 0.5), Util::Math::Vector2<double>(0.975, 0.5));
    graphics.drawLineDirect(Util::Math::Vector2<double>(0.925, 0), Util::Math::Vector2<double>(0.975, 0));
    graphics.drawLineDirect(Util::Math::Vector2<double>(0.925, -0.5), Util::Math::Vector2<double>(0.975, -0.5));
    graphics.drawLineDirect(Util::Math::Vector2<double>(0.925, -0.95), Util::Math::Vector2<double>(0.975, -0.95));

    // Draw radar
    auto headerSting = Util::String::format("P: %d  Y: %d", static_cast<int32_t>(getRotation().getY()), static_cast<int32_t>(getRotation().getZ()));
    graphics.drawStringDirect(Util::Math::Vector2<double>(-0.95, -0.96), headerSting);

    graphics.drawSquareDirect(Util::Math::Vector2<double>(-0.95, -0.95), 0.3);

    graphics.fillSquareDirect(Util::Math::Vector2<double>(-0.8, -0.8), 0.003);
    graphics.setColor(Util::Graphic::Colors::RED);

    for (uint32_t i = 0; i < enemies.size(); i++) {
        auto enemyTargetVector = enemies.get(i)->getPosition() - getPosition();
        auto relativeOnPlane = Util::Math::Vector3<double>(enemyTargetVector.getX(), 0, -enemyTargetVector.getZ()).rotate(Util::Math::Vector3<double>(0, -getRotation().getZ(), 0));
        auto drawX = relativeOnPlane.getX() / 20 * 0.15;
        auto drawY = relativeOnPlane.getZ() / 20 * 0.15;

        if (Util::Math::absolute(drawX) < 0.13 && Util::Math::absolute(drawY) < 0.13) {
            auto cutoffWhenOnSameHeight = 3;
            if (enemyTargetVector.getY() > cutoffWhenOnSameHeight) {
                graphics.drawLineDirect(Util::Math::Vector2<double>(-0.8 + drawX + 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<double>(-0.8 + drawX, -0.8 + drawY));
                graphics.drawLineDirect(Util::Math::Vector2<double>(-0.8 + drawX - 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<double>(-0.8 + drawX, -0.8 + drawY));
            } else if (enemyTargetVector.getY() < -cutoffWhenOnSameHeight) {
                graphics.drawLineDirect(Util::Math::Vector2<double>(-0.8 + drawX - 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<double>(-0.8 + drawX, -0.8 + drawY));
                graphics.drawLineDirect(Util::Math::Vector2<double>(-0.8 + drawX + 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<double>(-0.8 + drawX, -0.8 + drawY));
            } else {
                graphics.drawLineDirect(Util::Math::Vector2<double>(-0.8 + drawX - 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<double>(-0.8 + drawX + 0.02, -0.8 + drawY + 0.02));
                graphics.drawLineDirect(Util::Math::Vector2<double>(-0.8 + drawX + 0.02, -0.8 + drawY - 0.02) , Util::Math::Vector2<double>(-0.8 + drawX - 0.02, -0.8 + drawY + 0.02));
            }
        }
    }
}

void Player::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
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

Util::Math::Vector3<double> Player::getCurrentMovementDirection() {
    return currentMovementDirection;
}

void Player::setMovementDirection(Util::Math::Vector3<double> direction) {
    currentMovementDirection = direction;
}

void Player::setSpeed(double speed) {
    Player::speed = speed;
}

double Player::getSpeed() const {
    return speed;
}
