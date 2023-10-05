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

#include "DinoGame.h"

#include <cstdint>

#include "application/dino/Dino.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/2d/component/LinearMovementComponent.h"
#include "lib/util/game/2d/component/GravityComponent.h"
#include "Saw.h"
#include "lib/util/game/GameManager.h"
#include "application/dino/Ground.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/math/Vector3D.h"
#include "GameOverScreen.h"

DinoGame::DinoGame() {
    dino->addComponent(new Util::Game::D2::LinearMovementComponent(*dino));
    dino->addComponent(new Util::Game::D2::GravityComponent(*dino, 1.25, 0));

    addObject(dino);

    for (uint32_t i = 0; i < 4; i++) {
        auto *newGround = new Ground(Util::Math::Vector2D(getCamera().getPosition().getX() - 1.5 + i, -1));
        ground.offer(newGround);
        addObject(newGround);
    }

    setKeyListener(*this);
}

void DinoGame::initializeBackground(Util::Game::Graphics &graphics) {
    auto cloud1 = Util::Game::D2::Sprite("/initrd/dino/cloud1.bmp", 0.45, 0.15);
    auto cloud3 = Util::Game::D2::Sprite("/initrd/dino/cloud3.bmp", 0.6, 0.15);
    auto cloud4 = Util::Game::D2::Sprite("/initrd/dino/cloud4.bmp", 0.45, 0.15);

    graphics.clear(Util::Graphic::Color(57, 97, 255));
    cloud1.draw(graphics, Util::Math::Vector2D(-1, 0.65));
    cloud3.draw(graphics, Util::Math::Vector2D(0.2, 0.3));
    cloud4.draw(graphics, Util::Math::Vector2D(0.65, 0.7));
}

void DinoGame::update(double delta) {
    if (dino->isDead()) {
        auto &game = Util::Game::GameManager::getGame();
        game.pushScene(new GameOverScreen(dino->getPoints()));
        game.switchToNextScene();
    } else if (dino->hasHatched() && !dino->isDying()) {
        if (currentVelocity < MAX_VELOCITY) {
            currentVelocity += (delta / 100);
            if (currentVelocity >= DASH_VELOCITY) {
                dino->dash();
            }
        }

        dino->setVelocityX(currentVelocity);

        if (obstacleCooldown <= 0) {
            if (random.nextRandomNumber() < delta) {
                auto positionY = ground.peek()->getPosition().getY() + ground.peek()->getCollider().getHeight() + (random.nextRandomNumber() < 0.75 ? 0 : + 0.3);
                auto *saw = new Saw(Util::Math::Vector2D(getCamera().getPosition().getX() + 2, positionY));
                obstacles.add(saw);
                addObject(saw);
                obstacleCooldown = OBSTACLE_COOLDOWN;
            }
        } else {
            obstacleCooldown -= delta;
        }

        if (ground.peek()->getPosition().getX() < getCamera().getPosition().getX() - 2.5) {
            auto positionX = (static_cast<uint32_t>((getCamera().getPosition().getX() + 1.5) * 10) / 5) * 5 / 10.0 ;
            auto *newGround = new Ground(Util::Math::Vector2D(positionX, -1));
            removeObject(ground.poll());
            ground.offer(newGround);
            addObject(newGround);
        }

        for (auto *tree : obstacles) {
            if (tree->getPosition().getX() < getCamera().getPosition().getX() - 2) {
                obstacles.remove(tree);
                removeObject(tree);
            }
        }

        getCamera().setPosition(Util::Math::Vector2D(dino->getPosition().getX() + 0.8, 0));
        dino->setPoints(static_cast<uint32_t>(getCamera().getPosition().getX()));
    }
}

void DinoGame::keyPressed(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC :
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::SPACE :
            if (dino->hasHatched()) {
                dino->jump();
            } else {
                dino->hatch();
            }
            break;
    }
}

void DinoGame::keyReleased(Util::Io::Key key) {}
