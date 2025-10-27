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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "BattleSpaceGame.h"

#include "lib/util/game/Game.h"
#include "Astronomical.h"
#include "Missile.h"
#include "GameOverScreen.h"
#include "Enemy.h"
#include "application/battlespace/Player.h"
#include "lib/util/base/String.h"
#include "lib/util/game/Camera.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"

void BattleSpaceGame::initialize() {
    setLightEnabled(false);
    setGlRenderStyle(LINES);
    setGlShadeModel(FLAT);

    addEntity(new Astronomical("planet1", Util::Math::Vector3<double>(13, -15, -1000), 150, Util::Math::Vector3<double>(1, 0, 0), Util::Graphic::Colors::BLUE));
    addEntity(new Astronomical("planet1", Util::Math::Vector3<double>(37, -8, 3000), 400, Util::Math::Vector3<double>(0.25, 0, 0), Util::Graphic::Colors::YELLOW));

    auto modelId = static_cast<uint8_t>(random.getRandomNumber() * 2 + 1);
    auto scale = 5 + random.getRandomNumber() * 10;
    auto rotation = Util::Math::Vector3<double>(random.getRandomNumber() * 5, random.getRandomNumber() * 5, random.getRandomNumber() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3<double>(-25, 12, -42), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.getRandomNumber() * 2 + 1);
    scale = 5 + random.getRandomNumber() * 10;
    rotation = Util::Math::Vector3<double>(random.getRandomNumber() * 5, random.getRandomNumber() * 5, random.getRandomNumber() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3<double>(40, 30, -55), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.getRandomNumber() * 2 + 1);
    scale = 5 + random.getRandomNumber() * 10;
    rotation = Util::Math::Vector3<double>(random.getRandomNumber() * 5, random.getRandomNumber() * 5, random.getRandomNumber() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3<double>(78, -24, 63), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.getRandomNumber() * 2 + 1);
    scale = 5 + random.getRandomNumber() * 10;
    rotation = Util::Math::Vector3<double>(random.getRandomNumber() * 5, random.getRandomNumber() * 5, random.getRandomNumber() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3<double>(-92, -74, 48), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.getRandomNumber() * 2 + 1);
    scale = 5 + random.getRandomNumber() * 10;
    rotation = Util::Math::Vector3<double>(random.getRandomNumber() * 5, random.getRandomNumber() * 5, random.getRandomNumber() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3<double>(-48, 30, 37), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.getRandomNumber() * 2 + 1);
    scale = 5 + random.getRandomNumber() * 10;
    rotation = Util::Math::Vector3<double>(random.getRandomNumber() * 5, random.getRandomNumber() * 5, random.getRandomNumber() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3<double>(10, 23, 78), scale, rotation, Util::Graphic::Colors::BROWN));

    addEntity(player);
}

void BattleSpaceGame::update(double delta) {
    if (player->getHealth() <= 0) {
        Util::Game::Game::getInstance().pushScene(new GameOverScreen(player->getScore()));
        Util::Game::Game::getInstance().switchToNextScene();
    } else {
        player->setSpeed(inputSpeed);

        auto newRotation = player->getRotation() + inputRotation * delta * 50;
        auto newPitch = newRotation.getY();
        auto playerMaxPitch = 82;

        if (newPitch > playerMaxPitch && newPitch < 360 - playerMaxPitch) {
            if (newPitch <= 180) {
                newPitch = playerMaxPitch;
            } else {
                newPitch = 360 - playerMaxPitch;
            }
        }

        auto &camera = getCamera();
        camera.setRotation(Util::Math::Vector3<double>(newRotation.getX(), newPitch, newRotation.getZ()));

        auto translation = inputTranslation + camera.getFrontVector() * player->getSpeed();
        camera.translate(translation * delta);

        player->setPosition(camera.getPosition());
        player->setRotation(camera.getRotation());

        // Spawn enemies
        if (enemies.isEmpty()) {
            difficulty++;
            double minEnemies = (difficulty / 3.0);
            double enemyCount =  minEnemies + random.getRandomNumber() * (difficulty - minEnemies);

            for (uint32_t i = 0; i < enemyCount; i++) {
                auto x = (random.getRandomNumber() * (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE / 2.0;
                auto y = (random.getRandomNumber() * (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE / 2.0;
                auto z = (random.getRandomNumber() * (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE / 2.0;
                auto type = random.getRandomNumber() * 5;

                auto *enemy = new Enemy(*player, enemies, Util::Math::Vector3<double>(x, y / 2, z), Util::Math::Vector3<double>(0, 0, 0), 1, static_cast<Enemy::Type>(type));
                enemies.add(enemy);
                addEntity(enemy);
            }
        }
    }
}

void BattleSpaceGame::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::Game::getInstance().stop();
            break;
        case Util::Io::Key::LEFT:
            inputRotation = Util::Math::Vector3<double>(inputRotation.getX(), inputRotation.getY(), -1);
        break;
        case Util::Io::Key::RIGHT:
            inputRotation = Util::Math::Vector3<double>(inputRotation.getX(), inputRotation.getY(), 1);
        break;
        case Util::Io::Key::UP:
            inputRotation = Util::Math::Vector3<double>(inputRotation.getX(), 1, inputRotation.getZ());
        break;
        case Util::Io::Key::DOWN:
            inputRotation = Util::Math::Vector3<double>(inputRotation.getX(), -1, inputRotation.getZ());
        break;
        case Util::Io::Key::W:
            inputTranslation = Util::Math::Vector3<double>(inputTranslation.getX(), 1, inputTranslation.getZ());
        break;
        case Util::Io::Key::S:
            inputTranslation = Util::Math::Vector3<double>(inputTranslation.getX(), -1, inputTranslation.getZ());
        break;
        case Util::Io::Key::A:
            inputTranslation = Util::Math::Vector3<double>(-1, inputTranslation.getY(), inputTranslation.getZ());
        break;
        case Util::Io::Key::D:
            inputTranslation = Util::Math::Vector3<double>(1, inputTranslation.getY(), inputTranslation.getZ());
        break;
        case Util::Io::Key::Q:
            inputSpeed -= 0.1;
            if (inputSpeed < -1) inputSpeed = -1;
            break;
        case Util::Io::Key::E:
            inputSpeed += 0.1;
            if (inputSpeed > 1) inputSpeed = 1;
            break;
        case Util::Io::Key::SPACE:
            if (player->mayFireMissile()) {
                const auto &camera = getCamera();
                addEntity(new Missile(player->getPosition() + camera.getFrontVector() * 2, camera.getFrontVector()));
            }
            break;

        case Util::Io::Key::ENTER: {
            const auto &camera = getCamera();
            auto *enemy = new Enemy(*player, enemies, camera.getPosition() + camera.getFrontVector() * 10, Util::Math::Vector3<double>(0, 0, 0), 1, Enemy::Type::STATIONARY);
            enemies.add(enemy);
            addEntity(enemy);
        }
        default:
            break;
    }
}

void BattleSpaceGame::keyReleased(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::LEFT:
        case Util::Io::Key::RIGHT:
            inputRotation = Util::Math::Vector3<double>(inputRotation.getX(), inputRotation.getY(), 0);
        break;
        case Util::Io::Key::UP:
        case Util::Io::Key::DOWN:
            inputRotation = Util::Math::Vector3<double>(inputRotation.getX(), 0, inputRotation.getZ());
        break;
        case Util::Io::Key::W:
        case Util::Io::Key::S:
            inputTranslation = Util::Math::Vector3<double>(inputTranslation.getX(), 0, inputTranslation.getZ());
        break;
        case Util::Io::Key::A:
        case Util::Io::Key::D:
            inputTranslation = Util::Math::Vector3<double>(0, inputTranslation.getY(), inputTranslation.getZ());
        break;
        default:
            break;
    }

}
