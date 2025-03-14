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

#include "BattleSpaceGame.h"

#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "Astronomical.h"
#include "Missile.h"
#include "GameOverScreen.h"
#include "Enemy.h"
#include "application/battlespace/Player.h"
#include "lib/util/base/String.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/Scene.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/math/Math.h"

void BattleSpaceGame::initialize() {
    setLightEnabled(false);
    setGlRenderStyle(LINES);
    setGlShadeModel(FLAT);

    addObject(new Astronomical("planet1", Util::Math::Vector3D(13, -15, -1000), 150, Util::Math::Vector3D(1, 0, 0), Util::Graphic::Colors::BLUE));
    addObject(new Astronomical("planet1", Util::Math::Vector3D(37, -8, 3000), 400, Util::Math::Vector3D(0.25, 0, 0), Util::Graphic::Colors::YELLOW));

    auto modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    auto scale = 5 + random.nextRandomNumber() * 10;
    auto rotation = Util::Math::Vector3D(random.nextRandomNumber() * 5, random.nextRandomNumber() * 5, random.nextRandomNumber() * 5);
    addObject(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3D(-25, 12, -42), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = 5 + random.nextRandomNumber() * 10;
    rotation = Util::Math::Vector3D(random.nextRandomNumber() * 5, random.nextRandomNumber() * 5, random.nextRandomNumber() * 5);
    addObject(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3D(40, 30, -55), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = 5 + random.nextRandomNumber() * 10;
    rotation = Util::Math::Vector3D(random.nextRandomNumber() * 5, random.nextRandomNumber() * 5, random.nextRandomNumber() * 5);
    addObject(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3D(78, -24, 63), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = 5 + random.nextRandomNumber() * 10;
    rotation = Util::Math::Vector3D(random.nextRandomNumber() * 5, random.nextRandomNumber() * 5, random.nextRandomNumber() * 5);
    addObject(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3D(-92, -74, 48), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = 5 + random.nextRandomNumber() * 10;
    rotation = Util::Math::Vector3D(random.nextRandomNumber() * 5, random.nextRandomNumber() * 5, random.nextRandomNumber() * 5);
    addObject(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3D(-48, 30, 37), scale, rotation, Util::Graphic::Colors::BROWN));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = 5 + random.nextRandomNumber() * 10;
    rotation = Util::Math::Vector3D(random.nextRandomNumber() * 5, random.nextRandomNumber() * 5, random.nextRandomNumber() * 5);
    addObject(new Astronomical(Util::String::format("asteroid%u", modelId), Util::Math::Vector3D(10, 23, 78), scale, rotation, Util::Graphic::Colors::BROWN));

    addObject(player);

    setKeyListener(*this);
}

void BattleSpaceGame::update(double delta) {
    if (player->getHealth() <= 0) {
        Util::Game::GameManager::getGame().pushScene(new GameOverScreen(player->getScore()));
        Util::Game::GameManager::getGame().switchToNextScene();
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

        camera.setRotation(Util::Math::Vector3D(newRotation.getX(), newPitch, newRotation.getZ()));

        auto translation = inputTranslation + camera.getTargetVector() * player->getSpeed();
        camera.translate(translation * delta);

        player->setPosition(camera.getPosition());
        player->setRotation(camera.getRotation());

        // Spawn enemies
        if (enemies.isEmpty()) {
            difficulty++;
            double minEnemies = (difficulty / 3.0);
            double enemyCount =  minEnemies + random.nextRandomNumber() * (difficulty - minEnemies);

            for (uint32_t i = 0; i < enemyCount; i++) {
                auto x = (random.nextRandomNumber() * (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE / 2.0;
                auto y = (random.nextRandomNumber() * (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE / 2.0;
                auto z = (random.nextRandomNumber() * (ENEMY_SPAWN_RANGE * 2)) - ENEMY_SPAWN_RANGE / 2.0;
                auto type = random.nextRandomNumber() * 5;

                auto *enemy = new Enemy(*player, enemies, Util::Math::Vector3D(x, y / 2, z), Util::Math::Vector3D(0, 0, 0), 1, static_cast<Enemy::Type>(type));
                enemies.add(enemy);
                addObject(enemy);
            }
        }
    }
}

void BattleSpaceGame::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::LEFT:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), inputRotation.getY(), -1);
            break;
        case Util::Io::Key::RIGHT:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), inputRotation.getY(), 1);
            break;
        case Util::Io::Key::UP:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), 1, inputRotation.getZ());
            break;
        case Util::Io::Key::DOWN:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), -1, inputRotation.getZ());
            break;
        case Util::Io::Key::W:
            inputTranslation = Util::Math::Vector3D(0, 1, 0);
            break;
        case Util::Io::Key::S:
            inputTranslation = Util::Math::Vector3D(0, -1, 0);
            break;
        case Util::Io::Key::A:
            inputTranslation = camera.getRightVector() * -1;
            break;
        case Util::Io::Key::D:
            inputTranslation = camera.getRightVector();
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
                addObject(new Missile(player->getPosition() + camera.getTargetVector() * 2, camera.getTargetVector()));
            }
            break;

        case Util::Io::Key::ENTER: {
            auto *enemy = new Enemy(*player, enemies, camera.getPosition() + camera.getTargetVector() * 10, Util::Math::Vector3D(0, 0, 0), 1, Enemy::Type::STATIONARY);
            enemies.add(enemy);
            addObject(enemy);
        }
        default:
            break;
    }
}

void BattleSpaceGame::keyReleased(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::LEFT:
        case Util::Io::Key::RIGHT:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), inputRotation.getY(), 0);
            break;
        case Util::Io::Key::UP:
        case Util::Io::Key::DOWN:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), 0, inputRotation.getZ());
            break;
        case Util::Io::Key::W:
            inputTranslation = inputTranslation - Util::Math::Vector3D(0, 1, 0);
            break;
        case Util::Io::Key::S:
            inputTranslation = inputTranslation + Util::Math::Vector3D(0, 1, 0);
            break;
        case Util::Io::Key::A:
            inputTranslation = inputTranslation + camera.getRightVector();
            break;
        case Util::Io::Key::D:
            inputTranslation = inputTranslation - camera.getRightVector();
            break;
        default:
            break;
    }

}
