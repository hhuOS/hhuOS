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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"

#include "lib/util/game/3d/Entity.h"
#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/math/Math.h"

#include "lib/util/game/Graphics.h"
#include "lib/util/math/Vector3.h"
#include "Room.h"

Enemy::Enemy(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, Room &pRoom, Player &curPlayer, double radius) : Entity(TAG, position, rotation, Util::Math::Vector3<double>(1,1,1), Util::Game::D3::SphereCollider(position, radius)), player(curPlayer), room(pRoom) {}

void Enemy::draw(Util::Game::Graphics &graphics) {
    if (!active) {
        return;
    }

    if (type == BOSS) {
        graphics.setColor(Util::Graphic::Color(153,0,0));
        graphics.drawCuboid3D(getPosition() + Util::Math::Vector3<double>(0, 1.5, 0), Util::Math::Vector3<double>(3, 3, 3), getRotation());

        // Boss HealthBar
        graphics.setColor(Util::Graphic::Color(1, 117, 0));
        graphics.drawRectangleDirect(Util::Math::Vector2<double>(-0.50, 0.85), Util::Math::Vector2<double>(1.0, 0.1));

        graphics.setColor(Util::Graphic::Color(255,0,0));
        graphics.fillRectangleDirect(Util::Math::Vector2<double>(-0.475, 0.875), Util::Math::Vector2<double>(0.95 * (health / initHealth), 0.05));
    } else {
        graphics.setColor(Util::Graphic::Color(255,0,0));
        graphics.drawCuboid3D(getPosition(), Util::Math::Vector3<double>(1.5, 1.5, 1.5), getRotation());
    }
}

void Enemy::initialize() {}

void Enemy::setType(Type type) {
    Enemy::type = type;

    if (type == BOSS) {
        initHealth = 20;
        health = 20;
    }
}

void Enemy::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
    switch (event.getCollidedWidth().getTag()) {
        case Projectile::TAG_PLAYER:
            takeDamage(1);
            break;
        default:
            break;
    }
}

void Enemy::onUpdate(double delta) {
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

    double zRotation = Util::Math::toDegrees(Util::Math::arccosine(getFrontVector().dot(Util::Math::Vector3<double>(0,0,1))));
    if (player.getPosition().getX() < getPosition().getX()) {
        zRotation = -zRotation;
    }

    setRotation(Util::Math::Vector3<double>(0, 0, zRotation));
}

void Enemy::shoot() {
    if (cooldown <= 0) {
        Util::Game::GameManager::getCurrentScene().addObject(new Projectile(getPosition(),getFrontVector(), Projectile::TAG_ENEMY));
        cooldown = 0.9;
    }
}

void Enemy::shoot3() {
    if(cooldown<=0){
        Util::Game::GameManager::getCurrentScene().addObject(new Projectile(getPosition(),getFrontVector(), Projectile::TAG_ENEMY));
        Util::Game::GameManager::getCurrentScene().addObject(new Projectile(getPosition(),getFrontVector().rotate(Util::Math::Vector3<double>(0, 30, 0)), Projectile::TAG_ENEMY));
        Util::Game::GameManager::getCurrentScene().addObject(new Projectile(getPosition(),getFrontVector().rotate(Util::Math::Vector3<double>(0, -30, 0)), Projectile::TAG_ENEMY));
        cooldown = 1.5;
    }
}

void Enemy::setActive() {
    active = true;
}

void Enemy::takeDamage([[maybe_unused]] double damage) {
    health -= player.getDamage();
    if (health <= 0) {
        auto &scene = Util::Game::GameManager::getCurrentScene();
        room.removeEnemyFromList(this);
        scene.removeObject(this);
    }
}
