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

#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/Graphics.h"
#include "application/rogue3d/Rogue3D.h"
#include "application/rogue3d/GameOverScreen.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Item.h"
#include "Player.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector3.h"

uint32_t Player::DRAW_LIST_ID = UINT32_MAX;

Player::Player() : Entity(TAG, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1), Util::Game::D3::SphereCollider(Util::Math::Vector3<double>(0, 0, 0), 0.8)) {}

Player::Player(uint32_t damage, uint32_t health, uint32_t level) : Entity(TAG, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1), Util::Game::D3::SphereCollider(Util::Math::Vector3<double>(0, 0, 0), 0.8)), health(health), damage(damage), level(level) {}

void Player::initialize() {
    if (DRAW_LIST_ID == UINT32_MAX) {
        DRAW_LIST_ID = Util::Game::Graphics::startList3D();
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(1.5, 1.5, 1.5));
        Util::Game::Graphics::endList3D();
    }
}

void Player::onUpdate(double delta) {
    if (health <= 0) {
        Util::Game::GameManager::getGame().pushScene(new GameOverScreen(level));
        Util::Game::GameManager::getGame().switchToNextScene();
    }

    if (shootTimer > 0) {
        shootTimer -= delta * 10.0;
    }

    if (invulnerabilityTime > 0 && health > 0) {
        invulnerabilityTime -= delta;
    }
}

void Player::draw(Util::Game::Graphics &graphics) {
    graphics.setColor(invulnerabilityTime > 0 ? Util::Graphic::Color(255, 102, 102) : Util::Graphic::Color(68, 195, 212));
    graphics.drawList3D(getPosition(), getScale(), getRotation(), DRAW_LIST_ID);
}

void Player::onCollisionEvent(Util::Game::D3::CollisionEvent &event) {
    switch (event.getCollidedWidth().getTag()) {
        case Projectile::TAG_ENEMY:
        case Enemy::TAG:
            takeDamage(1 + (level - 1) / 3);
            break;
        case Item::TAG_HEALTH_UP:
            healthUp();
            break;
        case Item::TAG_DMG_UP:
            dmgUp();
            break;
        case Item::TAG_NEXT_LVL:
            Util::Game::GameManager::getGame().pushScene(new Rogue3D(new Player(damage, health, level + 1)));
            Util::Game::GameManager::getGame().switchToNextScene();
        default:
            break;
    }
}

void Player::takeDamage(uint8_t damage) {
    if (invulnerabilityTime <= 0) {
        health -= damage;
        invulnerabilityTime = 0.7;
    }
}

bool Player::shoot() {
    if (shootTimer <= 0) {
        shootTimer = 5;
        return true;
    }

    return false;
}

uint32_t Player::getHealth() const{
    return health;
}

uint32_t Player::getDamage() const{
    return damage;
}

uint32_t Player::getLevel() const{
    return level;
}

void Player::healthUp(){
    if(health < maxHealth) health += 1;
}

void Player::dmgUp(){
    if (damage < maxDmg) damage += 1;
}
