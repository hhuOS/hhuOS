/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/math/Vector2D.h"
#include "lib/util/graphic/Font.h"

Player::Player(const Util::ArrayList<Enemy *> &enemies) : Util::Game::D3::Entity(TAG, Util::Math::Vector3D(0, 0, 0), Util::Math::Vector3D(0, 0, 0), Util::Math::Vector3D(0, 0, 0), Util::Game::D3::SphereCollider(Util::Math::Vector3D(0, 0, 0), 0.8)), enemies(enemies) {}

void Player::initialize() {}

void Player::onUpdate(double delta) {
    if (invulnerabilityTimer > 0) invulnerabilityTimer -= delta;
    if (missileTimer > 0) missileTimer -= delta;
}

void Player::draw(Util::Game::Graphics &graphics) {
    auto &font = Util::Graphic::Font::getFontForResolution(static_cast<uint32_t>(Util::Game::GameManager::getAbsoluteResolution().getY()));
    const auto &resolution = Util::Game::GameManager::getAbsoluteResolution();
    const auto centerX = resolution.getX() / 2;
    const auto centerY = resolution.getY() / 2;

    graphics.setColor(Util::Graphic::Colors::GREEN);

    // Draw reticle
    auto raytraceDirection = Util::Math::Vector3D(0, 0, 1).rotate(getRotation());
    auto raytraceOffset = raytraceDirection + getPosition();
    auto *aimTarget = Util::Game::D3::Util::findEntityUsingRaytrace(reinterpret_cast<const Util::ArrayList<Util::Game::D3::Entity*>&>(enemies), raytraceOffset, raytraceDirection, 20, 0.5);

    if (aimTarget != nullptr) {
        graphics.setColor(Util::Graphic::Colors::RED);
    }

    graphics.drawLine({centerX - 20, centerY}, {centerX - 50, centerY});
    graphics.drawLine({centerX + 20, centerY}, {centerX + 50, centerY});
    graphics.drawLine({centerX, centerY + 20}, {centerX, centerY + 50});
    graphics.drawLine({centerX, centerY - 20}, {centerX, centerY - 50});

    graphics.setColor(invulnerabilityTimer > 0 ? Util::Graphic::Colors::RED : Util::Graphic::Colors::GREEN);

    // Draw player stats
    graphics.drawString(font, Util::Math::Vector2D(10, 20), "Health  : ");
    graphics.drawString(font, Util::Math::Vector2D(10, 24 + 1 * font.getCharHeight()), Util::String::format("Score   : %d", score));
    graphics.drawString(font, Util::Math::Vector2D(10, 28 + 2 * font.getCharHeight()), Util::String::format("Enemies : %d", enemies.size()));

    graphics.drawRectangle(Util::Math::Vector2D(70, 22), 100, font.getCharHeight() - 4);
    graphics.fillRectangle(Util::Math::Vector2D(70, 22), health, font.getCharHeight() - 4);

    // Draw speedometer
    const auto speedMeterX = resolution.getX() - 15;
    const auto speedMeterScalar = 120;
    const auto speedMeterY = speedDisplay > 0 ? centerY - (speedDisplay * speedMeterScalar * 2) : centerY;
    const auto speedMeterExtend = Util::Math::absolute(speedDisplay) * speedMeterScalar * 2;
    graphics.fillRectangle(Util::Math::Vector2D(speedMeterX, speedMeterY), 10, speedMeterExtend);

    graphics.drawLine(Util::Math::Vector2D(speedMeterX - 10, centerY - speedMeterScalar * 2), Util::Math::Vector2D(speedMeterX + 10, centerY - speedMeterScalar * 2));
    graphics.drawLine(Util::Math::Vector2D(speedMeterX - 8, centerY - speedMeterScalar), Util::Math::Vector2D(speedMeterX + 8, centerY - speedMeterScalar));
    graphics.drawLine(Util::Math::Vector2D(speedMeterX - 10, centerY), Util::Math::Vector2D(speedMeterX + 10, centerY));
    graphics.drawLine(Util::Math::Vector2D(speedMeterX - 8, centerY + speedMeterScalar), Util::Math::Vector2D(speedMeterX + 8, centerY + speedMeterScalar));
    graphics.drawLine(Util::Math::Vector2D(speedMeterX - 10, centerY + speedMeterScalar * 2), Util::Math::Vector2D(speedMeterX + 10, centerY + speedMeterScalar * 2));

    // Draw radar
    auto radarLocation = Util::Math::Vector2D(55, resolution.getY() - 55);
    auto radarX = radarLocation.getX();
    auto radarY = radarLocation.getY();
    auto radarSize = 50;

    auto headerSting = Util::String::format("Y: %d  P: %d", static_cast<int32_t>(getRotation().getY()), static_cast<int32_t>(getRotation().getX()));
    graphics.drawString(font, Util::Math::Vector2D(radarX - radarSize, radarY - radarSize - 15), headerSting);

    graphics.drawLine({radarX - radarSize, radarY - radarSize}, {radarX + radarSize, radarY - radarSize});
    graphics.drawLine({radarX + radarSize, radarY - radarSize}, {radarX + radarSize, radarY + radarSize});
    graphics.drawLine({radarX + radarSize, radarY + radarSize}, {radarX - radarSize, radarY + radarSize});
    graphics.drawLine({radarX - radarSize, radarY + radarSize}, {radarX - radarSize, radarY - radarSize});

    graphics.fillSquare(radarLocation, 2);
    graphics.setColor(Util::Graphic::Colors::RED);

    for (uint32_t i = 0; i < enemies.size(); i++) {
        auto enemyLocation = enemies.get(i)->getPosition();
        auto relativeX = enemyLocation.getX() - getPosition().getX();
        auto relativeY = enemyLocation.getY() - getPosition().getY();
        auto relativeZ = enemyLocation.getZ() - getPosition().getZ();
        auto relativeOnPlane = Util::Math::Vector3D(relativeX, 0, relativeZ).rotate(Util::Math::Vector3D(0, -getRotation().getY(), 0));
        auto drawX = relativeOnPlane.getX() / 20.0 * radarSize;
        auto drawY = -relativeOnPlane.getZ() / 20.0 * radarSize;

        if (Util::Math::absolute(drawX) <= radarSize - 5 && Util::Math::absolute(drawY) <= radarSize - 5) {
            Util::String drawString = "X";

            auto cutoffWhenOnSameHeight = 3;
            if (relativeY > cutoffWhenOnSameHeight) drawString = "^";
            else if (relativeY < -cutoffWhenOnSameHeight) drawString = "v";

            graphics.drawString(font, {radarX + drawX - 3, radarY + drawY - 3}, drawString);
        }
    }
}

void Player::onTransformChange() {}

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

Util::Math::Vector3D Player::getCurrentMovementDirection() {
    return currentMovementDirection;
}

void Player::setMovementDirection(Util::Math::Vector3D direction) {
    currentMovementDirection = direction;
}

void Player::setSpeedDisplay(double speed) {
    speedDisplay = speed;
}
