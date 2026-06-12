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

#include "Astronomical.h"
#include "Enemy.h"
#include "Missile.h"

#include <pulsar/3d/Scene.h>
#include <pulsar/3d/event/CollisionEvent.h>
#include <util/math/Math.h>
#include <util/base/String.h>
#include <util/collection/ArrayList.h>
#include <pulsar/3d/collider/SphereCollider.h>
#include <pulsar/Graphics.h>
#include <util/graphic/Colors.h>
#include <util/math/Vector2.h>
#include <pulsar/Scene.h>
#include <pulsar/Camera.h>

Player::Player(const Util::ArrayList<Enemy*> &enemies) : Entity(TAG, Util::Math::Vector3<float>(0, 0, 0),
    Util::Math::Vector3<float>(0, 0, 0), Util::Math::Vector3<float>(0, 0, 0),
        Pulsar::D3::SphereCollider(Util::Math::Vector3<float>(0, 0, 0), 0.8)),
    enemies(enemies) {}

void Player::onUpdate(const float delta) {
    if (invulnerabilityTimer > 0) {
        invulnerabilityTimer -= delta;
    }
    
    if (missileTimer > 0) {
        missileTimer -= delta;
    }
}

void Player::draw(Pulsar::Graphics &graphics) const {
    const auto dimensions = graphics.getDimensions();
    const auto &scene = reinterpret_cast<Pulsar::D3::Scene&>(getScene());
    graphics.setColor(Util::Graphic::Colors::GREEN);

    // Draw reticle
    const auto raytraceDirection = getScene().getCamera().getFrontVector();
    const auto *aimTarget = scene.findEntityUsingRaytrace(
        getPosition() + raytraceDirection, raytraceDirection, 20, 0.1);

    if (aimTarget != nullptr) {
        graphics.setColor(Util::Graphic::Colors::RED);
    }

    graphics.drawLineDirect(Util::Math::Vector2<float>(-0.025, 0), Util::Math::Vector2<float>(-0.1, 0));
    graphics.drawLineDirect(Util::Math::Vector2<float>(0.025, 0), Util::Math::Vector2<float>(0.1, 0));
    graphics.drawLineDirect(Util::Math::Vector2<float>(0, 0.025), Util::Math::Vector2<float>(0, 0.1));
    graphics.drawLineDirect(Util::Math::Vector2<float>(0, -0.025), Util::Math::Vector2<float>(0, -0.1));

    graphics.setColor(invulnerabilityTimer > 0 ? Util::Graphic::Colors::RED : Util::Graphic::Colors::GREEN);

    // Draw player stats
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05f, 0.95f),
        Util::String("Health  : "));
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05f, 0.9f),
        Util::String::format("Score   : %d", score));
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05f, 0.85f),
        Util::String::format("Enemies : %d", enemies.size()));

    const auto relativeCharWidth = Pulsar::Graphics::FONT_SIZE / static_cast<float>(graphics.getTransformation());
    const auto healthBarStart = -dimensions.getX() + 0.05f + 10 * relativeCharWidth;
    graphics.drawRectangleDirect(Util::Math::Vector2<float>(healthBarStart, 0.95),
        Util::Math::Vector2<float>(0.3, -0.025));
    graphics.fillRectangleDirect(Util::Math::Vector2<float>(healthBarStart, 0.95),
        Util::Math::Vector2<float>(0.3f * (static_cast<float>(getHealth()) / 100.0f), -0.025f));

    // Draw speedometer
    graphics.fillRectangleDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.0625f, 0),
        Util::Math::Vector2<float>(0.025f, speed * 0.95f));

    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075f, 0.95f),
        Util::Math::Vector2<float>(dimensions.getX() - 0.025f, 0.95f));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075f, 0.5f),
        Util::Math::Vector2<float>(dimensions.getX() - 0.025f, 0.5f));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075f, 0.0f),
        Util::Math::Vector2<float>(dimensions.getX() - 0.025f, 0.0f));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075f, -0.5f),
        Util::Math::Vector2<float>(dimensions.getX() - 0.025f, -0.5f));
    graphics.drawLineDirect(Util::Math::Vector2<float>(dimensions.getX() - 0.075f, -0.95f),
        Util::Math::Vector2<float>(dimensions.getX() - 0.025f, -0.95f));

    // Draw radar
    const auto headerSting = Util::String::format("P: %d  Y: %d",
        static_cast<int32_t>(getRotation().getY()), static_cast<int32_t>(getRotation().getZ()));
    graphics.drawStringDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05f, -0.96f), headerSting);

    graphics.drawSquareDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.05f, -0.95f), 0.3f);

    graphics.fillSquareDirect(Util::Math::Vector2<float>(-dimensions.getX() + 0.2f, -0.8f), 0.003f);
    graphics.setColor(Util::Graphic::Colors::RED);

    for (const auto *enemy : enemies) {
        const auto enemyTargetVector = enemy->getPosition() - getPosition();
        const auto relativeOnPlane =
            Util::Math::Vector3<float>(enemyTargetVector.getX(), 0, -enemyTargetVector.getZ())
            .rotate(Util::Math::Vector3<float>(0, -getRotation().getZ(), 0));
        const auto drawX = relativeOnPlane.getX() / 20 * 0.15f;
        const auto drawY = relativeOnPlane.getZ() / 20 * 0.15f;

        if (Util::Math::absolute(drawX) < 0.13 && Util::Math::absolute(drawY) < 0.13) {
            constexpr auto cutoffWhenOnSameHeight = 3.0f;
            if (enemyTargetVector.getY() > cutoffWhenOnSameHeight) {
                graphics.drawLineDirect(Util::Math::Vector2<float>(
                    -dimensions.getX() + 0.2f + drawX + 0.02f, -0.8f + drawY - 0.02f),
                    Util::Math::Vector2<float>(-dimensions.getX() + 0.2f + drawX, -0.8f + drawY));
                graphics.drawLineDirect(Util::Math::Vector2<float>(
                    -dimensions.getX() + 0.2f + drawX - 0.02f, -0.8f + drawY - 0.02f),
                    Util::Math::Vector2<float>(-dimensions.getX() + 0.2f + drawX, -0.8f + drawY));
            } else if (enemyTargetVector.getY() < -cutoffWhenOnSameHeight) {
                graphics.drawLineDirect(Util::Math::Vector2<float>(
                    -dimensions.getX() + 0.2f + drawX - 0.02f, -0.8f + drawY - 0.02f),
                    Util::Math::Vector2<float>(-dimensions.getX() + 0.2f + drawX, -0.8f + drawY));
                graphics.drawLineDirect(Util::Math::Vector2<float>(
                    -dimensions.getX() + 0.2f + drawX + 0.02f, -0.8f + drawY - 0.02f),
                    Util::Math::Vector2<float>(-dimensions.getX() + 0.2f + drawX, -0.8f + drawY));
            } else {
                graphics.drawLineDirect(Util::Math::Vector2<float>(
                    -dimensions.getX() + 0.2f + drawX - 0.02f, -0.8f + drawY - 0.02f),
                    Util::Math::Vector2<float>(-dimensions.getX() + 0.2f + drawX + 0.02f,
                        -0.8f + drawY + 0.02f));
                graphics.drawLineDirect(Util::Math::Vector2<float>(
                    -dimensions.getX() + 0.2f + drawX + 0.02f, -0.8f + drawY - 0.02f),
                    Util::Math::Vector2<float>(-dimensions.getX() + 0.2f + drawX - 0.02f,
                        -0.8f + drawY + 0.02f));
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
        missileTimer = MISSILE_COOLDOWN_TIME;
        return true;
    }

    return false;
}

void Player::takeDamage(const uint8_t damage) {
    if (invulnerabilityTimer <= 0) {
        invulnerabilityTimer = INVULNERABILITY_TIME;
        
        if (damage > health) {
            health = 0;
        } else {
            health -= damage;
        }
    }
}
