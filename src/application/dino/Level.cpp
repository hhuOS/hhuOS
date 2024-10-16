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
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "Level.h"

#include <stdint.h>

#include "lib/util/io/key/Key.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/math/Vector3D.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/collection/Pair.h"
#include "application/dino/entity/Coin.h"
#include "application/dino/entity/Fruit.h"
#include "application/dino/entity/EnemyFrog.h"
#include "application/dino/entity/PlayerDino.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/ArrayList.h"

Level::Level(const Util::Io::File &levelFile, uint32_t points) : levelFile(levelFile), startPoints(points) {}

void Level::initialize() {
    auto fileStream = Util::Io::FileInputStream(levelFile);
    auto levelNumber = Util::String::parseInt(levelFile.getName().substring(5, 6));
    auto nextLevelFile =  Util::Io::File(Util::String::format("/user/dino/level/level%u.txt", levelNumber + 1));
    if (!nextLevelFile.exists()) {
        nextLevelFile = Util::Io::File("/user/dino/level/level1.txt");
    }

    int32_t x = -1;
    double y = Util::Game::GameManager::getRelativeResolution().getY() / 2;

    int32_t minX = x;
    double minY = y;

    auto grassPositions = Util::ArrayList<Util::Pair<int32_t, double>>();
    auto dirtPositions = Util::ArrayList<Util::Pair<int32_t, double>>();
    auto waterPositions = Util::ArrayList<Util::Pair<int32_t, double>>();

    auto c = fileStream.read();
    while (c != -1) {
        auto position = Util::Math::Vector2D(x * Block::SIZE - (Util::Game::GameManager::getRelativeResolution().getX() / 2), y);
        switch (c) {
            case '\n':
                x = -1;
                y -= Block::SIZE;
                break;
            case '0':
                break;
            case '1':
                dirtPositions.add(Util::Pair(x, y));
                minX = x < minX ? x : minX;
                minY = y < minY ? y : minY;
                break;
            case '2':
                grassPositions.add(Util::Pair(x, y));
                minX = x < minX ? x : minX;
                break;
            case '3':
                waterPositions.add(Util::Pair(x, y));
                break;
            case '4':
                addObject(new Block(Block::BOX, position, 1, 1));
                break;
            case '5':
                addObject(new Coin(position));
                break;
            case '6':
                addObject(new Fruit(position, nextLevelFile));
                break;
            case '7': {
                addObject(new EnemyFrog(position));
                break;
            }
            default:
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Invalid character in level file!");
        }

        c = fileStream.read();
        x++;
    }

    spawnMergedBlocks(grassPositions, Block::GRASS);
    spawnMergedBlocks(dirtPositions, Block::DIRT);
    spawnMergedBlocks(waterPositions, Block::WATER);

    playerMinX = minX * Block::SIZE;
    playerMinY = minY;

    player = new PlayerDino(Util::Math::Vector2D(playerMinX, 0));
    player->setPoints(startPoints);
    addObject(player);

    setKeyListener(*this);
}

void Level::initializeBackground(Util::Game::Graphics &graphics) {
    auto cloud1 = Util::Game::D2::Sprite("/user/dino/background/cloud1.bmp", 0.45, 0.15);
    auto cloud3 = Util::Game::D2::Sprite("/user/dino/background/cloud3.bmp", 0.6, 0.15);
    auto cloud4 = Util::Game::D2::Sprite("/user/dino/background/cloud4.bmp", 0.45, 0.15);

    graphics.clear(Util::Graphic::Color(57, 97, 255));
    cloud1.draw(graphics, Util::Math::Vector2D(-1, 0.65));
    cloud3.draw(graphics, Util::Math::Vector2D(0.2, 0.3));
    cloud4.draw(graphics, Util::Math::Vector2D(0.65, 0.7));
}

void Level::update([[maybe_unused]] double delta) {
    if (player->isDead()) {
        auto &game = Util::Game::GameManager::getGame();
        game.switchToNextScene();

        return;
    }

    if (rightPressed) {
        player->run();
        player->setDirection(PlayerDino::RIGHT);
    } else if (leftPressed) {
        player->run();
        player->setDirection(PlayerDino::LEFT);
    } else {
        player->idle();
    }

    if (player->getPosition().getX() < playerMinX) {
        player->setPosition(Util::Math::Vector2D(playerMinX, player->getPosition().getY()));
    }

    if (player->getPosition().getY() < playerMinY) {
        player->die();
    }

    auto cameraPosY = getCamera().getPosition().getY();
    if (cameraPosY + 0.8 < player->getPosition().getY()) {
        cameraPosY = player->getPosition().getY() - 0.8;
    } else if (cameraPosY - 0.8 > player->getPosition().getY()) {
        cameraPosY = player->getPosition().getY() + 0.8;
    }

    if (cameraPosY - 1 < playerMinY) {
        cameraPosY = playerMinY + 1;
    }

    getCamera().setPosition(Util::Math::Vector2D(player->getPosition().getX() + 0.8, cameraPosY));
}

void Level::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::SPACE:
            if (player->hasHatched()) {
                player->jump();
            } else {
                player->hatch();
            }
            break;
        case Util::Io::Key::RIGHT:
            rightPressed = true;
            break;
        case Util::Io::Key::LEFT:
            leftPressed = true;
            break;
    }
}

void Level::keyReleased(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::RIGHT:
            rightPressed = false;
            break;
        case Util::Io::Key::LEFT:
            leftPressed = false;
            break;
    }
}

void Level::spawnMergedBlocks(Util::ArrayList<Util::Pair<int32_t, double>> &positions, Block::Tag tag) {
    if (positions.isEmpty()) {
        return;
    }

    auto rectangleStartPosition = positions.get(0);
    auto lastPosition = positions.get(0);
    for (uint32_t i = 1; i < positions.size(); i++) {
        auto position = positions.get(i);
        lastPosition = positions.get(i - 1);
        if (position.first - 1 != lastPosition.first) {
            auto mergedBlockStart = Util::Math::Vector2D(rectangleStartPosition.first * Block::SIZE - (Util::Game::GameManager::getRelativeResolution().getX()) / 2, rectangleStartPosition.second);
            auto countX = lastPosition.first - rectangleStartPosition.first + 1;
            addObject(new Block(tag, mergedBlockStart, countX, 1));

            rectangleStartPosition = position;
        }
    }

    auto mergedBlockStart = Util::Math::Vector2D(rectangleStartPosition.first * Block::SIZE - (Util::Game::GameManager::getRelativeResolution().getX()) / 2, rectangleStartPosition.second);
    auto countX = lastPosition.first - rectangleStartPosition.first + 1;
    addObject(new Block(tag, mergedBlockStart, countX, 1));
}
