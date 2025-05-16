/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/Scene.h"
#include "Room.h"
#include "Item.h"
#include "Enemy.h"
#include "lib/util/math/Random.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/base/Exception.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector3.h"

uint32_t Room::DRAW_LIST_ID = UINT32_MAX;

Room::Room(const Util::Math::Vector3<double> &position, Type type, uint32_t pRow, uint32_t pColumn) : Entity(TAG, position, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1)), type(type), row(pRow), column(pColumn) {}

void Room::initialize() {
    if (DRAW_LIST_ID == UINT32_MAX) {
        DRAW_LIST_ID = Util::Game::Graphics::startList3D();
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(20, 1, 20), Util::Graphic::Color(156, 109, 55)); // Floor
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(0, 1.5, -11.5), Util::Math::Vector3<double>(20, 4, 3), Util::Graphic::Color(204, 124, 64)); // Northern wall
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(-11.5, 1.5, 0), Util::Math::Vector3<double>(3, 4, 26), Util::Graphic::Color(204, 124, 64)); // Western wall
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(11.5, 1.5, 0), Util::Math::Vector3<double>(3, 4, 26), Util::Graphic::Color(204, 124, 64)); // Eastern wall
        Util::Game::Graphics::listCuboid3D(Util::Math::Vector3<double>(0, 1.5, 11.5), Util::Math::Vector3<double>(20, 4, 3), Util::Graphic::Color(204, 124, 64)); // Southern wall
        Util::Game::Graphics::endList3D();
    }
}

void Room::onUpdate([[maybe_unused]] double delta) {
    if (enemies.isEmpty() && cleared == false){
        clearAndRollItems();
    }
}

void Room::draw(Util::Game::Graphics &graphics) {
    if (!current) {
        return;
    }

    graphics.drawList3D(getPosition(), getScale(), getRotation(), DRAW_LIST_ID);

    if (cleared) {
        graphics.setColor(Util::Graphic::Color(0, 255, 0));

        if (rightRoom != nullptr) {
            graphics.drawCuboid3D(getPosition() + Util::Math::Vector3<double>(9.5, 0.5, 0.0), Util::Math::Vector3<double>(1, 0.1, 1), Util::Math::Vector3<double>(0, 0, 0));
        }

        if (leftRoom != nullptr) {
            graphics.drawCuboid3D(getPosition() + Util::Math::Vector3<double>(-9.5, 0.5, 0.0), Util::Math::Vector3<double>(1, 0.1, 1), Util::Math::Vector3<double>(0, 0, 0));
        }

        if (topRoom != nullptr) {
            graphics.drawCuboid3D(getPosition() + Util::Math::Vector3<double>(0, 0.5, -9.5), Util::Math::Vector3<double>(1, 0.1, 1), Util::Math::Vector3<double>(0, 0, 0));
        }

        if (downRoom != nullptr) {
            graphics.drawCuboid3D(getPosition() + Util::Math::Vector3<double>(0, 0.50, 8.5), Util::Math::Vector3<double>(1, 0.1, 1), Util::Math::Vector3<double>(0, 0, 0));
        }
    }
}

void Room::onCollisionEvent([[maybe_unused]] Util::Game::D3::CollisionEvent &event) {}

void Room::enterRoom() {
    entered = true;
    current = true;

    if (!enemies.isEmpty()) {
        for (uint32_t i = 0; i < enemies.size(); i++){
            enemies.get(i)->setActive();
        }
    }
}

void Room::leaveRoom(){
    current = false;
}

void Room::clearAndRollItems() {
    cleared = true;
    auto random = Util::Math::Random();
    const auto rand = static_cast<uint32_t>(random.nextRandomNumber() * 6);

    if (type == END) {
        Util::Game::GameManager::getCurrentScene().addObject(new Item(getPosition() + Util::Math::Vector3<double>(3.0, 1.0, 0.0), Item::TAG_HEALTH_UP));
        Util::Game::GameManager::getCurrentScene().addObject(new Item(getPosition() + Util::Math::Vector3<double>(-3.0, 1.0, 0.0), Item::TAG_DMG_UP));
        Util::Game::GameManager::getCurrentScene().addObject(new Item(getPosition() + Util::Math::Vector3<double>(8.5, 1.0, -7.0), Item::TAG_NEXT_LVL));
    } else {
        switch (rand) {
            case 0: // Add health item
                Util::Game::GameManager::getCurrentScene().addObject(new Item(getPosition() + Util::Math::Vector3<double>(0.0, 1.0, 0.0), Item::TAG_HEALTH_UP));
                break;
            case 1: // Add damage item
                Util::Game::GameManager::getCurrentScene().addObject(new Item(getPosition() + Util::Math::Vector3<double>(0.0, 1.0, 0.0), Item::TAG_DMG_UP));
                break;
            default:
                break;
        }
    }
}

void Room::addEnemiesToScene() const {
    for (uint32_t i = 0;i < enemies.size(); i++) {
        Util::Game::GameManager::getGame().getCurrentScene().addObject(enemies.get(i));
    }
}

void Room::generateBoss(Player &player) {
    auto *enemy = new Enemy(getPosition() - Util::Math::Vector3<double>(0, 0, 7), Util::Math::Vector3<double>(0, 0, 0), *this, player, 2);
    enemy->setType(Enemy::BOSS);
    enemies.add(enemy);
}

void Room::generateEnemies(Util::Math::Random &random, Player &player) {
    const auto rand = static_cast<uint32_t>(random.nextRandomNumber() * 4 + 1);

    for (uint32_t i = 0; i < rand; i++) {
        auto *enemy = new Enemy(getPosition(), Util::Math::Vector3<double>(0, 0, 0), *this, player, 0.7);
        auto typeRandomizer = static_cast<Enemy::Type>(random.nextRandomNumber() * 2);
        enemy->setType(typeRandomizer);
        auto position = enemy->getPosition();

        switch (i) {
            case 0:
            enemy->setPosition(position + Util::Math::Vector3<double>(-7, 1.5, -7));
            break;
        case 1:
            enemy->setPosition(position + Util::Math::Vector3<double>(7, 1.5, -7));
            break;
        case 2:
            enemy->setPosition(position + Util::Math::Vector3<double>(7, 1.5, 7));
            break;
        case 3:
            enemy->setPosition(position + Util::Math::Vector3<double>(-7, 1.5, 7));
            break;
        default:
            break;
        }

        enemies.add(enemy);
    }
}

void Room::removeEnemyFromList(Enemy *enemy) {
    enemies.remove(enemy);
}

void Room::enterRightRoom() const {
    if(rightRoom != nullptr){
        rightRoom->enterRoom();
    }
}

Room::Type Room::getType() const {
    return type;
}

bool Room::hasTopRoom() const {
    return topRoom != nullptr;
}

bool Room::hasDownRoom() const {
    return downRoom != nullptr;
}

bool Room::hasLeftRoom() const {
    return leftRoom != nullptr;
}

bool Room::hasRightRoom() const {
    return rightRoom != nullptr;
}

Room& Room::getLeftRoom() const {
    if (leftRoom == nullptr) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Rogue3D: Room not found!");
    }

    return *leftRoom;
}

Room& Room::getTopRoom() const {
    if (topRoom == nullptr) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Rogue3D: Room not found!");
    }

    return *topRoom;
}

Room& Room::getDownRoom() const {
    if (downRoom == nullptr) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Rogue3D: Room not found!");
    }

    return *downRoom;
}

Room& Room::getRightRoom() const {
    if (rightRoom == nullptr) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Rogue3D: Room not found!");
    }

    return *rightRoom;
}

uint32_t Room::getRow() const{
    return row;
}

uint32_t Room::getColumn() const{
    return column;
}

bool Room::isCleared() const{
    return cleared;
}

void Room::setRightRoom(Room &pRightRoom){
    rightRoom = &pRightRoom;
}

void Room::setLeftRoom(Room &pLeftRoom){
    leftRoom = &pLeftRoom;
}

void Room::setTopRoom(Room &pTopRoom){
    topRoom = &pTopRoom;
}

void Room::setDownRoom(Room &pDownRoom){
    downRoom = &pDownRoom;
}
