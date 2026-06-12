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

#include "BattleSpaceGame.h"

#include "battlespace.h"
#include "Astronomical.h"
#include "Missile.h"
#include "Enemy.h"
#include "Player.h"

#include <pulsar/Game.h>
#include <util/base/String.h>
#include <pulsar/Camera.h>
#include <util/graphic/Colors.h>
#include <util/io/key/KeyEvent.h>
#include <pulsar/TextScreen.h>

void BattleSpaceGame::initialize() {
    setLightingEnabled(false);
    setGlRenderStyle(LINES);
    setGlShadeModel(FLAT);

    addEntity(new Astronomical("planet1", Util::Graphic::Colors::BLUE,
        Util::Math::Vector3<float>(13, -15, -1000), 150,
        Util::Math::Vector3<float>(1, 0, 0)));
    addEntity(new Astronomical("planet1", Util::Graphic::Colors::YELLOW,
        Util::Math::Vector3<float>(37, -8, 3000), 400,
        Util::Math::Vector3<float>(0.25, 0, 0)));

    auto modelId = static_cast<uint8_t>(random.getRandomNumber<float>() * 2 + 1);
    auto scale = 5 + random.getRandomNumber<float>() * 10;
    auto rotation = Util::Math::Vector3<float>(random.getRandomNumber<float>() * 5,
        random.getRandomNumber<float>() * 5, random.getRandomNumber<float>() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Graphic::Colors::BROWN,
        Util::Math::Vector3<float>(-25, 12, -42), scale, rotation));

    modelId = static_cast<uint8_t>(random.getRandomNumber<float>() * 2 + 1);
    scale = 5 + random.getRandomNumber<float>() * 10;
    rotation = Util::Math::Vector3<float>(random.getRandomNumber<float>() * 5,
        random.getRandomNumber<float>() * 5, random.getRandomNumber<float>() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Graphic::Colors::BROWN,
        Util::Math::Vector3<float>(40, 30, -55), scale, rotation));

    modelId = static_cast<uint8_t>(random.getRandomNumber<float>() * 2 + 1);
    scale = 5 + random.getRandomNumber<float>() * 10;
    rotation = Util::Math::Vector3<float>(random.getRandomNumber<float>() * 5,
        random.getRandomNumber<float>() * 5, random.getRandomNumber<float>() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Graphic::Colors::BROWN,
        Util::Math::Vector3<float>(78, -24, 63), scale, rotation));

    modelId = static_cast<uint8_t>(random.getRandomNumber<float>() * 2 + 1);
    scale = 5 + random.getRandomNumber<float>() * 10;
    rotation = Util::Math::Vector3<float>(random.getRandomNumber<float>() * 5,
        random.getRandomNumber<float>() * 5, random.getRandomNumber<float>() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Graphic::Colors::BROWN,
        Util::Math::Vector3<float>(-92, -74, 48), scale, rotation));

    modelId = static_cast<uint8_t>(random.getRandomNumber<float>() * 2 + 1);
    scale = 5 + random.getRandomNumber<float>() * 10;
    rotation = Util::Math::Vector3<float>(random.getRandomNumber<float>() * 5,
        random.getRandomNumber<float>() * 5, random.getRandomNumber<float>() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Graphic::Colors::BROWN,
        Util::Math::Vector3<float>(-48, 30, 37), scale, rotation));

    modelId = static_cast<uint8_t>(random.getRandomNumber<float>() * 2 + 1);
    scale = 5 + random.getRandomNumber<float>() * 10;
    rotation = Util::Math::Vector3<float>(random.getRandomNumber<float>() * 5,
        random.getRandomNumber<float>() * 5, random.getRandomNumber<float>() * 5);
    addEntity(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Graphic::Colors::BROWN,
        Util::Math::Vector3<float>(10, 23, 78), scale, rotation));

    addEntity(player);
}

void BattleSpaceGame::update(const float delta) {
    if (player->getHealth() <= 0) {
        auto &game = Pulsar::Game::getInstance();
        game.pushScene(new Pulsar::TextScreen(Util::String::format(GAME_OVER_TEXT, player->getScore()),
            handleKeyPressOnTextScreen, Util::Graphic::Colors::GREEN));
        game.switchToNextScene();
    } else {
        // Set player velocity
        player->setSpeed(inputSpeed);

        // Set player rotation (capped to 82°)
        const auto newRotation = player->getRotation() + inputRotation * delta * 50;
        auto newPitch = newRotation.getY();

        if (newPitch > PLAYER_MAX_PITCH && newPitch < 360 - PLAYER_MAX_PITCH) {
            if (newPitch <= 180) {
                newPitch = PLAYER_MAX_PITCH;
            } else {
                newPitch = 360 - PLAYER_MAX_PITCH;
            }
        }

        auto &camera = getCamera();
        camera.setRotation(Util::Math::Vector3<float>(newRotation.getX(), newPitch, newRotation.getZ()));

        // Calculate new camera/player position according to player position and speed
        const auto translation = inputTranslation + camera.getFrontVector() * player->getSpeed();
        camera.translate(translation * delta);

        player->setPosition(camera.getPosition());
        player->setRotation(camera.getRotation());

        // Spawn enemies
        if (enemies.isEmpty()) {
            difficulty++;
            const auto minEnemies = difficulty / 3;
            const auto enemyCount = random.getRandomNumber(minEnemies, difficulty - minEnemies);

            for (uint32_t i = 0; i < enemyCount; i++) {
                const auto x = random.getRandomNumber<float>() * (ENEMY_SPAWN_RANGE * 2) - ENEMY_SPAWN_RANGE / 2;
                const auto y = random.getRandomNumber<float>() * (ENEMY_SPAWN_RANGE * 2) - ENEMY_SPAWN_RANGE / 2;
                const auto z = random.getRandomNumber<float>() * (ENEMY_SPAWN_RANGE * 2) - ENEMY_SPAWN_RANGE / 2;
                const auto strategy = static_cast<Enemy::Strategy>(random.getRandomNumber(0, 4));

                auto *enemy = new Enemy(*player, enemies, strategy,
                    Util::Math::Vector3<float>(x, y / 2, z),
                    Util::Math::Vector3<float>(0, 0, 0), 1);

                enemies.add(enemy);
                addEntity(enemy);
            }
        }
    }
}

void BattleSpaceGame::keyPressed(const Util::Io::KeyEvent &key) {
    switch (key.getScancode()) {
        case Util::Io::KeyEvent::ESC:
            Pulsar::Game::getInstance().stop();
            break;
        case Util::Io::KeyEvent::LEFT:
            inputRotation = Util::Math::Vector3<float>(inputRotation.getX(), inputRotation.getY(), -1);
        break;
        case Util::Io::KeyEvent::RIGHT:
            inputRotation = Util::Math::Vector3<float>(inputRotation.getX(), inputRotation.getY(), 1);
        break;
        case Util::Io::KeyEvent::UP:
            inputRotation = Util::Math::Vector3<float>(inputRotation.getX(), 1, inputRotation.getZ());
        break;
        case Util::Io::KeyEvent::DOWN:
            inputRotation = Util::Math::Vector3<float>(inputRotation.getX(), -1, inputRotation.getZ());
        break;
        case Util::Io::KeyEvent::W:
            inputTranslation = Util::Math::Vector3<float>(inputTranslation.getX(), 1, inputTranslation.getZ());
        break;
        case Util::Io::KeyEvent::S:
            inputTranslation = Util::Math::Vector3<float>(inputTranslation.getX(), -1, inputTranslation.getZ());
        break;
        case Util::Io::KeyEvent::A:
            inputTranslation = Util::Math::Vector3<float>(-1, inputTranslation.getY(), inputTranslation.getZ());
        break;
        case Util::Io::KeyEvent::D:
            inputTranslation = Util::Math::Vector3<float>(1, inputTranslation.getY(), inputTranslation.getZ());
        break;
        case Util::Io::KeyEvent::Q:
            inputSpeed -= 0.1;
            if (inputSpeed < -1) {
                inputSpeed = -1;
            }
            break;
        case Util::Io::KeyEvent::E:
            inputSpeed += 0.1;
            if (inputSpeed > 1) {
                inputSpeed = 1;
            }
            break;
        case Util::Io::KeyEvent::SPACE:
            if (player->mayFireMissile()) {
                const auto &camera = getCamera();
                addEntity(new Missile(player->getPosition() + camera.getFrontVector() * 2,
                    camera.getFrontVector()));
            }
            break;
        default:
            break;
    }
}

void BattleSpaceGame::keyReleased(const Util::Io::KeyEvent &key) {
    switch (key.getScancode()) {
        case Util::Io::KeyEvent::LEFT:
        case Util::Io::KeyEvent::RIGHT:
            inputRotation = Util::Math::Vector3<float>(inputRotation.getX(), inputRotation.getY(), 0);
        break;
        case Util::Io::KeyEvent::UP:
        case Util::Io::KeyEvent::DOWN:
            inputRotation = Util::Math::Vector3<float>(inputRotation.getX(), 0, inputRotation.getZ());
        break;
        case Util::Io::KeyEvent::W:
        case Util::Io::KeyEvent::S:
            inputTranslation = Util::Math::Vector3<float>(inputTranslation.getX(), 0, inputTranslation.getZ());
        break;
        case Util::Io::KeyEvent::A:
        case Util::Io::KeyEvent::D:
            inputTranslation = Util::Math::Vector3<float>(0, inputTranslation.getY(), inputTranslation.getZ());
        break;
        default:
            break;
    }

}
