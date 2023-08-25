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

#include "BattleSpace.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "Asteroid.h"
#include "Missile.h"

BattleSpace::BattleSpace() {
    addObject(player);

    auto modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    auto scale = random.nextRandomNumber() * 5;
    addObject(new Asteroid(Util::Math::Vector3D(0, 0, 10), Util::Math::Vector3D(random.nextRandomNumber(), random.nextRandomNumber(), random.nextRandomNumber()), Util::Math::Vector3D(scale, scale, scale), modelId));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = random.nextRandomNumber() * 5;
    addObject(new Asteroid(Util::Math::Vector3D(12, 3, -8), Util::Math::Vector3D(random.nextRandomNumber(), random.nextRandomNumber(), random.nextRandomNumber()), Util::Math::Vector3D(scale, scale, scale), modelId));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = random.nextRandomNumber() * 5;
    addObject(new Asteroid(Util::Math::Vector3D(14, -15, 7), Util::Math::Vector3D(random.nextRandomNumber(), random.nextRandomNumber(), random.nextRandomNumber()), Util::Math::Vector3D(scale, scale, scale), modelId));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = random.nextRandomNumber() * 5;
    addObject(new Asteroid(Util::Math::Vector3D(-2, 7, 12), Util::Math::Vector3D(random.nextRandomNumber(), random.nextRandomNumber(), random.nextRandomNumber()), Util::Math::Vector3D(scale, scale, scale), modelId));

    modelId = static_cast<uint8_t>(random.nextRandomNumber() * 2 + 1);
    scale = random.nextRandomNumber() * 5;
    addObject(new Asteroid(Util::Math::Vector3D(-8, 2, -11), Util::Math::Vector3D(random.nextRandomNumber(), random.nextRandomNumber(), random.nextRandomNumber()), Util::Math::Vector3D(scale, scale, scale), modelId));

    setKeyListener(*this);
}

void BattleSpace::update(double delta) {
    if (player->getHealth() <= 0) {
        /*Util::Game::GameManager::getGame().pushScene(new BattlespaceGameOver(player->getScore()));
        Util::Game::GameManager::getGame().switchToNextScene();*/
    } else {
        player->setSpeedDisplay(inputSpeed);

        // Player rotation
        auto newRotation = player->getRotation() + inputRotation * delta * 60;
        auto x = newRotation.getX();
        auto playerMaxPitch = 82;

        if (x > playerMaxPitch && x < 360 - playerMaxPitch) {
            if (x <= 180) {
                x = playerMaxPitch;
            } else {
                x = 360 - playerMaxPitch;
            }
        }

        newRotation = Util::Math::Vector3D(x, newRotation.getY(), newRotation.getZ());
        player->setRotation(newRotation);

        // Player movement
        Util::Math::Vector3D forwardDirection = {0, 0, 0.02};
        auto translation = inputTranslate * 0.02 + forwardDirection * inputSpeed;
        player->translateLocal(translation * delta * 60);
        player->setMovementDirection(translation.rotate(player->getRotation()));

        auto &camera = Util::Game::GameManager::getGame().getCurrentScene().getCamera();
        camera.setRotation(player->getRotation());
        camera.setPosition(player->getPosition());
    }
}

void BattleSpace::keyPressed(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::LEFT:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), -1, inputRotation.getZ());
            break;
        case Util::Io::Key::RIGHT:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), 1, inputRotation.getZ());
            break;
        case Util::Io::Key::UP:
            inputRotation = Util::Math::Vector3D(-1, inputRotation.getY(), inputRotation.getZ());
            break;
        case Util::Io::Key::DOWN:
            inputRotation = Util::Math::Vector3D(1, inputRotation.getY(), inputRotation.getZ());
            break;
        case Util::Io::Key::W:
            inputTranslate = Util::Math::Vector3D(inputTranslate.getX(), 1, 0);
            break;
        case Util::Io::Key::S:
            inputTranslate = Util::Math::Vector3D(inputTranslate.getX(), -1, 0);
            break;
        case Util::Io::Key::A:
            inputTranslate = Util::Math::Vector3D(-1, inputTranslate.getY(), 0);
            break;
        case Util::Io::Key::D:
            inputTranslate = Util::Math::Vector3D(1, inputTranslate.getY(), 0);
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
                fireMissile(player->getPosition(), player->getRotation(), false);
            }
            break;
        default:
            break;
    }
}

void BattleSpace::keyReleased(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::LEFT:
        case Util::Io::Key::RIGHT:
            inputRotation = Util::Math::Vector3D(inputRotation.getX(), 0, inputRotation.getZ());
            break;
        case Util::Io::Key::UP:
        case Util::Io::Key::DOWN:
            inputRotation = Util::Math::Vector3D(0, inputRotation.getY(), inputRotation.getZ());
            break;
        case Util::Io::Key::W:
        case Util::Io::Key::S:
            inputTranslate = Util::Math::Vector3D(inputTranslate.getX(), 0, 0);
            break;
        case Util::Io::Key::A:
        case Util::Io::Key::D:
            inputTranslate = Util::Math::Vector3D(0, inputTranslate.getY(), 0);
            break;
        default:
            break;
    }

}

void BattleSpace::updateEntities(double delta) {
    Scene::updateEntities(delta);
}

void BattleSpace::fireMissile(const Util::Math::Vector3D &position, const Util::Math::Vector3D &direction, bool enemy) {
    auto offset = Util::Math::Vector3D(0, 0, 1.5).rotate(direction);
    addObject(new Missile(position + offset, direction, Util::Math::Vector3D(0.2, 0.2, 0.2), Util::Graphic::Colors::GREEN));
}

double BattleSpace::getRandomNumber() {
    return random.nextRandomNumber();
}
