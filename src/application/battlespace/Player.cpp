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

#include "Player.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/math/Math.h"

Player::Player() : Util::Game::D3::Entity(TAG, Util::Math::Vector3D(0, 0, 0), Util::Math::Vector3D(0, 0, 0), Util::Math::Vector3D(0, 0, 0), Util::Game::D3::SphereCollider(Util::Math::Vector3D(0, 0, 0), 0.8)) {}

void Player::initialize() {}

void Player::onUpdate(double delta) {
    if (invulnerabilityTimer > 0) invulnerabilityTimer -= delta;
    if (missileTimer > 0) missileTimer -= delta;
}

void Player::draw(Util::Game::Graphics &graphics) {
    const auto &resolution = Util::Game::GameManager::getAbsoluteResolution();
    const auto centerX = resolution.getX() / 2;
    const auto centerY = resolution.getY() / 2;

    graphics.setColor(Util::Graphic::Colors::GREEN);

    // Draw reticle
    /*auto raytraceDirection = Util::Math::Vector3D(0, 0, 1).rotate(getRotation());
    auto raytraceOffset = raytraceDirection + getPosition();*/

    graphics.drawLine({centerX - 20, centerY}, {centerX - 50, centerY});
    graphics.drawLine({centerX + 20, centerY}, {centerX + 50, centerY});
    graphics.drawLine({centerX, centerY + 20}, {centerX, centerY + 50});
    graphics.drawLine({centerX, centerY - 20}, {centerX, centerY - 50});

    // Draw player stats
    Util::String healthString = "Health: ";
    for (int i = 0; i < health / 10; i++) {
        healthString += "=";
    }

    const auto scoreString = Util::String::format("Score   : %d", score);
    // auto miscString = Util::String::format("Enemies : %d", allEnemies.size());

    graphics.drawString(Util::Math::Vector2D(10, 20), healthString);
    graphics.drawString(Util::Math::Vector2D(10, 40), scoreString);
    // graphics.drawString({10, 60}, miscString);

    // Draw radar
    Util::Math::Vector2D radarLocation = {55, resolution.getY() - 55};
    const auto radarX = radarLocation.getX();
    const auto radarY = radarLocation.getY();
    const auto radarSize = 50;

    graphics.drawLine({radarX - radarSize, radarY - radarSize}, {radarX + radarSize, radarY - radarSize});
    graphics.drawLine({radarX + radarSize, radarY - radarSize}, {radarX + radarSize, radarY + radarSize});
    graphics.drawLine({radarX + radarSize, radarY + radarSize}, {radarX - radarSize, radarY + radarSize});
    graphics.drawLine({radarX - radarSize, radarY + radarSize}, {radarX - radarSize, radarY - radarSize});

    graphics.fillSquare(radarLocation, 2);

    /*graphics.setColor(Util::Graphic::Colors::RED);

    for (uint16_t i = 0; i < allEnemies.size(); i++) {
        auto enemyLoc = allEnemies.get(i)->getPosition();
        auto relX = enemyLoc.getX() - getPosition().getX();
        auto relY = enemyLoc.getY() - getPosition().getY();
        auto relZ = enemyLoc.getZ() - getPosition().getZ();
        Util::Math::Vector3D relOnPlane = {relX, 0, relZ};
        auto rotRelOnPlane = relOnPlane.rotate({0, -getRotation().getY(), 0});
        auto drawX = (rotRelOnPlane.getX())/20.0 * radarSize;
        auto drawY = (-rotRelOnPlane.getZ())/20.0 * radarSize;

        if (Util::Math::absolute(drawX) <= radarSize - 5 && Util::Math::absolute(drawY) <= radarSize - 5) {
            Util::String drawString = "X";

            auto cutoffWhenOnSameHeight = 3;
            if (relY > cutoffWhenOnSameHeight) drawString = "^";
            else if (relY < -cutoffWhenOnSameHeight) drawString = "v";

            graphics.drawString({radarX + drawX - 3, radarY + drawY - 3}, drawString);
        }
    }

    graphics.setColor(Util::Graphic::Colors::GREEN);*/

    const auto headerSting = Util::String::format("Y: %d  P: %d", (int32_t) getRotation().getY(), (int32_t) getRotation().getX());
    graphics.drawString(Util::Math::Vector2D(radarX - radarSize, radarY - radarSize - 15), headerSting);

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
}

void Player::onTransformChange() {}

void Player::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {}

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

void Player::setHealth(int16_t health) {
    Player::health = health;
}

void Player::takeDamage(uint8_t damage) {
    if (invulnerabilityTimer<= 0) {
        invulnerabilityTimer = 30;
        setHealth(health - damage);
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
