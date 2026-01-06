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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "Enemy.h"

#include "Player.h"
#include "Projectile.h"
#include "lib/pulsar/3d/Entity.h"
#include "lib/pulsar/3d/event/CollisionEvent.h"
#include "lib/pulsar/Scene.h"
#include "lib/util/math/Math.h"
#include "lib/pulsar/Graphics.h"
#include "lib/util/math/Vector3.h"
#include "Room.h"
#include "lib/pulsar/3d/collider/SphereCollider.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector2.h"

uint32_t Enemy::ENEMY_LIST_ID = UINT32_MAX;
uint32_t Enemy::BOSS_LIST_ID = UINT32_MAX;

Enemy::Enemy(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation, Room &pRoom, Player &curPlayer, float radius) : Entity(TAG, position, rotation, Util::Math::Vector3<float>(1,1,1), Pulsar::D3::SphereCollider(position, radius)), player(curPlayer), room(pRoom), health(ENEMY_INIT_HEALTH + (player.getLevel() - 1)), initHealth(health) {}

void Enemy::initialize() {
    if (ENEMY_LIST_ID == UINT32_MAX) {
        ENEMY_LIST_ID = Pulsar::Graphics::startList3D();
        Pulsar::Graphics::listCuboid3D(Util::Math::Vector3<float>(1.5, 1.5, 1.5));
        Pulsar::Graphics::endList3D();
    }

    if (BOSS_LIST_ID == UINT32_MAX) {
        BOSS_LIST_ID = Pulsar::Graphics::startList3D();
        Pulsar::Graphics::listCuboid3D(Util::Math::Vector3<float>(3, 3, 3));
        Pulsar::Graphics::endList3D();
    }
}

void Enemy::draw(Pulsar::Graphics &graphics) const {
    if (!active) {
        return;
    }

    if (type == BOSS) {
        graphics.setColor(Util::Graphic::Color(153, 0, 0));
        graphics.drawList3D(getPosition() + Util::Math::Vector3<float>(0, 1.5, 0), getScale(), getRotation(), BOSS_LIST_ID);

        // Boss health bar
        graphics.setColor(Util::Graphic::Color(1, 117, 0));
        graphics.drawRectangleDirect(Util::Math::Vector2<float>(-0.50, 0.85), Util::Math::Vector2<float>(1.0, 0.1));

        graphics.setColor(Util::Graphic::Color(255,0,0));
        graphics.fillRectangleDirect(Util::Math::Vector2<float>(-0.475, 0.875), Util::Math::Vector2<float>(0.95 * (static_cast<float>(health) / initHealth), 0.05));
    } else {
        graphics.setColor(Util::Graphic::Color(255, 0, 0));
        graphics.drawList3D(getPosition(), getScale(), getRotation(), ENEMY_LIST_ID);
    }
}

void Enemy::setType(Type type) {
    Enemy::type = type;

    if (type == BOSS) {
        initHealth = BOSS_INIT_HEALTH + 10 * (player.getLevel() - 1);
        health = initHealth;
    }
}

void Enemy::onCollisionEvent(const Pulsar::D3::CollisionEvent &event) {
    switch (event.getCollidedWidth().getTag()) {
        case Projectile::TAG_PLAYER:
            takeDamage(player.getDamage());
            break;
        default:
            break;
    }
}

void Enemy::onUpdate(float delta) {
    if (!active) {
        return;
    }

    setFrontVector(player.getPosition() - getPosition());

    switch (type) {
    case CHASING:
        translate(getFrontVector() * delta * 5);
        break;
    case ONLY_SHOOTING:
        shoot();

        if (cooldown > 0) {
            cooldown -= delta;
        }
        break;
    case BOSS:
        if (getPosition().getX() < room.getPosition().getX() - 8.5) {
            bossMovement = bossMovement * -1;
        } else if(getPosition().getX() > room.getPosition().getX() + 8.5){
            bossMovement = bossMovement*-1;
        }

        shoot3();

        if (cooldown > 0) {
            cooldown -= delta;
        }

        translate(bossMovement * delta * 5);
        break;
    default:
        break;
    }

    float zRotation = Util::Math::toDegrees(Util::Math::arccosine(getFrontVector().dotProduct(Util::Math::Vector3<float>(0,0,1))));
    if (player.getPosition().getX() < getPosition().getX()) {
        zRotation = -zRotation;
    }

    setRotation(Util::Math::Vector3<float>(0, 0, zRotation));
}

void Enemy::shoot() {
    if (cooldown <= 0) {
        getScene().addEntity(new Projectile(getPosition(),getFrontVector(), Projectile::TAG_ENEMY));
        cooldown = 0.9;
    }
}

void Enemy::shoot3() {
    if(cooldown<=0){
        getScene().addEntity(new Projectile(getPosition(),getFrontVector(), Projectile::TAG_ENEMY));
        getScene().addEntity(new Projectile(getPosition(),getFrontVector().rotate(Util::Math::Vector3<float>(0, 30, 0)), Projectile::TAG_ENEMY));
        getScene().addEntity(new Projectile(getPosition(),getFrontVector().rotate(Util::Math::Vector3<float>(0, -30, 0)), Projectile::TAG_ENEMY));
        cooldown = 1.5;
    }
}

void Enemy::setActive() {
    active = true;
}

void Enemy::takeDamage(uint8_t damage) {
    health -= damage;
    if (health <= 0) {
        room.removeEnemyFromList(this);
        removeFromScene();
    }
}
